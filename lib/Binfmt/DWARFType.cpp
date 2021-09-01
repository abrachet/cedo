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
#include <memory>
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

const DWARF::DIE *DWARF::getTypeDieFromDie(const DIE &die) const {
  auto typeOffsetOrErr = die.getAttributeIfPresent(DW_AT_type);
  if (!typeOffsetOrErr)
    return nullptr;
  return getDIEFromOffset(std::get<uint64_t>(*typeOffsetOrErr));
}

std::unique_ptr<Type> DWARF::getTypeFromBaseTypeDie(const DIE &die) const {
  assert(die.tag == DW_TAG_base_type);

  auto attrOrErr = die.getAttributeIfPresent(DW_AT_byte_size);
  if (!attrOrErr)
    return nullptr;

  return std::make_unique<BaseType>(0, std::get<uint64_t>(*attrOrErr));
}

std::unique_ptr<Type> DWARF::getTypeFromArrayDie(const DIE &die) const {
  assert(die.tag == DW_TAG_array_type);

  const DIE *typeDie = getTypeDieFromDie(die);
  if (!typeDie)
    return nullptr;
  std::unique_ptr<Type> elementType = getTypeFromTypeDie(*typeDie);
  if (!elementType)
    return nullptr;

  uint64_t firstChildOffset = die.childrenOffsets[0];
  const DIE *subrangeDie = getDIEFromOffset(firstChildOffset);
  if (!subrangeDie)
    return 0;

  // TOOD read the standard more here...
  assert(subrangeDie->tag == DW_TAG_subrange_type &&
         "array_type was arranged in an unkown way");

  auto numElements = subrangeDie->getAttributeIfPresent(DW_AT_count);
  if (!numElements)
    return nullptr;

  return std::make_unique<ArrayType>(0, std::move(elementType),
                                     std::get<uint64_t>(*numElements));
}

std::unique_ptr<Type> DWARF::getTypeFromStructTypeDie(const DIE &die) const {
  auto byteSize = die.getAttributeIfPresent(DW_AT_byte_size);
  if (!byteSize)
    return nullptr;

  std::unique_ptr<StructType> structType =
      std::make_unique<StructType>(0, std::get<uint64_t>(*byteSize));
  std::vector<StructType::Member> &members = structType->members;

  for (size_t childOffset : die.childrenOffsets) {
    const DIE *child = getDIEFromOffset(childOffset);
    // TOOD maybe children could be something other than member. Look into the
    // standard...
    if (!child || child->tag != DW_TAG_member)
      return nullptr;

    auto location = child->getAttributeIfPresent(DW_AT_data_member_location);
    if (!location)
      return nullptr;

    const DIE *childTypeDie = getTypeDieFromDie(*child);
    if (!childTypeDie)
      return nullptr;

    members.emplace_back(getTypeFromTypeDie(*childTypeDie),
                         std::get<uint64_t>(*location));
  }

  std::sort(members.begin(), members.end(),
            [](const StructType::Member &a, const StructType::Member &b) {
              return b.second > a.second;
            });

  return structType;
}

std::unique_ptr<Type> DWARF::getTypeFromPointerTypeDie(const DIE &die) const {
  const DWARF::DIE *pointingTypeDie = getTypeDieFromDie(die);
  std::unique_ptr<Type> pointingType = getTypeFromTypeDie(*pointingTypeDie);
  // TODO: find other qualifiers
  return std::make_unique<PointerType>(Type::Qualifier::Pointer, std::move(pointingType));
}

std::unique_ptr<Type> DWARF::getTypeFromTypeDie(const DIE &typeDie) const {
  if (typeDie.tag == DW_TAG_typedef) {
    const DWARF::DIE *realType = getTypeDieFromDie(typeDie);
    assert(realType &&
           "typedef DIE's DW_AT_type did not point to a valid type");
    return getTypeFromTypeDie(*realType);
  }
  switch (typeDie.tag) {
  case DW_TAG_base_type:
    return getTypeFromBaseTypeDie(typeDie);
  case DW_TAG_structure_type:
  case DW_TAG_class_type:
  case DW_TAG_union_type:
    return getTypeFromStructTypeDie(typeDie);
  case DW_TAG_array_type:
    return getTypeFromArrayDie(typeDie);
  case DW_TAG_pointer_type:
    return getTypeFromPointerTypeDie(typeDie);
  default:
    assert(0 && "only base_type is currently supported");
  }
  return nullptr;
}

std::unique_ptr<Type> DWARF::getVariableType(std::string_view sym_name) const {
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

  const DIE *typeDie = getTypeDieFromDie(*it);
  if (!typeDie)
    return {};

  return getTypeFromTypeDie(*typeDie);
}
