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
include_directories(include/ApproxLZ77Par)

add_subdirectory(externals)
include_directories(externals/malloc_count)
include_directories(externals/libsais/include)
include_directories(externals/unordered_dense/include/ankerl)
include_directories(externals/rabinkarp/include)
include_directories(externals/shardedmap/include/sharded_map)

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
set(pizza_sources ${CMAKE_SOURCE_DIR}/data/sources.100MB)
set(pizza_proteins ${CMAKE_SOURCE_DIR}/data/proteins.100MB)
set(pizza_dna ${CMAKE_SOURCE_DIR}/data/dna.100MB)
set(pizza_english ${CMAKE_SOURCE_DIR}/data/english.100MB)
set(pizza_xml ${CMAKE_SOURCE_DIR}/data/dblp.xml.100MB)

if(LOAD_PIZZA AND NOT EXISTS ${pizza_sources})
  message(STATUS "Downloading Samples from Pizza&Chili Corpus")
  file(DOWNLOAD https://pizzachili.dcc.uchile.cl/texts/code/sources.100MB.gz ${pizza_sources}.gz)
  file(DOWNLOAD https://pizzachili.dcc.uchile.cl/texts/protein/proteins.100MB.gz ${pizza_proteins}.gz)
  file(DOWNLOAD https://pizzachili.dcc.uchile.cl/texts/dna/dna.100MB.gz ${pizza_dna}.gz)
  file(DOWNLOAD https://pizzachili.dcc.uchile.cl/texts/nlang/english.100MB.gz ${pizza_english}.gz)
  file(DOWNLOAD https://pizzachili.dcc.uchile.cl/texts/xml/dblp.xml.100MB.gz ${pizza_xml}.gz)
  
  execute_process(COMMAND gunzip ${pizza_sources}.gz > ${pizza_sources})
  execute_process(COMMAND gunzip ${pizza_proteins}.gz > ${pizza_proteins})
  execute_process(COMMAND gunzip ${pizza_pitches}.gz > ${pizza_pitches})
  execute_process(COMMAND gunzip ${pizza_dna}.gz > ${pizza_dna})
  execute_process(COMMAND gunzip ${pizza_english}.gz > ${pizza_english})
  execute_process(COMMAND gunzip ${pizza_xml}.gz > ${pizza_xml})

endif()

