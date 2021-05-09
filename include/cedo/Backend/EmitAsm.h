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
#include <string>
#include <tuple>
#include <vector>

#include "cedo/Binfmt/Type.h"

using SymName = std::string;
using Sym = std::tuple<SymName, Type, const void *>;

void emitAsm(const std::vector<Sym> &symList, std::ostream &os);

#endif // CEDO_BACKEND_EMITASM_H
