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

#include <array>
#include <optional>
#include <type_traits>

#include "cedo/Binfmt/Binfmt.h"

#include "ELF.h"

struct BinFmt {
  off_t offset;
  std::string_view magic;

  // std::function bad with constexpr...
  std::optional<Triple> (*acceptor)(const FileReader &);
  std::unique_ptr<ObjectFileReader> (*getReader)(FileReader &&);
};

constexpr auto formats = std::array{
    BinFmt{ELF::offset, ELF::magic, ELF::acceptor, ELF::createReader}};

template <typename Callable>
static std::result_of_t<Callable(const BinFmt &)>
forEachFmt(const char *fileBuffer, Callable c) {
  for (const BinFmt &fmt : formats) {
    const char *fileOffset = fileBuffer + fmt.offset;
    std::string_view magic{fileOffset, fmt.magic.size()};
    if (magic != fmt.magic)
      continue;

    if (auto ret = c(fmt); ret)
      return ret;
  }

  return {};
}

std::optional<Triple> findFileTriple(const FileReader &file) {
  return forEachFmt(file.getFileBuffer(),
                    [&file](const BinFmt &fmt) { return fmt.acceptor(file); });
}

std::unique_ptr<ObjectFileReader> createObjectFileReader(FileReader &&f) {
  return forEachFmt(f.getFileBuffer(), [&f](const BinFmt &fmt) {
    return fmt.getReader(std::move(f));
  });
}
