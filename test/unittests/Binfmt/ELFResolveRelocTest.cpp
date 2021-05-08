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

#include "ELFReaderTest.h"
#include "gtest/gtest.h"

struct ResolveReloc : public ELFReaderTest {};

TEST_F(ResolveReloc, Basic32) {
  SetUp("Inputs/ResolveReloc.o");
  const ELF::Reader &reader = getReader();

  ErrorOr<const uint8_t *> locOrErr =
      reader.attemptResolveLocalReloc(".test32", 0);
  ASSERT_TRUE(locOrErr);

  const char *str = reinterpret_cast<const char *>(*locOrErr);
  EXPECT_STREQ(str, "String 0");
}

TEST_F(ResolveReloc, Basic64) {
  SetUp("Inputs/ResolveReloc.o");
  const ELF::Reader &reader = getReader();

  ErrorOr<const uint8_t *> locOrErr =
      reader.attemptResolveLocalReloc(".test64", 0);
  ASSERT_TRUE(locOrErr);

  const char *str = reinterpret_cast<const char *>(*locOrErr);
  EXPECT_STREQ(str, "String 0");
}

TEST_F(ResolveReloc, SecOffset) {
  SetUp("Inputs/ResolveReloc.o");
  const ELF::Reader &reader = getReader();

  ErrorOr<const uint8_t *> locOrErr =
      reader.attemptResolveLocalReloc(".test32", 4);
  ASSERT_TRUE(locOrErr) << locOrErr.getError();

  const char *str = reinterpret_cast<const char *>(*locOrErr);
  EXPECT_STREQ(str, "String 0");
}

TEST_F(ResolveReloc, SecOffsetSymOffset) {
  SetUp("Inputs/ResolveReloc.o");
  const ELF::Reader &reader = getReader();

  ErrorOr<const uint8_t *> locOrErr =
      reader.attemptResolveLocalReloc(".test32", 8);
  ASSERT_TRUE(locOrErr) << locOrErr.getError();

  const char *str = reinterpret_cast<const char *>(*locOrErr);
  EXPECT_STREQ(str, "String 4");
}
