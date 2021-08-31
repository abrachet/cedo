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

#ifndef CEDO_BACKEND_EMITASM_H
#define CEDO_BACKEND_EMITASM_H

#include <iosfwd>
#include <memory>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

#include "cedo/Backend/AsmStreamer.h"
#include "cedo/Binfmt/Type.h"
#include "cedo/Binfmt/Binfmt.h"

using SymName = std::string;
using Sym = std::tuple<SymName, std::unique_ptr<Type>, const void *>;

class AsmEmitter {
  Triple outputTriple;
  AsmStreamer stream;

  void emitFilePrologue();
  void emitFileEpilogue(std::string_view versionStr);

  void emitOneSym(const Sym &sym);

  void emitObject(const Type &type, const uint8_t *addr);

  template <size_t AddrSize>
  void emitPointerValue(const Type &type, const uint8_t *addr);
  void emitPointerType(const Type& type, const uint8_t *addr);

  void emitValueForIntegralType(const Type& type, const uint8_t *addr);
  void emitForSize(size_t size, const uint8_t *addr);

public:
  AsmEmitter(Triple outputTriple, std::ostream &os)
    : outputTriple(outputTriple), stream(os) {}

  void emitAsm(const std::vector<Sym> &symList, std::string_view versionStr = {});
};

#endif // CEDO_BACKEND_EMITASM_H
