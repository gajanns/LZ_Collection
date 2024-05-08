set(BINARY ${CMAKE_PROJECT_NAME})

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
file(GLOB_RECURSE HEADERS ${CMAKE_SOURCE_DIR}/include/ *.hpp)

add_executable(${BINARY} ${SOURCES})

if(PERFORMANCE_MEASUREMENT)
    add_compile_definitions(PERF)
    target_link_libraries(${BINARY} malloc_count_lib ${CMAKE_DL_LIBS})
endif()

add_library(${BINARY}_lib STATIC ${SOURCES})