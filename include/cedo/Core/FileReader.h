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

#ifndef CEDO_CORE_FILEREADER_H
#define CEDO_CORE_FILEREADER_H

#include <sys/mman.h>

#include <cstdint>
#include <string_view>

#include "ErrorOr.h"

struct FileReader {
  void *file_mapping = MAP_FAILED;
  size_t size;

  FileReader(void *file_mapping, size_t size) :
    file_mapping(file_mapping), size(size) {}
public:
  static ErrorOr<FileReader> create(std::string_view path);
  // Needed for ErrorOr<FileReader> even though it's ugly...
  FileReader(FileReader &&f) : file_mapping(f.file_mapping), size(f.size) {
    f.file_mapping = MAP_FAILED;
  }
  ~FileReader();

  const char *getFileBuffer() const {
    return reinterpret_cast<const char *>(file_mapping);
  }

  operator const char *() const {
    return getFileBuffer();
  }

  size_t getFileSize() const {
    return size;
  }
};

#endif // CEDO_CORE_FILEREADER_H
