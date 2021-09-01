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

class HasChildTypes {
protected:
  struct iterator_impl {
    using reference = std::pair<const Type &, off_t>;

    virtual ~iterator_impl() {}

    virtual void operator++() = 0;
    virtual bool operator!=(const iterator_impl &) = 0;
    virtual reference operator*() const = 0;
  };

public:
  struct iterator {
    std::unique_ptr<iterator_impl> it;

    iterator(std::unique_ptr<iterator_impl> &&it) : it(std::move(it)) {}

    iterator &operator++() {
      it->operator++();
      return *this;
    }
    bool operator!=(const iterator &other) { return *it != *other.it; }
    iterator_impl::reference operator*() const { return **it; }
  };

  virtual iterator begin() const = 0;
  virtual iterator end() const = 0;
};

struct BaseType : public Type {
  size_t byteSize;

  BaseType(uint8_t qualifiers, size_t byteSize)
      : Type(qualifiers), byteSize(byteSize) {}

  size_t getObjectSize() const override { return byteSize; }
};

struct ArrayType : public Type, public HasChildTypes {
private:
  struct iterator_impl : public HasChildTypes::iterator_impl {
    const Type &type;
    const size_t numElements;
    const size_t typeSize;
    off_t currentMemberOffset{0};

    iterator_impl(const Type &type, size_t numElements)
      : type(type), numElements(numElements), typeSize(type.getObjectSize()) {}

    static std::unique_ptr<HasChildTypes::iterator_impl> createEnd(const Type &type, size_t numElements) {
      auto ret = std::make_unique<iterator_impl>(type, numElements);
      ret->currentMemberOffset += numElements * ret->typeSize;
      return ret;
    }

    void operator++() override {
      currentMemberOffset += typeSize;
    }

    bool operator!=(const HasChildTypes::iterator_impl &other) override {
      return currentMemberOffset != reinterpret_cast<const iterator_impl &>(other).currentMemberOffset;
    }

    reference operator*() const override {
      return {type, currentMemberOffset};
    }
  };

public:
  std::unique_ptr<Type> elementType;
  size_t numElements;

  ArrayType(uint8_t qualifiers, std::unique_ptr<Type> &&elementType,
            size_t numElements)
      : Type(qualifiers | Type::Qualifier::Array), elementType(std::move(elementType)),
        numElements(numElements) {}

  size_t getObjectSize() const override {
    return elementType->getObjectSize() * numElements;
  }

  iterator begin() const override {
    return {std::make_unique<iterator_impl>(*elementType, numElements)};
  }

  iterator end() const override {
    return iterator_impl::createEnd(*elementType, numElements);
  }
};

class StructType : public Type, public HasChildTypes {
public:
  using MemberOffset = off_t;
  using Member = std::pair<std::unique_ptr<Type>, MemberOffset>;

private:
  struct iterator_impl : public HasChildTypes::iterator_impl {
    using IterT = std::vector<Member>::const_iterator;

    IterT it;

    iterator_impl(IterT it) : it(it) {}

    void operator++() override {
      it++;
    }

    bool operator!=(const HasChildTypes::iterator_impl &other) override {
      return it != reinterpret_cast<const iterator_impl &>(other).it;
    }

    reference operator*() const override {
      auto &[typePtr, offset] = *it;
      return {*typePtr, offset};
    }
  };

public:
  std::vector<Member> members;
  size_t totalByteSize;

  StructType(uint8_t qualifiers, size_t totalByteSize)
      : Type(qualifiers | Type::Qualifier::Compound), totalByteSize(totalByteSize) {}

  size_t getObjectSize() const override { return totalByteSize; }

  iterator begin() const override {
    return {std::make_unique<iterator_impl>(members.cbegin())};
  }

  iterator end() const override {
    return {std::make_unique<iterator_impl>(members.cend())};
  }
};

struct PointerType : public Type {
  std::unique_ptr<Type> pointingType;

  PointerType(uint8_t qualifiers, std::unique_ptr<Type> &&pointingType)
    : Type(qualifiers | Type::Qualifier::Pointer), pointingType(std::move(pointingType)) {}

  size_t getObjectSize() const override {
    // TODO: Need to do something about this...
    return 8;
  }
};

#endif // CEDO_BINFMT_TYPE_H
