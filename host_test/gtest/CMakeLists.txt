idf_build_get_property(target IDF_TARGET)
if(NOT ${target} STREQUAL "linux")
    return()
endif()

set(GTEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/googletest-release-1.12.1")

idf_component_register(
    SRCS
        "${GTEST_DIR}/googletest/src/gtest-all.cc"
        "${GTEST_DIR}/googlemock/src/gmock-all.cc"
    INCLUDE_DIRS
        "${GTEST_DIR}/googletest/include"
        "${GTEST_DIR}/googlemock/include"
    PRIV_INCLUDE_DIRS
        "${GTEST_DIR}/googletest"
        "${GTEST_DIR}/googlemock"
)
