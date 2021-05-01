add_library(Binfmt
    Binfmt.cpp
    Debug.cpp
    ELF.cpp
)

target_link_libraries(Binfmt Core)
