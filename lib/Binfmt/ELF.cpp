// Copyright 2021 Alex Brachet (alex@brachet.dev)
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//     http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <elf.h>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <optional>
#include <string_view>
#include <type_traits>
#include <utility>

#include "cedo/Binfmt/Binfmt.h"
#include "cedo/Core/ErrorOr.h"
#include "cedo/Core/FileReader.h"

#include "ELF.h"

using namespace std::string_literals;

namespace ELF {

std::optional<AddressSize> getAddressSize(uint8_t e) {
  if (e != 1 && e != 2)
    return {};
  return e == 1 ? AddressSize::Four : AddressSize::Eight;
}

std::optional<Endianness> getEndianness(uint8_t e) {
  if (e != 1 && e != 2)
    return {};
  return e == 1 ? Endianness::Little : Endianness::Big;
}

std::optional<Triple> acceptor(const FileReader &file) {
  assert(!magic.compare({file.getFileBuffer(), 4}) &&
         "ELF::acceptor shouldn't have been called if magic was incorrect");
  if (file.getFileSize() <= 6)
    return {};

  const uint8_t *ehdr = reinterpret_cast<const uint8_t *>(file.getFileBuffer());
  Triple t{FileFormat::ELF};

  if (auto addrSize = getAddressSize(ehdr[4]))
    t.addrSize = *addrSize;
  else
    return {};

  if (auto endianness = getEndianness(ehdr[5]))
    t.endianness = *endianness;
  else
    return {};

  return t;
}

template <Endianness endianness, AddressSize addrSize>
class ELFReaderImpl final : public Reader {
  using Ehdr = std::conditional_t<addrSize == AddressSize::Eight, Elf64_Ehdr,
                                  Elf32_Ehdr>;
  using Shdr = std::conditional_t<addrSize == AddressSize::Eight, Elf64_Shdr,
                                  Elf32_Shdr>;
  using Rel =
      std::conditional_t<addrSize == AddressSize::Eight, Elf64_Rel, Elf32_Rel>;
  using Rela = std::conditional_t<addrSize == AddressSize::Eight, Elf64_Rela,
                                  Elf32_Rela>;
  using Sym =
      std::conditional_t<addrSize == AddressSize::Eight, Elf64_Sym, Elf32_Sym>;

  template <typename RelType>
  const RelType *getRelocForOffset(const Shdr &shdr, uint64_t offset) const {
    uint64_t numRelocs = shdr.sh_size / sizeof(RelType);
    const RelType *relocs =
        reinterpret_cast<const RelType *>(getSectionAddr(shdr));

    auto it =
        std::find_if(relocs, relocs + numRelocs, [offset](const RelType &relo) {
          return relo.r_offset == offset;
        });
    return it == relocs + numRelocs ? nullptr : it;
  }

  template <typename RelType>
  std::pair<uint64_t, uint64_t> getRelocTypeAndSym(const RelType &rel) const {
    if constexpr (addrSize == AddressSize::Four)
      return {ELF32_R_TYPE(rel.r_info), ELF32_R_SYM(rel.r_info)};
    return {ELF64_R_TYPE(rel.r_info), ELF64_R_SYM(rel.r_info)};
  }

  ErrorOr<uint64_t> getSymValue(const Sym &sym) const {
    auto shdrTabOrErr = getShdrTable();
    if (!shdrTabOrErr)
      return shdrTabOrErr.getError();

    auto [table, size] = *shdrTabOrErr;
    if (sym.st_shndx >= size)
      return "sym.st_shndx is larger than section header size"s;

    return table[sym.st_shndx].sh_offset + sym.st_value;
  }

  template <typename RelType>
  ErrorOr<uint64_t> resolveLocalDefinedReloc(const RelType &rel) const {
    ErrorOr<const Shdr &> symtabShdrOrErr = getSectionHeader(".symtab");
    if (!symtabShdrOrErr)
      return symtabShdrOrErr.getError();

    const Sym *symtab =
        reinterpret_cast<const Sym *>(getSectionAddr(*symtabShdrOrErr));
    size_t numSyms = symtabShdrOrErr->sh_size / sizeof(Sym);
    auto [type, sym] = getRelocTypeAndSym(rel);

    assert(type == R_X86_64_32 ||
           type == R_X86_64_64 && "Can only handle these basic relocs for now");

    if (sym >= numSyms)
      return "Relocation symbol '"s + std::to_string(sym) +
             "' is too large for symtab of size '" + std::to_string(numSyms) +
             '\'';

    ErrorOr<uint64_t> symValOrErr = getSymValue(symtab[sym]);
    if (!symValOrErr)
      return symValOrErr.getError();

    return *symValOrErr;
  }

