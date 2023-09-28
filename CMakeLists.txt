cmake_minimum_required(VERSION 3.12)  # Choose whatever minimum version you're comfortable with

project(strange VERSION 0.1)

# Specify that it's a header-only library
add_library(strange INTERFACE)


# Specify the include directories
target_include_directories(strange INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(strange INTERFACE cxx_std_20)

enable_testing()
add_subdirectory(test)

# Make the package discoverable
install(TARGETS strange EXPORT strangeConfig)

# Specify where the headers should be installed
install(DIRECTORY include/ DESTINATION include)

# Generate and install the CMake config file
install(EXPORT strangeConfig DESTINATION share/strange/cmake)

export(TARGETS strange FILE strangeConfig.cmake)

