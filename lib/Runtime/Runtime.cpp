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

#include <dlfcn.h>

#include <functional>
#include <string_view>
#include <vector>

#include "cedo/Core/ErrorOr.h"
#include "cedo/Runtime/Runtime.h"

using namespace std::string_literals;

ErrorOr<Runtime> Runtime::loadUserCode(std::string_view filename) {
  void *handle = ::dlopen(filename.data(), RTLD_NOW | RTLD_LOCAL);
  if (!handle)
    return "Couldn't dlopen(\""s + filename.data() +
           "\"). Reason: " + dlerror();
  Runtime ret;
  ret.userSOHandle = handle;
  return ret;
}

void *Runtime::findSymbol(std::string_view name) const {
  return ::dlsym(userSOHandle, name.data());
}

ErrorOr<int>
Runtime::run(const std::function<std::string(const Runtime &)> &concurFunc,
             std::vector<char *> argv) {
  using MainT = int(int, char **);
  MainT *main = reinterpret_cast<MainT *>(findSymbol("main"));
  if (!main)
    return "Couldn't find symbol \"main\". Reason: "s + dlerror();

  // TODO later this will be concurrent but for now don't bother.
  if (std::string err = concurFunc(*this); err != std::string{})
    return err;

  return main(argv.size(), argv.data());
}
