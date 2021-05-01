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

#include <cassert>
#include <cstdint>
#include <optional>
#include <string_view>

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

} // namespace ELF
