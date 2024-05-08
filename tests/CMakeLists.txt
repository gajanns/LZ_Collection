set(BINARY ${CMAKE_PROJECT_NAME}_tst)

file(GLOB_RECURSE TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)


add_executable(${BINARY} ${TEST_SOURCES})

if(PERFORMANCE_MEASUREMENT)
    add_compile_definitions(PERF)
    target_link_libraries(${BINARY} PUBLIC GTest::gtest GTest::gtest_main ${CMAKE_PROJECT_NAME}_lib malloc_count_lib ${CMAKE_DL_LIBS})
else()
    target_link_libraries(${BINARY} PUBLIC GTest::gtest GTest::gtest_main ${CMAKE_PROJECT_NAME}_lib)
endif()

gtest_discover_tests(${BINARY})