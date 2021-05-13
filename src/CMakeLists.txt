add_executable(cedo
    cedo.cpp
)

target_link_libraries(cedo
  Backend
  Binfmt
  Core
  Runtime
)

add_subdirectory(version)
