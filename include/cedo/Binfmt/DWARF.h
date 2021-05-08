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

#ifndef CEDO_BINFMT_DWARF_H
#define CEDO_BINFMT_DWARF_H

#include <cstdint>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "cedo/Binfmt/Binfmt.h"
#include "cedo/Binfmt/DWARFConstants.h"
#include "cedo/Binfmt/Type.h"

class DWARFReader;

class DWARF {
  friend class DWARFReader;

  using Data = std::variant<uint64_t, std::string>;

  struct DIE {
    using Info = std::vector<std::pair<DW_AT, Data>>;

    DW_TAG tag;
    uint64_t offset;
    Info info;

    std::optional<Data> getAttributeIfPresent(DW_AT attr) const;
  };

  uint16_t version;
  AddressSize addrSize;
  const uint8_t *debugInfoStart;
  std::vector<DIE> debugInfo;

  std::optional<DIE> getTypeDieFromVarDIE(const DIE &varDie) const;
  Type getTypeFromTypeDIE(const DIE &die) const;

public:
  static ErrorOr<DWARF>
  readFromObject(const ObjectFileReader &objectFileReader);

  std::optional<Type> getVariableType(std::string_view sym_name) const;
};

#endif // CEDO_BINFMT_DWARF_H
