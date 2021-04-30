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

#include <fstream>

#include "cedo/Core/FileReader.h"
#include "gtest/gtest.h"

TEST(FileReader, FileDoesntExist) {
  ErrorOr<FileReader> file = FileReader::create("");
  ASSERT_FALSE(file);
  ASSERT_EQ(file.getError(), "Couldn't open file \"\"");
}

TEST(FileReader, ReadBasicFile) {
  std::ofstream f("test.tmp");
  f.write("text", 4);
  f.close();

  ErrorOr<FileReader> file = FileReader::create("test.tmp");
  ASSERT_TRUE(file);
  ASSERT_EQ(file->getFileSize(), 4);
  ASSERT_STREQ(file->getFileBuffer(), "text");
}
