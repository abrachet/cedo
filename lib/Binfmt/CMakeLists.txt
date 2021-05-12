add_library(Binfmt
    Binfmt.cpp
    DWARF.cpp
    DWARFType.cpp
    ELF.cpp
)

target_link_libraries(Binfmt Core)
