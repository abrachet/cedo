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

#include <ostream>
#include <map>

#include <cstdint>

#include "cedo/Backend/AsmStreamer.h"
#include "cedo/Backend/EmitAsm.h"
#include "cedo/Binfmt/Binfmt.h"


static size_t findAlignment(const Sym &sym) {
  auto &[_, type, addr] = sym;
  for (uintptr_t i = 1; i < type->getObjectSize(); i++)
    if (!(reinterpret_cast<uintptr_t>(addr) % i))
      return i;
  return type->getObjectSize();
}

static const std::map<FileFormat, std::map<size_t, AsmStreamer::Directive>> directives {
  { FileFormat::ELF,
    {
      {8, {".quad"}},
      {4, {".long"}},
      {2, {".value"}},
      {1, {".byte"}}
    }
  }
};

static std::pair<size_t, AsmStreamer::Directive> findLargestType(Triple triple, size_t size) {
  const auto &directiveMap = directives.find(triple.fileFormat)->second;

  auto it = directiveMap.rbegin();

  for (size_t addrSize = getAddrSize(triple.addrSize); it->first > addrSize; it++);

  // If we reached rend there was an issue somewhere else anyway so don't bother checking...
  for (; size < it->first; it++);

  assert(it->first <= size);
  return *it;
}

void AsmEmitter::emitForSize(size_t size, const uint8_t *addr) {
  if (size == 8)
    return (void) (stream << *reinterpret_cast<const uint64_t *>(addr));
  if (size == 4)
    return (void) (stream << *reinterpret_cast<const uint32_t *>(addr));
  if (size == 2)
    return (void) (stream << *reinterpret_cast<const uint16_t *>(addr));
  assert(size == 1);
  return (void) (stream << *addr);
}

void AsmEmitter::emitValueForIntegralType(const Type& type, const uint8_t *addr) {
  for (size_t remainingBytes = type.getObjectSize(); remainingBytes; ) {
    auto pair = findLargestType(outputTriple, remainingBytes);
    auto &[size, directive] = pair;
    stream << directive << ' ';
    emitForSize(size, addr);
    stream << '\n';
    addr += size;
    remainingBytes -= size;
  }
}

void AsmEmitter::emitOneSym(const Sym &sym) {
  auto &[name, type, addr] = sym;
  stream << AsmStreamer::Directive{".type"} << ' ' << name << ",@object";
  stream << AsmStreamer::Directive{".size"} << ' ' << name << ", "
         << type->getObjectSize();
  stream << AsmStreamer::Directive{".global"} << ' ' << name;
  stream << AsmStreamer::Directive{".align"} << ' ' << findAlignment(sym);
  stream << AsmStreamer::Label{name};
  emitValueForIntegralType(*type, reinterpret_cast<const uint8_t *>(addr));
  stream << '\n';
}

void AsmEmitter::emitFilePrologue() {
  stream << AsmStreamer::Directive{".data"};
}

void AsmEmitter::emitFileEpilogue(std::string_view versionStr) {
  stream << AsmStreamer::Directive{".ident"} << " \"cedo";
  if (versionStr.size())
    stream << ' ' << versionStr;
  stream << '"';
}

void AsmEmitter::emitAsm(const std::vector<Sym> &symList, std::string_view versionStr) {
  emitFilePrologue();
  for (const Sym &sym : symList)
    emitOneSym(sym);
  emitFileEpilogue(versionStr);
  stream.flush();
}
