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

#include "cedo/Backend/AsmStreamer.h"
#include "cedo/Backend/EmitAsm.h"

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

static void emitOneSym(AsmStreamer &stream, const Sym &sym) {
  auto &[name, type, addr] = sym;
  stream << AsmStreamer::Directive{".type"} << ' ' << name << ",@object";
  stream << AsmStreamer::Directive{".size"} << ' ' << name << ", "
         << type->getObjectSize();
  stream << AsmStreamer::Directive{".global"} << ' ' << name;
  stream << AsmStreamer::Directive{".align"} << ' ' << findAlignment(sym);
  stream << AsmStreamer::Label{name};
  stream << AsmStreamer::RawBytes{reinterpret_cast<const uint8_t *>(addr),
                                  type->getObjectSize()};
  stream << '\n';
}

static void emitFileEpilogue(AsmStreamer &stream, std::string_view versionStr) {
  stream << AsmStreamer::Directive{".ident"} << " \"cedo";
  if (versionStr.size())
    stream << ' ' << versionStr;
  stream << '"';
}

void emitAsm(const std::vector<Sym> &symList, std::ostream &os,
             std::string_view versionStr) {
  AsmStreamer stream{os};
  emitFilePrologue(stream);
  for (const Sym &sym : symList)
    emitOneSym(stream, sym);
  emitFileEpilogue(stream, versionStr);
  stream.flush();
}
