cmake_minimum_required(VERSION 3.26)

set(PROJECT_NAME wavio)
project(${PROJECT_NAME} LANGUAGES C VERSION 0.1.1)

# std
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

# Export compile commands for tooling
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Enable position independent code for libraries
set(CMAKE_POSITION_INDEPENDENT_CODE ON)



add_subdirectory(tests)
