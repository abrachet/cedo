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


static void emitFilePrologue(AsmStreamer &stream) {
  stream << AsmStreamer::Directive{".data"};
}

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

static void emitForSize(AsmStreamer &stream, size_t size, const uint8_t *addr) {
  if (size == 8)
    return (void) (stream << *reinterpret_cast<const uint64_t *>(addr));
  if (size == 4)
    return (void) (stream << *reinterpret_cast<const uint32_t *>(addr));
  if (size == 2)
    return (void) (stream << *reinterpret_cast<const uint16_t *>(addr));
  assert(size == 1);
  return (void) (stream << *addr);
}

static void emitValueForIntegralType(Triple triple, AsmStreamer &stream,
                             const Type& type, const uint8_t *addr) {
  for (size_t remainingBytes = type.getObjectSize(); remainingBytes; ) {
    auto pair = findLargestType(triple, remainingBytes);
    auto &[size, directive] = pair;
    stream << directive << ' ';
    emitForSize(stream, size, addr);
    stream << '\n';
    addr += size;
    remainingBytes -= size;
  }
}

static void emitOneSym(Triple triple, AsmStreamer &stream, const Sym &sym) {
  auto &[name, type, addr] = sym;
  stream << AsmStreamer::Directive{".type"} << ' ' << name << ",@object";
  stream << AsmStreamer::Directive{".size"} << ' ' << name << ", "
         << type->getObjectSize();
  stream << AsmStreamer::Directive{".global"} << ' ' << name;
  stream << AsmStreamer::Directive{".align"} << ' ' << findAlignment(sym);
  stream << AsmStreamer::Label{name};
  emitValueForIntegralType(triple, stream, *type, reinterpret_cast<const uint8_t *>(addr));
  stream << '\n';
}

static void emitFileEpilogue(AsmStreamer &stream, std::string_view versionStr) {
  stream << AsmStreamer::Directive{".ident"} << " \"cedo";
  if (versionStr.size())
    stream << ' ' << versionStr;
  stream << '"';
}

void emitAsm(Triple outputTriple, const std::vector<Sym> &symList, std::ostream &os,
             std::string_view versionStr) {
  AsmStreamer stream{os};
  emitFilePrologue(stream);
  for (const Sym &sym : symList)
    emitOneSym(outputTriple, stream, sym);
  emitFileEpilogue(stream, versionStr);
  stream.flush();
}
