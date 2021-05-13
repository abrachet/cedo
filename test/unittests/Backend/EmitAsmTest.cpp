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

#include <memory>
#include <sstream>
#include <vector>

#include "cedo/Backend/AsmStreamer.h"
#include "cedo/Backend/EmitAsm.h"

#include "gtest/gtest.h"

TEST(EmitAsm, EmitBasicTypes) {
  const char *expectedBasicTypes =
      R"(    .data
    .type sym4,@object
    .size sym4, 4
    .global sym4
    .align 1
sym4:
    .byte 1
    .byte 2
    .byte 3
    .byte 4

    .type sym8,@object
    .size sym8, 8
    .global sym8
    .align 1
sym8:
    .byte 1
    .byte 2
    .byte 3
    .byte 4
    .byte 5
    .byte 6
    .byte 7
    .byte 8

    .ident "cedo 0.1"
)";

  std::stringstream output;

  uint8_t bytes[8] = {1, 2, 3, 4, 5, 6, 7, 8};
  std::vector<Sym> syms;
  syms.emplace_back("sym4", std::make_unique<BaseType>(0, 4), bytes);
  syms.emplace_back("sym8", std::make_unique<BaseType>(0, 8), bytes);
  emitAsm(syms, output);

  EXPECT_STREQ(output.str().c_str(), expectedBasicTypes);
}
