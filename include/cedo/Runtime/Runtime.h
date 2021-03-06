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

#ifndef CEDO_RUNTIME_RUNTIME_H
#define CEDO_RUNTIME_RUNTIME_H

#include <functional>
#include <string_view>
#include <vector>

#include "cedo/Core/ErrorOr.h"

class Runtime {
  void *userSOHandle;

public:
  static ErrorOr<Runtime> loadUserCode(std::string_view filename);

  ErrorOr<int>
  run(const std::function<std::string(const Runtime &)> &concurFunc,
      std::vector<char *> argv = {});

  void *findSymbol(std::string_view name) const;
};

#endif // CEDO_RUNTIME_RUNTIME_H
