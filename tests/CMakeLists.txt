set(UNIT_TEST_NAME wavio_unit_tests)

file(GLOB_RECURSE SRC_FILES *.c)

add_executable(${UNIT_TEST_NAME}
    ${SRC_FILES}
)

# Prefer system packages
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG TRUE)


find_package(PkgConfig REQUIRED)
if (NOT PkgConfig_FOUND)
    message(FATAL_ERROR "pkg-config not found.")
endif()

pkg_check_modules(SNDFILE REQUIRED IMPORTED_TARGET sndfile)
message(" ** sndfile libraries: ${SNDFILE_LIBRARIES}")
message(" ** sndfile include dirs: ${SNDFILE_INCLUDE_DIRS}")

target_link_libraries(${UNIT_TEST_NAME} PRIVATE ${SNDFILE_LIBRARIES})
target_include_directories(${UNIT_TEST_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${SNDFILE_INCLUDE_DIRS}
)
target_compile_options(${UNIT_TEST_NAME} PRIVATE
    -Wextra -Wall -Wpedantic -Werror
)
