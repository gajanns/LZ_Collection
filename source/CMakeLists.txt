set(BINARY ${CMAKE_PROJECT_NAME})

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
file(GLOB_RECURSE HEADERS ${CMAKE_SOURCE_DIR}/include/ *.hpp)

add_executable(${BINARY} ${SOURCES})

if(PERFORMANCE_MEASUREMENT)
    add_compile_definitions(PERF)
endif()

add_library(${BINARY}_lib STATIC ${SOURCES})