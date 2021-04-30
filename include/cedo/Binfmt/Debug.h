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

#include <cstdint>
#include <vector>
#include <string_view>
#include <optional>

#include "cedo/Core/ErrorOr.h"
#include "cedo/Core/FileReader.h"

struct Type {};

class DebugSymbols {
public:
  virtual ~DebugSymbols() = 0;

  virtual std::optional<Type> getSymType(std::string_view sym_name) const = 0;
};

class BinFile {
  FileReader file;
  std::unique_ptr<DebugSymbols> debugSyms;

public:
  std::optional<Type> getSymType(std::string_view sym_name) {
    return debugSyms->getSymType(sym_name);
  }

  virtual size_t getAddressSizeBytes() const {
    return sizeof(size_t);
  }
};

ErrorOr<BinFile> readBinFile(std::string_view path);
