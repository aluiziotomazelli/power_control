cmake_minimum_required(VERSION 3.16)

list(APPEND EXTRA_COMPONENT_DIRS 
    "../.."                              # Component power_control
    "../gtest"                           # GTest
    "$ENV{IDF_PATH}/tools/mocks/driver"  # Mocks ESP
)

set(COMPONENTS main power_control)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(test_power_control)