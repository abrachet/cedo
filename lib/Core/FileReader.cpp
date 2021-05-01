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

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <optional>
#include <string>
#include <string_view>

#include "cedo/Core/ErrorOr.h"
#include "cedo/Core/FileReader.h"

ErrorOr<FileReader> FileReader::open(std::string_view filename) {
  using namespace std::string_literals;
  int fd = ::open(filename.data(), O_RDONLY);
  if (fd == -1)
    return "Couldn't open file \""s + filename.data() + "\"";

  struct stat s;
  if (::fstat(fd, &s) == -1)
    return "Couldn't stat file \""s + filename.data() + "\"";

  void *mapping =
      ::mmap(nullptr, s.st_size, PROT_READ, MAP_PRIVATE | MAP_POPULATE, fd, 0);
  (void)::close(fd);
  if (mapping == MAP_FAILED)
    return "mmap failed"s;
  return FileReader{mapping, static_cast<size_t>(s.st_size)};
}

FileReader::~FileReader() {
  if (file_mapping == MAP_FAILED)
    return;

  int ret = ::munmap(file_mapping, size);
  (void)ret;
  assert(!ret && "munmap failed");
}
