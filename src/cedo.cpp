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

#include <unistd.h>

#include <cstdio>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>

#include "cedo/Backend/EmitAsm.h"
#include "cedo/Binfmt/Binfmt.h"
#include "cedo/Binfmt/DWARF.h"
#include "cedo/Core/FileReader.h"
#include "cedo/Runtime/Runtime.h"

#include "version/Version.h"

struct Args {
  std::string_view inputFile;
  std::string outputFile;
  std::vector<std::string_view> outputSyms;
  bool saveTemps = false;
  bool emitVersion = true;
};

Args parseArgs(int argc, const char **argv) {
  using namespace std::string_literals;

  Args args;
  for (const char **current = argv + 1, **end = argv + argc; current != end;
       current++) {
    if ("-S"s == *current) {
      args.saveTemps = 1;
      continue;
    }
    if ("-s"s == *current || "--sym"s == *current) {
      args.outputSyms.emplace_back(*++current);
      continue;
    }
    if ("-o"s == *current || "--output"s == *current) {
      args.outputFile = *++current;
      continue;
    }

    if ("--no-version"s == *current) {
      args.emitVersion = false;
      continue;
    }

    args.inputFile = *current;
  }

  if (!args.saveTemps) {
    std::fputs("-S must currently be specified\n", stderr);
    std::exit(1);
  }

  if (args.inputFile == "") {
    std::fputs("No input file was specified\n", stderr);
    std::exit(1);
  }

  if (!args.outputSyms.size()) {
    std::fputs("No output symbols were specified\n", stderr);
    std::exit(1);
  }

  if (args.outputFile == "") {
    std::string out{args.inputFile.data()};
    auto it = out.rfind(".");
    if (it != out.npos)
      out.resize(it);
    out += ".s";
    args.outputFile = std::move(out);
  }

  return args;
}

void warn(std::string_view warning) {
  std::fprintf(stderr, "Warning: %s\n", warning.data());
}

static ErrorOr<std::vector<Sym>>
runUserCodeAndGetSyms(std::string_view userFilename,
                      std::vector<std::string_view> outputSyms) {
  using namespace std::string_literals;

  std::vector<Sym> resolvedSyms;

  auto concurrent = [&](const Runtime &runtime) -> std::string {
    ErrorOr<FileReader> fileOrErr = FileReader::open(userFilename);
    if (!fileOrErr)
      return fileOrErr.getError();

    std::unique_ptr<ObjectFileReader> objFileReader =
        createObjectFileReader(std::move(*fileOrErr));
    if (!objFileReader)
      return "Couldn't read object file"s;

    ErrorOr<DWARF> debugSymbols = DWARF::readFromObject(*objFileReader);
    if (!debugSymbols)
      return debugSymbols.getError();

    for (std::string_view symName : outputSyms) {
      std::unique_ptr<Type> type = debugSymbols->getVariableType(symName);
      if (!type) {
        warn("Couldn't find debug info for '"s + symName.data() + '\'');
        continue;
      }

      void *symLocation = runtime.findSymbol(symName);
      if (!symLocation) {
        warn("Symbol '"s + symName.data() +
             "' is in debug info but was not found in shared object");
        continue;
      }

      resolvedSyms.emplace_back(symName, std::move(type), symLocation);
    }

    return {};
  };

  ErrorOr<Runtime> runtimeOrErr = Runtime::loadUserCode(userFilename);
  if (!runtimeOrErr)
    return runtimeOrErr.getError();

  ErrorOr<int> exitCodeOrErr = runtimeOrErr->run(concurrent);
  if (!exitCodeOrErr)
    return exitCodeOrErr.getError();

  if (*exitCodeOrErr)
    return "Exit code: '"s + std::to_string(*exitCodeOrErr) + '\'';

  return resolvedSyms;
}

int main(int argc, const char **argv) {
  Args args = parseArgs(argc, argv);

  ErrorOr<std::vector<Sym>> symsOrErr =
      runUserCodeAndGetSyms(args.inputFile, args.outputSyms);
  if (!symsOrErr) {
    std::fputs(symsOrErr.getError().c_str(), stderr);
    return 1;
  }

  std::ofstream stream{args.outputFile};
  emitAsm(*symsOrErr, stream, args.emitVersion ? createVersionString() : "");

  return 0;
}
