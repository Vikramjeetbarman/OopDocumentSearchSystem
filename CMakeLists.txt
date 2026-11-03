cmake_minimum_required(VERSION 3.10)

project(MiniSearchEngine)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(src)

# Collect source files (only .cpp)
file(GLOB_RECURSE SOURCES
    src/cli/*.cpp
    src/user/*.cpp
    src/storage/*.cpp
    src/index/*.cpp
    src/search/*.cpp
    src/ranking/*.cpp
    src/filter/*.cpp
    src/security/*.cpp
    src/util/*.cpp
    src/observer/*.cpp
   

)

# Create executable
add_executable(cli ${SOURCES})

# Optional: warnings (recommended)
if (MSVC)
    target_compile_options(cli PRIVATE /W4)
else()
    target_compile_options(cli PRIVATE -Wall -Wextra -Wpedantic)
endif()