cmake_minimum_required(VERSION 3.20)
project(the-sweeper)
set(CMAKE_CXX_STANDARD 20)
set(GCC_COVERAGE_COMPILE_FLAGS "-Wall -pedantic -std=c++20")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.x
)

FetchContent_MakeAvailable(SFML)
file(GLOB_RECURSE SRC "src/*.cpp" "src/*.hpp" "src/*.h")
file(GLOB BASE "*.cpp" "*.hpp" "*.h")

add_executable(the-sweeper
        ${SRC}
        ${BASE}
        GameInstance.cpp
        GameInstance.h
)

# find_package(SFML COMPONENTS graphics window system REQUIRED)
target_link_libraries(the-sweeper sfml-system sfml-window sfml-graphics sfml-audio)
target_compile_features(the-sweeper PRIVATE cxx_std_20)
target_link_directories(the-sweeper PRIVATE SFML::Graphics)

if (WIN32)
    add_custom_command(
            TARGET the-sweeper
            PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy
            ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:the-sweeper>
            VERBATIM)
endif ()