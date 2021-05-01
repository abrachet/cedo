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

#include "cedo/Binfmt/Binfmt.h"
#include "cedo/Core/ErrorOr.h"
#include "cedo/Core/FileReader.h"
#include "gtest/gtest.h"

TEST(FindFileTriple, LE64) {
  ErrorOr<FileReader> file = FileReader::open("Inputs/BasicELFLE64.o");
  ASSERT_TRUE(file);
  std::optional<Triple> triple = findFileTriple(*file);
  ASSERT_TRUE(triple);
  ASSERT_EQ(triple->fileFormat, FileFormat::ELF);
  ASSERT_EQ(triple->addrSize, AddressSize::Eight);
  ASSERT_EQ(triple->endianness, Endianness::Little);
}

TEST(FindFileTriple, LE32) {
  ErrorOr<FileReader> file = FileReader::open("Inputs/BasicELFLE32.o");
  ASSERT_TRUE(file);
  std::optional<Triple> triple = findFileTriple(*file);
  ASSERT_TRUE(triple);
  ASSERT_EQ(triple->fileFormat, FileFormat::ELF);
  ASSERT_EQ(triple->addrSize, AddressSize::Four);
  ASSERT_EQ(triple->endianness, Endianness::Little);
}

TEST(FindFileTriple, BE64) {
  ErrorOr<FileReader> file = FileReader::open("Inputs/BasicELFBE64.o");
  ASSERT_TRUE(file);
  std::optional<Triple> triple = findFileTriple(*file);
  ASSERT_TRUE(triple);
  ASSERT_EQ(triple->fileFormat, FileFormat::ELF);
  ASSERT_EQ(triple->addrSize, AddressSize::Eight);
  ASSERT_EQ(triple->endianness, Endianness::Big);
}
