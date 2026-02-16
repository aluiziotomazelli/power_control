idf_build_get_property(target IDF_TARGET)
if(NOT ${target} STREQUAL "linux")
    return()
endif()

idf_component_register()

if(NOT CMAKE_BUILD_EARLY_EXPANSION)
    include(FetchContent)
    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.tar.gz
      DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )

    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    set(BUILD_GMOCK ON CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(googletest)

    target_link_libraries(${COMPONENT_LIB} INTERFACE gtest gmock)
endif()
