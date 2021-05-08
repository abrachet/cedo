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

#ifndef CEDO_LIB_BINFMT_ELF_H
#define CEDO_LIB_BINFMT_ELF_H

#include <memory>
#include <optional>
#include <string_view>

#include "cedo/Binfmt/Binfmt.h"
#include "cedo/Binfmt/Debug.h"
#include "cedo/Core/FileReader.h"

namespace ELF {

class Reader : public ::Reader {
protected:
  FileReader file;

  Reader(FileReader &&file) : file(std::move(file)) {}

public:
  static std::unique_ptr<Reader> create(FileReader &&file);
  static std::unique_ptr<Reader> create(FileReader &&file, Triple t);

  // This does not change the underlying file or it's buffer, just returns the
  // value.
  virtual ErrorOr<const uint8_t *>
  attemptResolveLocalReloc(std::string_view section_name,
                           uint64_t offset) const = 0;

  virtual const uint8_t *getSection(std::string_view name) const = 0;
};

constexpr std::string_view magic = "\x7f"
                                   "ELF";
constexpr off_t offset = 0;

std::optional<Triple> acceptor(const FileReader &);

static inline std::unique_ptr<::Reader> createReader(FileReader &&f) {
  return Reader::create(std::move(f));
}

} // namespace ELF

#endif // CEDO_LIB_BINFMT_ELF_H