  ErrorOr<std::pair<const Shdr *, size_t>> getShdrTable() const {
    const Ehdr &ehdr =
        *reinterpret_cast<const Ehdr *>(getFileReader().getFileBuffer());
    if (!ehdr.e_shoff)
      return "No section headers in binary"s;

    const Shdr *shdr = reinterpret_cast<const Shdr *>(
        getFileReader().getFileBuffer() + ehdr.e_shoff);
    if (ehdr.e_shstrndx >= ehdr.e_shnum)
      return "String table section larger than known sections"s;

    return std::pair<const Shdr *, size_t>{shdr, ehdr.e_shnum};
  }

  ErrorOr<const Shdr &> getSectionHeader(std::string_view name) const {
    const Ehdr &ehdr =
        *reinterpret_cast<const Ehdr *>(getFileReader().getFileBuffer());
    auto shdrOrErr = getShdrTable();
    if (!shdrOrErr)
      return shdrOrErr.getError();
    const Shdr *shdr = shdrOrErr->first;

    const Shdr &shstr = shdr[ehdr.e_shstrndx];
    const char *const strtab =
        getFileReader().getFileBuffer() + shstr.sh_offset;

    for (const Shdr *currentSection = shdr, *end = shdr + ehdr.e_shnum;
         currentSection != end; currentSection++)
      if (name == std::string_view{strtab + currentSection->sh_name})
        return *currentSection;

    return "Couldn't find section '"s + name.data() + '\'';
  }

  // TODO maybe error check...
  const uint8_t *getSectionAddr(const Shdr &shdr) const {
    return reinterpret_cast<const uint8_t *>(getFileReader().getFileBuffer()) +
           shdr.sh_offset;
  }

public:
  ELFReaderImpl(FileReader &&file) : Reader(std::move(file)) {}

  // TODO: don't assume same endianness as currently running on.
  const uint8_t *getSection(std::string_view name) const override {
    ErrorOr<const Shdr &> shdrOrErr = getSectionHeader(name);
    if (!shdrOrErr)
      return nullptr;
    return getSectionAddr(*shdrOrErr);
  }

  // TODO right now only Rela is being handled because it's all that's needed
  // but eventually handle just Rel's too.
  ErrorOr<const uint8_t *>
  attemptResolveLocalReloc(std::string_view section_name,
                           uint64_t offset) const override {
    std::string relaSection = ".rela"s + section_name.data();
    ErrorOr<const Shdr &> relShdrOrErr = getSectionHeader(relaSection);
    if (!relShdrOrErr)
      return relShdrOrErr.getError();

    const Rela *rela = getRelocForOffset<Rela>(*relShdrOrErr, offset);
    if (!rela)
      return "Couldn't find relocation at offset"s;

    ErrorOr<uint64_t> offsetOrErr = resolveLocalDefinedReloc(*rela);
    if (!offsetOrErr)
      return offsetOrErr.getError();

    return reinterpret_cast<const uint8_t *>(getFileReader().getFileBuffer()) +
           *offsetOrErr + rela->r_addend;
  }

  Triple getTriple() const override {
    return {FileFormat::ELF, addrSize, endianness};
  }
};

std::unique_ptr<Reader> Reader::create(FileReader &&file, Triple t) {
  assert(t.fileFormat == FileFormat::ELF);

  if (t.endianness == Endianness::Little && t.addrSize == AddressSize::Eight)
    return std::make_unique<
        ELFReaderImpl<Endianness::Little, AddressSize::Eight>>(std::move(file));
  if (t.endianness == Endianness::Little && t.addrSize == AddressSize::Four)
    return std::make_unique<
        ELFReaderImpl<Endianness::Little, AddressSize::Four>>(std::move(file));
  if (t.endianness == Endianness::Big && t.addrSize == AddressSize::Eight)
    return std::make_unique<ELFReaderImpl<Endianness::Big, AddressSize::Eight>>(
        std::move(file));
  if (t.endianness == Endianness::Big && t.addrSize == AddressSize::Four)
    return std::make_unique<ELFReaderImpl<Endianness::Big, AddressSize::Four>>(
        std::move(file));
  return nullptr;
}

std::unique_ptr<Reader> Reader::create(FileReader &&file) {
  std::optional<Triple> triple = ELF::acceptor(file);
  return triple ? create(std::move(file), *triple) : nullptr;
}

} // namespace ELF
