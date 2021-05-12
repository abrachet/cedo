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
#include <cstring>
#include <stack>
#include <string>

#include "cedo/Binfmt/Binfmt.h"
#include "cedo/Binfmt/DWARF.h"
#include "cedo/Binfmt/DWARFConstants.h"
#include "cedo/Core/EndianByteReader.h"
#include "cedo/Core/ErrorOr.h"

#include "ELF.h"

using namespace std::string_literals;

class DWARFReader {
  struct Abbrev {
    DW_TAG tag;
    bool children;
    std::vector<std::pair<DW_AT, DW_FORM>> attributes;
  };

  DWARF &dwarf;
  const ELF::Reader &elfReader;
  Triple objTriple;

  const char *const objectFileStart;
  const uint8_t *const abbrevSecStart;
  const uint8_t *const debugInfoStart;
  AddressSize currentSecAddrSize;
  std::vector<Abbrev> abbrevTable;

  std::stack<uint64_t> parentDIEs;

  DWARFReader(DWARF &dwarf, const ELF::Reader &elfReader,
              const char *objectFileStart, const uint8_t *abbrevSecStart,
              const uint8_t *debugInfoStart)
      : dwarf(dwarf), elfReader(elfReader), objTriple(elfReader.getTriple()),
        objectFileStart(objectFileStart), abbrevSecStart(abbrevSecStart),
        debugInfoStart(debugInfoStart) {}

  std::string readAbbrevTable();
  std::string readDebugInfo();
  std::string readOneDIE(const uint8_t *&debugInfo, const uint8_t *end);

  size_t getDTypeSize(DWARFType type, const uint8_t *ptr) {
    if (size_t size = static_cast<uint64_t>(type); size <= 8)
      return size;

    switch (type) {
    case DWARFType::DWARFAddr:
    case DWARFType::StringPtr:
      return currentSecAddrSize == AddressSize::Eight ? 8 : 4;

    case DWARFType::MachineAddr:
      return objTriple.addrSize == AddressSize::Eight ? 8 : 4;

    case DWARFType::String:
    case DWARFType::Exprloc:
    case DWARFType::ULEB128:
    case DWARFType::LEB128:
      assert(0 && "type has dynamic size and should have not been called here");
      __builtin_trap();

    default:
      assert(0 && "type was not implemented yet");
      __builtin_trap();
    }
  }

  uint64_t readULEB128(const uint8_t *&ptr) {
    uint64_t result = 0;
    for (uint64_t shift = 0;; shift += 7) {
      uint8_t byte = *ptr++ & 0b01111111;
      result |= (byte << shift);
      if (!(byte & 0b10000000))
        break;
    }
    return result;
  }

  DWARF::Data readFromPointer(DWARFType type, const uint8_t *&ptr) {
    if (type == DWARFType::String) {
      std::string str{reinterpret_cast<const char *>(ptr)};
      ptr += str.size() + 1;
      return str;
    }

    // TODO reading exprloc's, for now just advance the pointer the right amount
    // and return 0.
    if (type == DWARFType::Exprloc) {
      size_t numToAdvance = readULEB128(ptr);
      ptr += numToAdvance;
      return uint64_t{0};
    }

    if (type == DWARFType::ULEB128)
      return readULEB128(ptr);

    if (type == DWARFType::LEB128)
      assert(0 && "LEB128 not yet implemented");

    uint64_t data;
    size_t size = getDTypeSize(type, ptr);
    switch (size) {
    // Hacky way to handle DW_FORM_flag_present
    case 0:
      data = 1;
      break;
    case 1:
      data = *ptr;
      break;
    case 2:
      data = *reinterpret_cast<const uint16_t *>(ptr);
      break;
    case 4:
      data = *reinterpret_cast<const uint32_t *>(ptr);
      break;
    case 8:
      data = *reinterpret_cast<const uint64_t *>(ptr);
      break;
    default:
      assert(0 && "unkown size for type");
      __builtin_trap();
    }
    ptr += size;

    if (type == DWARFType::StringPtr) {
      if (!data) {
        ErrorOr<const uint8_t *> resolvedRelocOrErr =
            elfReader.attemptResolveLocalReloc(".debug_info",
                                               ptr - size - debugInfoStart);
        if (!resolvedRelocOrErr)
          return std::string{};
        data = reinterpret_cast<uint64_t>(*resolvedRelocOrErr);
      } else {
        const uint8_t *debugStrSec = elfReader.getSection(".debug_str");
        assert(debugStrSec && "Couldn't find .debug_str");
        data = reinterpret_cast<uintptr_t>(debugStrSec) + data;
      }
      return std::string{reinterpret_cast<const char *>(data)};
    }

    return data;
  };

  static ErrorOr<DWARF> read(const uint8_t *abbrevSec, const uint8_t *debugInfo,
                             const char *objectFileStart,
                             const ELF::Reader &elfReader) {
    DWARF dwarf;
    dwarf.debugInfoStart = debugInfo;
    DWARFReader reader{dwarf, elfReader, objectFileStart, abbrevSec, debugInfo};

    if (std::string err = reader.readAbbrevTable(); err != std::string{})
      return err;
    if (std::string err = reader.readDebugInfo(); err != std::string{})
      return err;
    return dwarf;
  }

public:
  static ErrorOr<DWARF> readFromELFObject(const ELF::Reader &elfReader) {
    const uint8_t *abbrevSec = elfReader.getSection(".debug_abbrev");
    const uint8_t *debugInfo = elfReader.getSection(".debug_info");
    const char *objectFileStart = elfReader.getFileReader().getFileBuffer();
    return read(abbrevSec, debugInfo, objectFileStart, elfReader);
  }
};

