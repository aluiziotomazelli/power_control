idf_component_register(
    SRCS 
        "main.cpp"
        "test_power_control.cpp"
    INCLUDE_DIRS 
        "."
    REQUIRES 
        gtest
        power_control
        
    WHOLE_ARCHIVE
)

if(IDF_TARGET STREQUAL "linux")
    target_compile_options(${COMPONENT_LIB} PUBLIC --coverage)
    target_link_options(${COMPONENT_LIB} PUBLIC --coverage)
endif()