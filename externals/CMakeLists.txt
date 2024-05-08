
include(FetchContent)
FetchContent_Declare(
  malloc_count
  GIT_REPOSITORY https://github.com/bingmann/malloc_count.git
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/malloc_count
)
FetchContent_MakeAvailable(malloc_count)

# Add the library
add_library(malloc_count_lib STATIC malloc_count/malloc_count.c malloc_count/malloc_count.h)