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

#include <string>

#include "Version.h"

char githash[41] __attribute__((weak)) = "";
char version[20] __attribute__((weak)) = "";

std::string createVersionString() {
  using namespace std::string_literals;

  if (!version[0])
    return {};
  return "("s + version + ") " + githash;
}
