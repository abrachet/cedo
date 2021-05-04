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

#include <array>

#include "cedo/Binfmt/Binfmt.h"
#include "cedo/Core/EndianByteReader.h"
#include "gtest/gtest.h"

TEST(EndianByteReader, Basic) {
  uint8_t ule32[4]{0x78, 0x56, 0x34, 0x12};  
  uint32_t i;

  readFromPointer<Endianness::Little>(i, ule32);
  ASSERT_EQ(i, 0x12345678);
  readFromPointer<Endianness::Big>(i, ule32);
  ASSERT_EQ(i, 0x78563412);
}
