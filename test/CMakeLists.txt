find_package(Catch REQUIRED)

add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${CATCH_INCLUDE_DIR})

function(add_catch_test name)
    set(TEST_TARGET test_${name})
    add_executable(${TEST_TARGET}
        ${name}.cpp
    )
    target_link_libraries(${TEST_TARGET}
        ${PROJECT_NAME} Catch
    )
    set_target_properties(${TEST_TARGET} PROPERTIES
        CXX_STANDARD 14
    )
    add_test(${TEST_TARGET} ${TEST_TARGET})
endfunction()

add_catch_test(specs)
add_catch_test(variant)
add_catch_test(unresolved_handler)
add_catch_test(udt)