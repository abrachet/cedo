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

#include "cedo/Core/ErrorOr.h"
#include "cedo/Core/FileReader.h"
#include "lib/Binfmt/ELF.h"
#include "gtest/gtest.h"

class ELFReaderTest : public ::testing::Test {
  std::unique_ptr<ELF::Reader> reader;
  const uint8_t *fileStart;

public:
  void SetUp(std::string_view filename) {
    ErrorOr<FileReader> fOrErr = FileReader::open(filename);
    ASSERT_TRUE(fOrErr) << fOrErr.getError();
    fileStart = reinterpret_cast<const uint8_t *>(fOrErr->getFileBuffer());
    reader = ELF::Reader::create(std::move(*fOrErr));
    ASSERT_TRUE(reader);
  }

  const ELF::Reader &getReader() const { return *reader; }

  const uint8_t *getFileStart() const { return fileStart; }
};