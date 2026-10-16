add_executable(pancake_demo demo.cpp)
target_link_libraries(pancake_demo PRIVATE pancake)

# Run the demo for a few hundred frames as a smoke test of the whole engine.
if (PANCAKE_BUILD_TESTS)
    find_program(XVFB_RUN xvfb-run)
    if (XVFB_RUN)
        add_test(NAME demo_smoke COMMAND ${XVFB_RUN} -a -s "-screen 0 1024x768x24" $<TARGET_FILE:pancake_demo> --frames 300)
    else()
        add_test(NAME demo_smoke COMMAND pancake_demo --frames 300)
    endif()
    set_tests_properties(demo_smoke PROPERTIES ENVIRONMENT "ASAN_OPTIONS=detect_leaks=0" LABELS engine TIMEOUT 120
                                               WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endif()
