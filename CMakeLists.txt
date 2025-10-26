cmake_minimum_required(VERSION 3.26)

set(PROJECT_NAME wavio)
project(${PROJECT_NAME} LANGUAGES C VERSION 0.1.0)

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

# macOS Homebrew prefix
if(APPLE)
    if(NOT DEFINED HOMEBREW_PREFIX)
        execute_process(COMMAND brew --prefix
                        OUTPUT_VARIABLE HOMEBREW_PREFIX
                        OUTPUT_STRIP_TRAILING_WHITESPACE)
    endif()
endif()

# Find headers
find_path(SNDFILE_INCLUDE_DIR NAMES sndfile.h PATHS
    ${HOMEBREW_PREFIX}/include
    /usr/local/include
    /usr/include
)

# Find library
find_library(SNDFILE_LIBRARY NAMES sndfile PATHS
    ${HOMEBREW_PREFIX}/lib
    /usr/local/lib
    /usr/lib
)

message(STATUS "SNDFILE_INCLUDE_DIR: ${SNDFILE_INCLUDE_DIR}")
message(STATUS "SNDFILE_LIBRARY: ${SNDFILE_LIBRARY}")

add_library(${PROJECT_NAME} wavio.c)
target_link_libraries(${PROJECT_NAME} PRIVATE ${SNDFILE_LIBRARY})
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${SNDFILE_INCLUDE_DIR}
)

add_subdirectory(tests)
