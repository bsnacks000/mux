set(UNIT_TEST_NAME wavio_unit_tests)

file(GLOB_RECURSE SRC_FILES *.c)

add_executable(${UNIT_TEST_NAME}
    ${SRC_FILES}
)

# Prefer system packages
#set(CMAKE_FIND_PACKAGE_PREFER_CONFIG TRUE)

set(BUILD_SHARED_LIBS ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SNDFILE REQUIRED IMPORTED_TARGET sndfile)

target_link_libraries(${UNIT_TEST_NAME} PRIVATE PkgConfig::SNDFILE)
target_include_directories(${UNIT_TEST_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}
)
target_compile_options(${UNIT_TEST_NAME} PRIVATE
    -Wextra -Wall -Wpedantic -Werror
)
