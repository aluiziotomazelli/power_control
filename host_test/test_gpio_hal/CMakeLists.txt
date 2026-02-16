add_executable(test_gpio_hal
    test_gpio_hal.cpp
    ../../gpio_hal.cpp
)

target_include_directories(test_gpio_hal PRIVATE
    ../../include
    ../mocks/esp
)

target_link_libraries(test_gpio_hal
    gtest_main
)

add_test(NAME test_gpio_hal COMMAND test_gpio_hal)
