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

#include <cassert>
#include <cstdint>
#include <optional>
#include <string_view>
#include <type_traits>

#include "cedo/Binfmt/Binfmt.h"
#include "cedo/Core/FileReader.h"

#include "ELF.h"

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

public:
  ELFReaderImpl(FileReader &&file) : Reader(std::move(file)) {}

  // TODO: don't assume same endianness as currently running on.
  const uint8_t *getSection(std::string_view name) override {
    const Ehdr &ehdr = *reinterpret_cast<const Ehdr *>(file.getFileBuffer());
    if (!ehdr.e_shoff)
      return nullptr;

    const Shdr *shdr =
        reinterpret_cast<const Shdr *>(file.getFileBuffer() + ehdr.e_shoff);
    if (ehdr.e_shstrndx >= ehdr.e_shnum)
      return nullptr;
    const Shdr &shstr = shdr[ehdr.e_shstrndx];
    const char *const strtab = file.getFileBuffer() + shstr.sh_offset;

    for (const Shdr *currentSection = shdr, *end = shdr + ehdr.e_shnum;
         currentSection != end; currentSection++)
      if (name == std::string_view{strtab + currentSection->sh_name})
        return reinterpret_cast<const uint8_t *>(file.getFileBuffer()) +
               currentSection->sh_offset;
    return nullptr;
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
