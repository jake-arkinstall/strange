find_package(Catch2 3 REQUIRED)

add_executable(strange_tests
    range.cpp
    file.cpp
)

# Link against your header-only library and Catch2
target_link_libraries(strange_tests PRIVATE strange)
target_link_libraries(strange_tests PRIVATE Catch2::Catch2WithMain)
target_compile_options(strange_tests PRIVATE "-O3")

# Discover tests in the executable
include(CTest)
include(Catch)
catch_discover_tests(strange_tests)
