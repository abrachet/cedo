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

#ifndef CEDO_BINFMT_BINFMT_H
#define CEDO_BINFMT_BINFMT_H

#include <memory>
#include <optional>

#include "cedo/Core/FileReader.h"

enum class FileFormat {
  ELF,
};

enum class AddressSize {
  Eight,
  Four,
};

enum class Endianness {
  Little,
  Big,
};

struct Triple {
  FileFormat fileFormat : 4;
  AddressSize addrSize : 2;
  Endianness endianness : 2;
};

class ObjectFileReader {
  FileReader file;

public:
  ObjectFileReader(FileReader &&file) : file(std::move(file)) {}

  virtual ~ObjectFileReader() {}

  const FileReader &getFileReader() const { return file; }

  virtual Triple getTriple() const = 0;
};

std::optional<Triple> findFileTriple(const FileReader &file);

std::unique_ptr<ObjectFileReader> createObjectFileReader(FileReader &&f);

#endif // CEDO_BINFMT_BINFMT_H
