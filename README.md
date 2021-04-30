# Cedo (constexpr dot o)

`cedo` is a tool for creating object files from state at runtime. `cedo` exists to solve the same problem as the `constexpr` or the more recent and more apt keyword `consteval`. `cedo` can be used as part of a projects build to create state at compile time that cannot be computed by `constexpr`. The inital idea was inspired after `std::embed` failed to make it into the C++20 standard.

## Example

We can use linkers to embed files or any arbitrary data into an object file. `cedo` however goes a step beyond it reads the debugging symbols of the program it runs to export any arbitrary data object.

Currently `cedo` is just being started so this example will not actually work.

#### **`githash.c`**
```C
char githash[41];

int main() {
    // Find current git hash and fill in githash
}
```

This can be build like `clang githash.c -shared -g -o export_githash`
Then we can run `cedo export_githash -s githash -S -o githash.s `. This will run the program `export_githash` and on it's exit serialize the symbol `githash`, `-S` says to output an assembly file, just like it does with `cc`. It then create an according assembly file `githash.s`. We can then use this file with our build has we would like. `githash.s` might look like:

#### **`githash.s`**
```
    .file   "export_githash"
    .global githash
    .data
    .align  32
    .type   githash, @object
    .size   githash, 41
githash:
    .asciz  "d670460b4b4aece5915caf5c68d12f560a9fe3e4"

    .ident  "cedo 1.0 (e5b3985af804eb658a78d5546915ba8bfb256884)"
```

Obviously, something like a git hash is trivial enough that we can rely on the linker. However `cedo` with the help of debug symbols can serialize arbitrarily complicated data objects into object files.

## Building

`mkdir build; cd build; cmake ..`

## Testing

`cd build/test/unittest; ctest`
