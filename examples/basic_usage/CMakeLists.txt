cmake_minimum_required(VERSION 3.16)

# The following line is required by the ESP-IDF build system
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# We add the parent directory (the component root) to the component search path.
set(EXTRA_COMPONENT_DIRS ../../)

set(COMPONENTS main power_control)

project(basic_usage)
