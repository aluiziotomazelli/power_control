idf_component_register(
    SRCS 
        "power_control.cpp"
        "gpio_hal.cpp"
    
    INCLUDE_DIRS 
        "include"
    
    REQUIRES 
        driver
)

if(ENABLE_COVERAGE AND IDF_TARGET STREQUAL "linux")
    target_compile_options(${COMPONENT_LIB} PUBLIC --coverage)
    target_link_options(${COMPONENT_LIB} PUBLIC --coverage)
endif()
