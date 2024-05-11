cmake_minimum_required(VERSION 3.28.0)
project(LZ_Collection VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PERFORMANCE_MEASUREMENT ON)
set(TESTING OFF)
set(LOAD_PIZZA OFF)

include(GoogleTest)
include(CTest)
enable_testing()

include_directories(include)
include_directories(include/LZ77)
include_directories(include/LZW)
include_directories(include/ApproxLZ77)

add_subdirectory(externals)
include_directories(externals/malloc_count)
include_directories(externals/libsais/include)

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

# Looks Dumb, Is Dumb 
set(pizza_sources ${CMAKE_SOURCE_DIR}/data/sources.50MB)
set(pizza_proteins ${CMAKE_SOURCE_DIR}/data/proteins.50MB)
if(LOAD_PIZZA AND NOT EXISTS ${pizza_sources})
  message(STATUS "Downloading Samples from Pizza&Chili Corpus")
  file(DOWNLOAD https://pizzachili.dcc.uchile.cl/texts/code/sources.50MB.gz ${pizza_sources}.gz)
  file(DOWNLOAD https://pizzachili.dcc.uchile.cl/texts/protein/proteins.50MB.gz ${pizza_proteins}.gz)
  execute_process(COMMAND gunzip ${pizza_sources}.gz > ${pizza_sources})
  execute_process(COMMAND gunzip ${pizza_proteins}.gz > ${pizza_proteins})
endif()