std::string DWARFReader::readAbbrevTable() {
  const uint8_t *abbrevPtr = abbrevSecStart;
  // Put fake abbrev 0
  abbrevTable.emplace_back();

  const auto readAbbrev = [&abbrevPtr](Abbrev &currentAbbrev) {
    currentAbbrev.tag = DW_TAG{*abbrevPtr++};
    currentAbbrev.children = *abbrevPtr++;

    for (;;) {
      DW_AT attribute{*abbrevPtr++};
      uint8_t form = *abbrevPtr++;

      if (!attribute && !form)
        return;

      currentAbbrev.attributes.emplace_back(attribute, get_DW_FORM(form));
    }
  };

  for (int expectedCode = 1;; expectedCode++) {
    uint8_t abbrevCode = *abbrevPtr++;

    if (!abbrevCode)
      return {};

    if (abbrevCode != expectedCode)
      return "Error parsing DWARF, expected Abbreviation code '"s +
             std::to_string(expectedCode) + "' but found '" +
             std::to_string(abbrevCode) + '\'';

    readAbbrev(abbrevTable.emplace_back());
  }

  __builtin_unreachable();
}

std::string DWARFReader::readDebugInfo() {
  const uint8_t *debugInfo = debugInfoStart;

  uint64_t size =
      std::get<uint64_t>(readFromPointer(DWARFType::Four, debugInfo));
  if (size == 0xffffffff) {
    size = std::get<uint64_t>(readFromPointer(DWARFType::Eight, debugInfo));
    currentSecAddrSize = AddressSize::Eight;
  } else {
    if (size >= 0xfffffff0)
      return "Malformed DWARF: initial length field has first four bytes of value: "s +
             std::to_string(size);
    currentSecAddrSize = AddressSize::Four;
  }
  if (size < 7)
    return "Debug info section is too small for needed data";
  const uint8_t *end = debugInfo - 1 + size;
  uint64_t versionNum =
      std::get<uint64_t>(readFromPointer(DWARFType::Two, debugInfo));
  uint64_t abbrevOffset =
      std::get<uint64_t>(readFromPointer(DWARFType::Four, debugInfo));
  uint64_t addrSize =
      std::get<uint64_t>(readFromPointer(DWARFType::One, debugInfo));

  if (versionNum > 4)
    return "Unknown DWARF version: '"s + std::to_string(versionNum) + '\'';
  if (addrSize != (objTriple.addrSize == AddressSize::Eight ? 8 : 4))
    return "Malformed DWARF: should have same address size as it's ELF file";

  dwarf.version = static_cast<uint16_t>(versionNum);
  dwarf.addrSize = objTriple.addrSize;
  dwarf.debugInfo.reserve(abbrevTable.size());

  while (debugInfo < end)
    if (std::string err =
            readOneDIE(debugInfo, reinterpret_cast<const uint8_t *>(end));
        err != std::string{})
      return err;

  assert(!parentDIEs.size() &&
         "Didn't find all end of child marks for DIEs with children");
  return {};
}

std::string DWARFReader::readOneDIE(const uint8_t *&debugInfo,
                                    const uint8_t *end) {
  if (debugInfo == end)
    return "Malformed DWARF: expected another DIE but debug_info section has "
           "ended";

  uint64_t offset = debugInfo - debugInfoStart;

  uint64_t abbrevCode =
      std::get<uint64_t>(readFromPointer(DWARFType::One, debugInfo));

  if (abbrevCode >= abbrevTable.size())
    return "Malformed DWARF: Abbrev. Code '"s + std::to_string(abbrevCode) +
           "' is larger than largest known abbrev code '" +
           std::to_string(abbrevTable.size()) + '\'';

  const Abbrev &currentDieType = abbrevTable[abbrevCode];
  auto &die = dwarf.debugInfo.emplace_back();

  if (parentDIEs.size()) {
    uint64_t parentOffset = parentDIEs.top();
    DWARF::DIE *parentDie = dwarf.getDIEFromOffset(parentOffset);
    assert(parentDie && "parent wasn't found, messed up somewhere");
    parentDie->childrenOffsets.emplace_back(offset);
  }

  if (currentDieType.children)
    parentDIEs.push(offset);

  die.tag = currentDieType.tag;
  die.offset = offset;

  // TODO check if we would have read past end
  for (const auto &[attr, form] : currentDieType.attributes)
    die.info.emplace_back(attr, readFromPointer(form.type, debugInfo));

  // End of child mark
  if (parentDIEs.size() && !*debugInfo) {
    debugInfo++;
    parentDIEs.pop();
  }

  return {};
}

ErrorOr<DWARF> DWARF::readFromObject(const ObjectFileReader &objectFileReader) {
  if (const ELF::Reader *elfReader =
          dynamic_cast<const ELF::Reader *>(&objectFileReader))
    return DWARFReader::readFromELFObject(*elfReader);
  return "Cannot get debug info from unkown objectFileReaderType"s;
}
