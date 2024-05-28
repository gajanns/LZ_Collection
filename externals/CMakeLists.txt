# Memory-Tracking Library written by Timo Bingmann and Lorenz Hübschle
include(FetchContent)
FetchContent_Declare(
  malloc_count
  GIT_REPOSITORY https://github.com/bingmann/malloc_count.git
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/malloc_count
)
FetchContent_MakeAvailable(malloc_count)
add_library(malloc_count_lib STATIC malloc_count/malloc_count.c malloc_count/malloc_count.h)


# Suffixarray Library written by Ilya Grebnov : Apache-2.0 License
FetchContent_Declare(
  libsais
  GIT_REPOSITORY https://github.com/IlyaGrebnov/libsais.git
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libsais
)
FetchContent_MakeAvailable(libsais)

# Unordered Dense Hash Map Library written by Martin Ankerl : MIT License
FetchContent_Declare(
  unordered_dense
  GIT_REPOSITORY https://github.com/martinus/unordered_dense.git
  GIT_TAG v4.4.0
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/unordered_dense
)
FetchContent_MakeAvailable(unordered_dense)

# Rabin-Karp Library written by Peter Dinklage : MIT License
FetchContent_Declare(
  rabinkarp
  GIT_REPOSITORY https://github.com/pdinklag/fp.git
  GIT_TAG main
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/rabinkarp
)
FetchContent_MakeAvailable(rabinkarp)

# Sharded Map Library written by Skadic : MIT License
FetchContent_Declare(
  shardedmap
  GIT_REPOSITORY https://github.com/Skadic/sharded_map.git
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shardedmap
)
FetchContent_MakeAvailable(shardedmap)




