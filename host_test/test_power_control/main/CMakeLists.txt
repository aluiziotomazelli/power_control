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