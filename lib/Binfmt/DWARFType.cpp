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

#include <algorithm>
#include <optional>
#include <string_view>

#include "cedo/Binfmt/DWARF.h"
#include "cedo/Binfmt/DWARFConstants.h"
#include "cedo/Binfmt/Type.h"

std::optional<DWARF::Data> DWARF::DIE::getAttributeIfPresent(DW_AT attr) const {
  auto it = std::find_if(info.begin(), info.end(),
                         [attr](const auto &i) { return i.first == attr; });
  if (it == info.end())
    return {};
  return it->second;
}

std::optional<DWARF::DIE> DWARF::getTypeDieFromDie(const DIE &die) const {
  auto typeOffsetOrErr = die.getAttributeIfPresent(DW_AT_type);
  if (!typeOffsetOrErr)
    return {};

  uint64_t offset = std::get<uint64_t>(*typeOffsetOrErr);

  auto it = std::find_if(debugInfo.begin(), debugInfo.end(),
                         [offset](const DIE &d) { return d.offset == offset; });

  if (it == debugInfo.end())
    return {};
  return *it;
}

Type DWARF::getTypeFromTypeDie(const DIE &typeDie) const {
  if (typeDie.tag == DW_TAG_typedef) {
    std::optional<DWARF::DIE> realType = getTypeDieFromDie(typeDie);
    assert(realType &&
           "typedef DIE's DW_AT_type did not point to a valid type");
    return getTypeFromTypeDie(*realType);
  }
  assert(typeDie.tag == DW_TAG_base_type &&
         "can't deal with struct/array types");
  auto attrOrErr = typeDie.getAttributeIfPresent(DW_AT_byte_size);
  // TODO error handling
  assert(attrOrErr && "All type die's must have a DW_AT_byte_size attribute");
  return {std::get<uint64_t>(*attrOrErr)};
}

std::optional<Type> DWARF::getVariableType(std::string_view sym_name) const {
  auto it = std::find_if(
      debugInfo.begin(), debugInfo.end(), [sym_name](const DIE &die) {
        if (die.tag != DW_TAG_variable)
          return false;

        auto attrOrError = die.getAttributeIfPresent(DW_AT_name);
        if (!attrOrError)
          return false;

        return std::get<std::string>(*attrOrError) == sym_name;
      });
  if (it == debugInfo.end())
    return {};

  std::optional<DIE> typeDie = getTypeDieFromDie(*it);
  if (!typeDie)
    return {};

  return getTypeFromTypeDie(*typeDie);
}
