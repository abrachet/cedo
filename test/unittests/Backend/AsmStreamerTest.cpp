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

#include <sstream>

#include "cedo/Backend/AsmStreamer.h"

#include "gtest/gtest.h"

TEST(AsmStreamer, PrintBytes) {
  std::stringstream output;
  AsmStreamer streamer{output};

  uint8_t bytes[4]{1, 2, 3, 4};
  streamer << AsmStreamer::RawBytes{bytes, 4};

  const char *expected = "    .byte 1\n    .byte 2\n    .byte 3\n    .byte 4\n";

  EXPECT_STREQ(output.str().c_str(), expected);
}
