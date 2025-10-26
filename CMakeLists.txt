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

find_package(PkgConfig REQUIRED)
if (NOT PkgConfig_FOUND)
    message(FATAL_ERROR "pkg-config not found.")
endif()

pkg_check_modules(SNDFILE REQUIRED IMPORTED_TARGET sndfile)
message(" ** sndfile libraries: ${SNDFILE_LIBRARIES}")
message(" ** sndfile include dirs: ${SNDFILE_INCLUDE_DIRS}")

add_library(${PROJECT_NAME} wavio.c)
target_link_libraries(${PROJECT_NAME} PRIVATE PkgConfig::SNDFILE)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${SNDFILE_INCLUDE_DIRS}
)

add_subdirectory(tests)
