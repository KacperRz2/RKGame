include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.17.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

set(RELEASE_CONFIG "$<CONFIG:Release>")

add_library(KGame_test ../src/function.c)
target_link_libraries(KGame_test SDL3-static)
target_include_directories(KGame_test PRIVATE ../src)

set_target_properties(KGame_test PROPERTIES EXCLUDE_FROM_ALL ${RELEASE_CONFIG})

add_executable(unit_tests tests.cpp)
target_link_libraries(unit_tests GTest::gtest_main KGame_test)

set_target_properties(unit_tests PROPERTIES EXCLUDE_FROM_ALL ${RELEASE_CONFIG})

include(GoogleTest)

gtest_discover_tests(unit_tests)
