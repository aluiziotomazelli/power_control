cmake_minimum_required(VERSION 3.16)

# Append extra component directories so IDF can find our local components.
list(APPEND EXTRA_COMPONENT_DIRS 
    "../.."                              # The 'power_control' component being tested
    "../gtest"                           # The GTest wrapper component
    "$ENV{IDF_PATH}/tools/mocks/driver"  # Official ESP-IDF driver mocks
)

# Explicitly list the components to be included in the build.
set(COMPONENTS main power_control)

# Standard ESP-IDF project configuration.
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(test_power_control)

if(ENABLE_COVERAGE AND IDF_TARGET STREQUAL "linux")
    find_program(LCOV_PATH lcov REQUIRED)

    # Custom target to run tests and generate a filtered coverage report
    add_custom_target(generate_coverage
        # Cleanup old coverage data
        COMMAND find . -name "*.gcda" -delete
        # Run the test binary
        COMMAND ./test_power_control.elf
        # Capture coverage data
        COMMAND ${LCOV_PATH} --capture --directory . --output-file coverage.info --rc branch_coverage=1 --ignore-errors mismatch
        # Filter to include only files from the component directory, excluding tests and external libs.
        COMMAND ${LCOV_PATH} --extract coverage.info "*/power_control/power_control.cpp" "*/power_control/gpio_hal.cpp" "*/power_control/include/*" --output-file coverage_filtered.info --rc branch_coverage=1 --ignore-errors mismatch,unused,empty
        # List the results for a quick overview in the console
        COMMAND ${LCOV_PATH} --list coverage_filtered.info --rc branch_coverage=1 --ignore-errors mismatch,unused,empty
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Running tests and generating filtered coverage report"
    )
endif()
