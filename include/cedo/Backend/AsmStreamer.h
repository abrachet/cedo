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

#ifndef CEDO_BACKEND_BACKEND_H
#define CEDO_BACKEND_BACKEND_H

#include <fstream>

#include "cedo/Binfmt/Binfmt.h"

class AsmStreamer {
  std::ostream &underlyingStream;

  static constexpr char spaces[17] = "                ";
  static constexpr int tabSize = 4;
  static constexpr std::string_view tab{spaces, tabSize};

public:
  AsmStreamer(std::ostream &underlyingStream)
      : underlyingStream(underlyingStream) {}
  ~AsmStreamer() { underlyingStream.flush(); }

  struct Tab {};
  struct Directive : public std::string_view {};

  struct RawBytes {
    const uint8_t *start;
    size_t size;
  };

  struct Byte {
    uint8_t byte;

    static constexpr Directive directive{".byte"};
  };

  template <typename T> AsmStreamer &operator<<(T t) {
    underlyingStream << t;
    return *this;
  }

  template <> AsmStreamer &operator<<<Tab>(Tab) {
    underlyingStream << tab;
    return *this;
  }

  AsmStreamer &operator<<(Directive d) {
    *this << Tab{} << d.data();
    return *this;
  }

  AsmStreamer &operator<<(const Byte &byte) {
    *this << Byte::directive << ' ' << (int)byte.byte << '\n';
    return *this;
  }

  AsmStreamer &operator<<(const RawBytes &rawBytes) {
    for (const uint8_t *curr = rawBytes.start,
                       *end = rawBytes.start + rawBytes.size;
         curr < end; curr++)
      *this << Byte{*curr};
    return *this;
  }
};

#endif // CEDO_BACKEND_BACKEND_H
