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
#include "cedo/Binfmt/DWARF.h"
#include "cedo/Core/FileReader.h"
#include "gtest/gtest.h"

TEST(DWARFBasic, ReadBasicType) {
  ErrorOr<FileReader> fileReaderOrErr = FileReader::open("Inputs/BasicTypes.o");
  ASSERT_TRUE(fileReaderOrErr);

  std::unique_ptr<ObjectFileReader> objFileReader =
      createObjectFileReader(std::move(*fileReaderOrErr));
  ASSERT_NE(objFileReader, nullptr);

  ErrorOr<DWARF> dwarfOrErr = DWARF::readFromObject(*objFileReader);
  ASSERT_TRUE(dwarfOrErr) << dwarfOrErr.getError();

  auto expectVarSize = [&dwarfOrErr](std::string_view sym_name, size_t size) {
    std::optional<Type> type = dwarfOrErr->getVariableType(sym_name);
    if (!type) {
      EXPECT_TRUE(false) << "Couldn't find symbol: " << sym_name;
      return;
    }
    EXPECT_EQ(type->size, size);
  };

  expectVarSize("one", 1);
  expectVarSize("two", 2);
  expectVarSize("four", 4);
  expectVarSize("eight", 8);

  EXPECT_FALSE(dwarfOrErr->getVariableType("doesnt_exist"));
}
