cmake_minimum_required(VERSION 3.16)

# The following line is required by the ESP-IDF build system
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# We add the parent directory (the component root) to the component search path.
list(APPEND EXTRA_COMPONENT_DIRS "../../..")
list(APPEND EXTRA_COMPONENT_DIRS "../../external")
list(APPEND EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/components")

set(COMPONENTS main power_control idf_hals)

project(basic_usage)
