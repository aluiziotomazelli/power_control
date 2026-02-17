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

if(IDF_TARGET STREQUAL "linux")
    find_program(LCOV_PATH lcov REQUIRED)
    find_program(GENHTML_PATH genhtml REQUIRED)

    set(COVERAGE_DIR "${PROJECT_SOURCE_DIR}/../../coverage")

    # Custom target to run tests and generate a filtered coverage report
    add_custom_target(generate_coverage
        # Cleanup old coverage data and old report
        COMMAND find . -name "*.gcda" -delete
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${COVERAGE_DIR}
        # Run the test binary
        COMMAND ./test_power_control.elf
        # Capture coverage data
        COMMAND ${LCOV_PATH} --capture --directory . --output-file coverage.info --rc branch_coverage=1 --ignore-errors mismatch,inconsistent
        # Filter to include only files from the component directory, excluding tests and external libs.
        # We use a pattern that matches the component name but carefully avoid other things.
        COMMAND ${LCOV_PATH} --extract coverage.info "*/power_control/src/*" "*/power_control/include/*" --output-file coverage_filtered.info --rc branch_coverage=1 --ignore-errors mismatch,inconsistent,unused,empty
        # Generate HTML report
        COMMAND ${GENHTML_PATH} coverage_filtered.info --output-directory ${COVERAGE_DIR} --title "Power Control Component Coverage" --rc branch_coverage=1 --ignore-errors mismatch,inconsistent,unused
        # List the results for a quick overview in the console
        COMMAND ${LCOV_PATH} --list coverage_filtered.info --rc branch_coverage=1 --ignore-errors mismatch,inconsistent,unused,empty
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Running tests and generating filtered coverage report"
    )
endif()
