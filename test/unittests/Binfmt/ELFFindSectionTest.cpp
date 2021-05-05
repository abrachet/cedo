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

#include "cedo/Core/FileReader.h"
#include "lib/Binfmt/ELF.h"
#include "gtest/gtest.h"

TEST(FindSection, Basic) {
  ErrorOr<FileReader> fOrErr = FileReader::open("Inputs/Shdr.o");
  ASSERT_TRUE(fOrErr);
  const uint8_t *fileStart =
      reinterpret_cast<const uint8_t *>(fOrErr->getFileBuffer());
  std::unique_ptr<ELF::Reader> reader = ELF::Reader::create(std::move(*fOrErr));
  ASSERT_TRUE(reader);
  EXPECT_EQ(reader->getSection(".cedotest"), fileStart + 0x2000);
}
