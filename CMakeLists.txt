idf_component_register(
    SRCS 
        "src/power_control.cpp"
        "src/gpio_hal.cpp"
    
    INCLUDE_DIRS 
        "include"
    
    REQUIRES 
        driver
)

if(IDF_TARGET STREQUAL "linux")
    target_compile_options(${COMPONENT_LIB} PUBLIC --coverage)
    target_link_options(${COMPONENT_LIB} PUBLIC --coverage)
endif()
