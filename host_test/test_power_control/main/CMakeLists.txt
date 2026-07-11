idf_component_register(
    SRCS 
        "main.cpp"
        "test_power_control.cpp"
    INCLUDE_DIRS 
        "."
        "../../../external/idf_hals/mocks"
    REQUIRES 
        gtest
        power_control
        idf_hals
        
    WHOLE_ARCHIVE
)

if(IDF_TARGET STREQUAL "linux")
    target_compile_options(${COMPONENT_LIB} PUBLIC --coverage)
    target_link_options(${COMPONENT_LIB} PUBLIC --coverage)
endif()