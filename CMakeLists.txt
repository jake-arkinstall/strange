cmake_minimum_required(VERSION 3.12)  # Choose whatever minimum version you're comfortable with

project(strange VERSION 0.1)

option(STRANGE_BUILD_TESTS "Build tests" ON)

find_package(fmt REQUIRED)

add_library(strange INTERFACE)


target_include_directories(strange INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(strange INTERFACE fmt::fmt)
target_compile_features(strange INTERFACE cxx_std_20)

if(STRANGE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

install(TARGETS strange EXPORT strangeConfig)
install(DIRECTORY include/ DESTINATION include)
install(EXPORT strangeConfig DESTINATION share/strange/cmake)
export(TARGETS strange FILE strangeConfig.cmake)
