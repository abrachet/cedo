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

#ifndef CEDO_BINFMT_TYPE_H
#define CEDO_BINFMT_TYPE_H

#include <cstdint>
#include <string>

class Type {
  uint8_t qualifiers;

protected:
  Type(uint8_t qualifiers) : qualifiers(qualifiers) {}

public:
  // Signedness and CV qualifiers are not important for serialization,
  // but they will be used in comments in the outputted assembly.
  enum Qualifier : uint8_t {
    Signed = 0,
    Unsigned = 1,

    Const = 2,

    Volatile = 4,

    Pointer = 8,
    Array = 16,
    Compound = 32, // class, struct, union
  };

  virtual ~Type() {}

  virtual size_t getObjectSize() const = 0;

  bool isBuiltin() const { return !isPointer() && !isArray() && !isCompound(); }
  bool isPointer() const { return qualifiers & Pointer; }
  bool isArray() const { return qualifiers & Array; }
  bool isCompound() const { return qualifiers & Compound; }
};

struct BaseType : public Type {
  size_t byteSize;

  BaseType(uint8_t qualifiers, size_t byteSize)
      : Type(qualifiers), byteSize(byteSize) {}

  size_t getObjectSize() const override { return byteSize; }
};

struct ArrayType : public Type {
  std::unique_ptr<Type> elementType;
  size_t numElements;

  ArrayType(uint8_t qualifiers, std::unique_ptr<Type> &&elementType,
            size_t numElements)
      : Type(qualifiers), elementType(std::move(elementType)),
        numElements(numElements) {}

  size_t getObjectSize() const override {
    return elementType->getObjectSize() * numElements;
  }
};

struct StructType : public Type {
  using MemberOffset = size_t;
  using Member = std::pair<std::unique_ptr<Type>, MemberOffset>;

  std::vector<Member> members;
  size_t totalByteSize;

  StructType(uint8_t qualifiers, size_t totalByteSize)
      : Type(qualifiers), totalByteSize(totalByteSize) {}

  size_t getObjectSize() const override { return totalByteSize; }
};

#endif // CEDO_BINFMT_TYPE_H
