add_library(Binfmt
    Binfmt.cpp
    DWARF.cpp
    ELF.cpp
)

target_link_libraries(Binfmt Core)
