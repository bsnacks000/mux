set(UNIT_TEST_NAME mux_unit_tests)

file(GLOB_RECURSE SRC_FILES *.c)

add_executable(${UNIT_TEST_NAME}
    ${SRC_FILES}
)


target_link_libraries(${UNIT_TEST_NAME} PRIVATE ${PROJECT_NAME})
target_compile_options(${UNIT_TEST_NAME} PRIVATE
    -Wextra -Wall -Wpedantic -Werror
)
