idf_component_register(
    SRCS 
        "power_control.cpp"
        "gpio_hal.cpp"
    
    INCLUDE_DIRS 
        "include"
    
    REQUIRES 
        driver
)
