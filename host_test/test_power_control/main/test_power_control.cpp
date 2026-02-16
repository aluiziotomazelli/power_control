#include "gpio_hal_interface.hpp"
#include "power_control.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "gpio_hal_interface.hpp"
#include "power_control.hpp"
#include "driver/gpio.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::_;
using ::testing::Field;
using ::testing::Return;

class MockGpioHAL : public IGpioHAL
{
public:
    MOCK_METHOD(esp_err_t, reset_pin, (gpio_num_t pin), (override));
    MOCK_METHOD(esp_err_t, config, (const gpio_config_t &config), (override));
    MOCK_METHOD(esp_err_t, set_level, (gpio_num_t pin, bool level), (override));
    MOCK_METHOD(esp_err_t, set_drive_capability, (gpio_num_t gpio_num, gpio_drive_cap_t strength), (override));
};

// Fixture para reutilizar setup
class PowerControlTest : public ::testing::Test
{
protected:
    MockGpioHAL mock_gpio;
    const gpio_num_t TEST_PIN = GPIO_NUM_4;
};

TEST_F(PowerControlTest, NormalLogic_TurnOff)
{
    PowerControl pc(mock_gpio, TEST_PIN, false, false);

    // Expectativas do init()
    EXPECT_CALL(mock_gpio, reset_pin(TEST_PIN)).WillOnce(Return(ESP_OK));
    EXPECT_CALL(mock_gpio, config(_)).WillOnce(Return(ESP_OK));
    EXPECT_CALL(mock_gpio, set_level(TEST_PIN, false)).WillOnce(Return(ESP_OK)); // initial_on = false

    EXPECT_EQ(ESP_OK, pc.init());

    // Test turnOff - deve setar nível LOW (false)
    EXPECT_CALL(mock_gpio, set_level(TEST_PIN, false)).WillOnce(Return(ESP_OK));
    EXPECT_EQ(ESP_OK, pc.turn_off());
    EXPECT_FALSE(pc.is_on());
}

TEST_F(PowerControlTest, NormalLogic_TurnOn)
{
    PowerControl pc(mock_gpio, TEST_PIN, false, false);

    EXPECT_CALL(mock_gpio, reset_pin(TEST_PIN)).WillOnce(Return(ESP_OK));
    EXPECT_CALL(mock_gpio, config(_)).WillOnce(Return(ESP_OK));
    EXPECT_CALL(mock_gpio, set_level(TEST_PIN, false)).WillOnce(Return(ESP_OK));

    EXPECT_EQ(ESP_OK, pc.init());

    // Test turnOn - deve setar nível HIGH (true)
    EXPECT_CALL(mock_gpio, set_level(TEST_PIN, true)).WillOnce(Return(ESP_OK));
    EXPECT_EQ(ESP_OK, pc.turn_on());
    EXPECT_TRUE(pc.is_on());
}

TEST_F(PowerControlTest, NormalLogic_Toggle)
{
    PowerControl pc(mock_gpio, TEST_PIN, false, false);

    EXPECT_CALL(mock_gpio, reset_pin(TEST_PIN)).WillOnce(Return(ESP_OK));
    EXPECT_CALL(mock_gpio, config(_)).WillOnce(Return(ESP_OK));
    EXPECT_CALL(mock_gpio, set_level(TEST_PIN, false)).WillOnce(Return(ESP_OK)); // init OFF

    EXPECT_EQ(ESP_OK, pc.init());

    // Toggle: OFF -> ON
    EXPECT_CALL(mock_gpio, set_level(TEST_PIN, true)).WillOnce(Return(ESP_OK));
    EXPECT_EQ(ESP_OK, pc.toggle());
    EXPECT_TRUE(pc.is_on());

    // Toggle: ON -> OFF
    EXPECT_CALL(mock_gpio, set_level(TEST_PIN, false)).WillOnce(Return(ESP_OK));
    EXPECT_EQ(ESP_OK, pc.toggle());
    EXPECT_FALSE(pc.is_on());
}

TEST_F(PowerControlTest, InvertedLogic_TurnOff)
{
    PowerControl pc(mock_gpio, TEST_PIN, true, false); // inverted = true

    EXPECT_CALL(mock_gpio, reset_pin(TEST_PIN)).WillOnce(Return(ESP_OK));
    EXPECT_CALL(mock_gpio, config(_)).WillOnce(Return(ESP_OK));
    EXPECT_CALL(mock_gpio, set_level(TEST_PIN, true)).WillOnce(Return(ESP_OK)); // invertido: OFF = HIGH

    EXPECT_EQ(ESP_OK, pc.init());

    // TurnOff com lógica invertida - físico HIGH
    EXPECT_CALL(mock_gpio, set_level(TEST_PIN, true)).WillOnce(Return(ESP_OK));
    EXPECT_EQ(ESP_OK, pc.turn_off());
    EXPECT_FALSE(pc.is_on());
}

TEST_F(PowerControlTest, InvertedLogic_TurnOn)
{
    PowerControl pc(mock_gpio, TEST_PIN, true, false);

    EXPECT_CALL(mock_gpio, reset_pin(TEST_PIN)).WillOnce(Return(ESP_OK));
    EXPECT_CALL(mock_gpio, config(_)).WillOnce(Return(ESP_OK));
    EXPECT_CALL(mock_gpio, set_level(TEST_PIN, true)).WillOnce(Return(ESP_OK)); // init OFF invertido

    EXPECT_EQ(ESP_OK, pc.init());

    // TurnOn com lógica invertida - físico LOW
    EXPECT_CALL(mock_gpio, set_level(TEST_PIN, false)).WillOnce(Return(ESP_OK));
    EXPECT_EQ(ESP_OK, pc.turn_on());
    EXPECT_TRUE(pc.is_on());
}

TEST_F(PowerControlTest, Deinit_Success)
{
    PowerControl pc(mock_gpio, TEST_PIN, false, false);

    EXPECT_CALL(mock_gpio, reset_pin(TEST_PIN)).Times(2); // init + deinit
    EXPECT_CALL(mock_gpio, config(_)).WillOnce(Return(ESP_OK));
    EXPECT_CALL(mock_gpio, set_level(TEST_PIN, _)).Times(2); // init + deinit force low

    pc.init();
    EXPECT_EQ(ESP_OK, pc.deinit());
    EXPECT_FALSE(pc.is_initialized());
}

TEST_F(PowerControlTest, IntentionalLeak)
{
    int *leak = new int(42); // Aloca mas não deleta
    EXPECT_EQ(*leak, 42);
    // delete leak; // Comentado de propósito
}