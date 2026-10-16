# Unit tests run without a window. Engine tests open a real window, so they need a display;
# when xvfb-run is available they are wrapped in a virtual one.

find_program(XVFB_RUN xvfb-run)

set(PANCAKE_TEST_ENVIRONMENT
    "ASAN_OPTIONS=detect_leaks=1"
    "LSAN_OPTIONS=suppressions=${CMAKE_CURRENT_SOURCE_DIR}/lsan.supp:print_suppressions=0"
)

function(pancake_test name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} PRIVATE pancake)
    add_test(NAME ${name} COMMAND ${name})
    set_tests_properties(${name} PROPERTIES ENVIRONMENT "${PANCAKE_TEST_ENVIRONMENT}" LABELS unit)
endfunction()

function(pancake_engine_test name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} PRIVATE pancake)
    if (XVFB_RUN)
        add_test(NAME ${name} COMMAND ${XVFB_RUN} -a -s "-screen 0 1024x768x24" $<TARGET_FILE:${name}>)
    else()
        add_test(NAME ${name} COMMAND ${name})
    endif()
    # Leak checking is off here: GL drivers leak from libraries they unload, which cannot be suppressed by name.
    set_tests_properties(${name} PROPERTIES ENVIRONMENT "ASAN_OPTIONS=detect_leaks=0" LABELS engine TIMEOUT 120
                                            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endfunction()

pancake_test(test_spatial)
pancake_test(test_physics)
pancake_test(test_raycast)
pancake_test(test_serialise)
pancake_test(test_entity)
pancake_test(test_time)
pancake_engine_test(test_engine)
pancake_engine_test(test_time_engine)
