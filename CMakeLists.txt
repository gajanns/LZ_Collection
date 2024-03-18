cmake_minimum_required(VERSION 3.0.0)
project(LZ_Collection VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PERFORMANCE_MEASUREMENT ON)
set(TESTING ON)

include(GoogleTest)
enable_testing()

include_directories(include)
include_directories(include/LZ77)
include_directories(include/LZW)

add_subdirectory(source)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

add_compile_options(-Wall -Wextra -pedantic -Werror)

if(TESTING) 
  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/main.zip
  )
  FetchContent_MakeAvailable(googletest)
  add_subdirectory(tests)
endif()