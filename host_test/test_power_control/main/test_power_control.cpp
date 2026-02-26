#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "driver/gpio.h"

#include "i_gpio_hal.hpp"
#include "power_control.hpp"

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

TEST_F(PowerControlTest, Init_FailsWhenResetPinFails)
{
    PowerControl pc(mock_gpio, TEST_PIN, false, false);

    // Expect reset_pin to fail
    EXPECT_CALL(mock_gpio, reset_pin(TEST_PIN)).WillOnce(Return(ESP_ERR_INVALID_ARG));

    // config should NOT be called because reset_pin failed
    EXPECT_CALL(mock_gpio, config(_)).Times(0);
    EXPECT_CALL(mock_gpio, set_level(_, _)).Times(0);

    // init should return the error from reset_pin
    EXPECT_EQ(ESP_ERR_INVALID_ARG, pc.init());
    EXPECT_FALSE(pc.is_initialized());
}

TEST_F(PowerControlTest, Init_FailsWhenConfigFails)
{
    PowerControl pc(mock_gpio, TEST_PIN, false, false);

    // Reset pin succeeds
    EXPECT_CALL(mock_gpio, reset_pin(TEST_PIN)).WillOnce(Return(ESP_OK));

    // Config fails
    EXPECT_CALL(mock_gpio, config(_)).WillOnce(Return(ESP_ERR_INVALID_ARG));

    // set_level should NOT be called because config failed
    EXPECT_CALL(mock_gpio, set_level(_, _)).Times(0);

    // init should return the error from config
    EXPECT_EQ(ESP_ERR_INVALID_ARG, pc.init());
    EXPECT_FALSE(pc.is_initialized());
}

TEST_F(PowerControlTest, Init_WithInitialOnTrue)
{
    PowerControl pc(mock_gpio, TEST_PIN, false, true); // initial_on = true

    EXPECT_CALL(mock_gpio, reset_pin(TEST_PIN)).WillOnce(Return(ESP_OK));
    EXPECT_CALL(mock_gpio, config(_)).WillOnce(Return(ESP_OK));
    EXPECT_CALL(mock_gpio, set_level(TEST_PIN, true)).WillOnce(Return(ESP_OK)); // Should turn ON initially

    EXPECT_EQ(ESP_OK, pc.init());
    EXPECT_TRUE(pc.is_initialized());
    EXPECT_TRUE(pc.is_on()); // Verify state is ON
}

TEST_F(PowerControlTest, ApplyGpio_FailsWhenNotInitialized)
{
    PowerControl pc(mock_gpio, TEST_PIN, false, false);

    // Don't initialize the component

    // Try to turn on/off without initialization
    EXPECT_EQ(ESP_ERR_INVALID_STATE, pc.turn_on());
    EXPECT_EQ(ESP_ERR_INVALID_STATE, pc.turn_off());
    EXPECT_EQ(ESP_ERR_INVALID_STATE, pc.toggle());

    // Verify that no GPIO operations were attempted
    EXPECT_CALL(mock_gpio, set_level(_, _)).Times(0);
}

TEST_F(PowerControlTest, ApplyGpio_FailsWhenSetLevelFails)
{
    PowerControl pc(mock_gpio, TEST_PIN, false, false);

    // Successful initialization
    EXPECT_CALL(mock_gpio, reset_pin(TEST_PIN)).WillOnce(Return(ESP_OK));
    EXPECT_CALL(mock_gpio, config(_)).WillOnce(Return(ESP_OK));
    EXPECT_CALL(mock_gpio, set_level(TEST_PIN, false)).WillOnce(Return(ESP_OK));

    EXPECT_EQ(ESP_OK, pc.init());

    // Now make set_level fail during turn_on
    EXPECT_CALL(mock_gpio, set_level(TEST_PIN, true)).WillOnce(Return(ESP_ERR_INVALID_ARG));

    // The operation should fail and is_on should remain false
    EXPECT_EQ(ESP_ERR_INVALID_ARG, pc.turn_on());
    EXPECT_FALSE(pc.is_on());
}

TEST_F(PowerControlTest, SetDriveCapability_Success)
{
    PowerControl pc(mock_gpio, TEST_PIN, false, false);

    // Initialize successfully
    EXPECT_CALL(mock_gpio, reset_pin(TEST_PIN)).WillOnce(Return(ESP_OK));
    EXPECT_CALL(mock_gpio, config(_)).WillOnce(Return(ESP_OK));
    EXPECT_CALL(mock_gpio, set_level(TEST_PIN, false)).WillOnce(Return(ESP_OK));

    EXPECT_EQ(ESP_OK, pc.init());
    EXPECT_TRUE(pc.is_initialized());

    // Test set_drive_capability success
    gpio_drive_cap_t strength = GPIO_DRIVE_CAP_1;
    EXPECT_CALL(mock_gpio, set_drive_capability(TEST_PIN, strength)).WillOnce(Return(ESP_OK));

    EXPECT_EQ(ESP_OK, pc.set_drive_capability(strength));
}

TEST_F(PowerControlTest, SetDriveCapability_Failure)
{
    PowerControl pc(mock_gpio, TEST_PIN, false, false);

    // Initialize successfully
    EXPECT_CALL(mock_gpio, reset_pin(TEST_PIN)).WillOnce(Return(ESP_OK));
    EXPECT_CALL(mock_gpio, config(_)).WillOnce(Return(ESP_OK));
    EXPECT_CALL(mock_gpio, set_level(TEST_PIN, false)).WillOnce(Return(ESP_OK));

    EXPECT_EQ(ESP_OK, pc.init());
    EXPECT_TRUE(pc.is_initialized());

    // Test set_drive_capability failure
    gpio_drive_cap_t strength = GPIO_DRIVE_CAP_1;
    EXPECT_CALL(mock_gpio, set_drive_capability(TEST_PIN, strength)).WillOnce(Return(ESP_ERR_INVALID_ARG));

    EXPECT_EQ(ESP_ERR_INVALID_ARG, pc.set_drive_capability(strength));
}

TEST_F(PowerControlTest, SetDriveCapability_WhenNotInitialized)
{
    PowerControl pc(mock_gpio, TEST_PIN, false, false);

    // Don't initialize the component
    EXPECT_FALSE(pc.is_initialized());

    // Try to set drive capability without initialization
    gpio_drive_cap_t strength = GPIO_DRIVE_CAP_1;

    // HAL function should NOT be called because initialization check fails first
    EXPECT_CALL(mock_gpio, set_drive_capability(_, _)).Times(0);

    EXPECT_EQ(ESP_ERR_INVALID_STATE, pc.set_drive_capability(strength));
}

TEST_F(PowerControlTest, SetDriveCapability_AfterDeinit)
{
    PowerControl pc(mock_gpio, TEST_PIN, false, false);

    // Initialize successfully
    EXPECT_CALL(mock_gpio, reset_pin(TEST_PIN)).WillOnce(Return(ESP_OK));
    EXPECT_CALL(mock_gpio, config(_)).WillOnce(Return(ESP_OK));
    EXPECT_CALL(mock_gpio, set_level(TEST_PIN, false)).WillOnce(Return(ESP_OK));

    EXPECT_EQ(ESP_OK, pc.init());
    EXPECT_TRUE(pc.is_initialized());

    // Deinitialize successfully
    EXPECT_CALL(mock_gpio, set_level(TEST_PIN, 0)).WillOnce(Return(ESP_OK));
    EXPECT_CALL(mock_gpio, reset_pin(TEST_PIN)).WillOnce(Return(ESP_OK));

    EXPECT_EQ(ESP_OK, pc.deinit());
    EXPECT_FALSE(pc.is_initialized());

    // Try to set drive capability after deinit
    gpio_drive_cap_t strength = GPIO_DRIVE_CAP_1;

    // HAL function should NOT be called because component is not initialized
    EXPECT_CALL(mock_gpio, set_drive_capability(_, _)).Times(0);

    EXPECT_EQ(ESP_ERR_INVALID_STATE, pc.set_drive_capability(strength));
}

TEST_F(PowerControlTest, Deinit_PartialFailure_SetLevelFailsButResetSucceeds)
{
    PowerControl pc(mock_gpio, TEST_PIN, false, false);

    // Successful initialization
    EXPECT_CALL(mock_gpio, reset_pin(TEST_PIN)).WillOnce(Return(ESP_OK));
    EXPECT_CALL(mock_gpio, config(_)).WillOnce(Return(ESP_OK));
    EXPECT_CALL(mock_gpio, set_level(TEST_PIN, false)).WillOnce(Return(ESP_OK));

    EXPECT_EQ(ESP_OK, pc.init());

    // set_level fails during deinit
    EXPECT_CALL(mock_gpio, set_level(TEST_PIN, 0)).WillOnce(Return(ESP_ERR_INVALID_ARG));

    // reset_pin succeeds
    EXPECT_CALL(mock_gpio, reset_pin(TEST_PIN)).WillOnce(Return(ESP_OK));

    // deinit should return the error from set_level
    EXPECT_EQ(ESP_ERR_INVALID_ARG, pc.deinit());

    // Component should be marked as deinitialized despite the error
    EXPECT_FALSE(pc.is_initialized());
}

TEST_F(PowerControlTest, Deinit_PartialFailure_SetLevelSucceedsButResetFails)
{
    PowerControl pc(mock_gpio, TEST_PIN, false, false);

    // Successful initialization
    EXPECT_CALL(mock_gpio, reset_pin(TEST_PIN)).WillOnce(Return(ESP_OK));
    EXPECT_CALL(mock_gpio, config(_)).WillOnce(Return(ESP_OK));
    EXPECT_CALL(mock_gpio, set_level(TEST_PIN, false)).WillOnce(Return(ESP_OK));

    EXPECT_EQ(ESP_OK, pc.init());

    // set_level succeeds during deinit
    EXPECT_CALL(mock_gpio, set_level(TEST_PIN, 0)).WillOnce(Return(ESP_OK));

    // reset_pin fails
    EXPECT_CALL(mock_gpio, reset_pin(TEST_PIN)).WillOnce(Return(ESP_ERR_INVALID_ARG));

    // deinit should return the error from reset_pin
    EXPECT_EQ(ESP_ERR_INVALID_ARG, pc.deinit());

    // Component should be marked as deinitialized despite the error
    EXPECT_FALSE(pc.is_initialized());
}

TEST_F(PowerControlTest, Deinit_CompleteFailure)
{
    PowerControl pc(mock_gpio, TEST_PIN, false, false);

    // Successful initialization
    EXPECT_CALL(mock_gpio, reset_pin(TEST_PIN)).WillOnce(Return(ESP_OK));
    EXPECT_CALL(mock_gpio, config(_)).WillOnce(Return(ESP_OK));
    EXPECT_CALL(mock_gpio, set_level(TEST_PIN, false)).WillOnce(Return(ESP_OK));

    EXPECT_EQ(ESP_OK, pc.init());

    // Both operations fail during deinit
    EXPECT_CALL(mock_gpio, set_level(TEST_PIN, 0)).WillOnce(Return(ESP_ERR_INVALID_ARG));

    EXPECT_CALL(mock_gpio, reset_pin(TEST_PIN)).WillOnce(Return(ESP_ERR_INVALID_STATE));

    // deinit should return the first error
    EXPECT_EQ(ESP_ERR_INVALID_ARG, pc.deinit());

    // Component should still be marked as deinitialized
    EXPECT_FALSE(pc.is_initialized());
}

TEST_F(PowerControlTest, GetPin_ReturnsCorrectPin)
{
    PowerControl pc(mock_gpio, TEST_PIN, false, false);

    // Test get_pin before initialization
    EXPECT_EQ(TEST_PIN, pc.get_pin());

    // Initialize (minimal expectations for init)
    EXPECT_CALL(mock_gpio, reset_pin(TEST_PIN)).WillOnce(Return(ESP_OK));
    EXPECT_CALL(mock_gpio, config(_)).WillOnce(Return(ESP_OK));
    EXPECT_CALL(mock_gpio, set_level(TEST_PIN, false)).WillOnce(Return(ESP_OK));

    EXPECT_EQ(ESP_OK, pc.init());

    // Test get_pin after initialization
    EXPECT_EQ(TEST_PIN, pc.get_pin());

    // Deinitialize
    EXPECT_CALL(mock_gpio, set_level(TEST_PIN, 0)).WillOnce(Return(ESP_OK));
    EXPECT_CALL(mock_gpio, reset_pin(TEST_PIN)).WillOnce(Return(ESP_OK));

    EXPECT_EQ(ESP_OK, pc.deinit());

    // Test get_pin after deinitialization
    EXPECT_EQ(TEST_PIN, pc.get_pin());
}

TEST_F(PowerControlTest, GetPin_WithDifferentPinValues)
{
    // Test with different GPIO pins
    const gpio_num_t test_pins[] = {
        GPIO_NUM_0,
        GPIO_NUM_2,
        GPIO_NUM_5,
        GPIO_NUM_12,
        GPIO_NUM_13,
        GPIO_NUM_14,
        GPIO_NUM_15,
        GPIO_NUM_16,
        GPIO_NUM_25,
        GPIO_NUM_26,
        GPIO_NUM_32,
        GPIO_NUM_33};

    for (gpio_num_t pin : test_pins) {
        PowerControl pc(mock_gpio, pin, false, false);
        EXPECT_EQ(pin, pc.get_pin());
    }
}

TEST_F(PowerControlTest, GetPin_WithDifferentLogicConfigurations)
{
    // Test that get_pin returns the same pin regardless of logic configuration

    // Normal logic, initial off
    PowerControl pc1(mock_gpio, TEST_PIN, false, false);
    EXPECT_EQ(TEST_PIN, pc1.get_pin());

    // Normal logic, initial on
    PowerControl pc2(mock_gpio, TEST_PIN, false, true);
    EXPECT_EQ(TEST_PIN, pc2.get_pin());

    // Inverted logic, initial off
    PowerControl pc3(mock_gpio, TEST_PIN, true, false);
    EXPECT_EQ(TEST_PIN, pc3.get_pin());

    // Inverted logic, initial on
    PowerControl pc4(mock_gpio, TEST_PIN, true, true);
    EXPECT_EQ(TEST_PIN, pc4.get_pin());
}

TEST_F(PowerControlTest, GetPin_ConstCorrectness)
{
    PowerControl pc(mock_gpio, TEST_PIN, false, false);

    // Test that get_pin can be called on const object
    const PowerControl &const_pc = pc;
    EXPECT_EQ(TEST_PIN, const_pc.get_pin());

    // Test chaining or using in expressions
    EXPECT_TRUE(pc.get_pin() == TEST_PIN);
    EXPECT_FALSE(pc.get_pin() == GPIO_NUM_5);

    // Test that get_pin returns the same value consistently
    gpio_num_t first_call = pc.get_pin();
    gpio_num_t second_call = pc.get_pin();
    EXPECT_EQ(first_call, second_call);
}

//==============================================================================
//  Intgegration tests
//==============================================================================
TEST_F(PowerControlTest, MultipleInstances_WorkIndependently)
{
    // Make two instances with different configurations
    PowerControl pc1(mock_gpio, GPIO_NUM_4, false, false); // Normal, OFF
    PowerControl pc2(mock_gpio, GPIO_NUM_5, true, true);   // Inverted, ON

    // Configure expectations for init for each instance
    EXPECT_CALL(mock_gpio, reset_pin(GPIO_NUM_4)).WillOnce(Return(ESP_OK));
    EXPECT_CALL(mock_gpio, config(testing::Field(&gpio_config_t::pin_bit_mask, 1ULL << GPIO_NUM_4)))
        .WillOnce(Return(ESP_OK));
    EXPECT_CALL(mock_gpio, set_level(GPIO_NUM_4, false)).WillOnce(Return(ESP_OK));

    EXPECT_CALL(mock_gpio, reset_pin(GPIO_NUM_5)).WillOnce(Return(ESP_OK));
    EXPECT_CALL(mock_gpio, config(testing::Field(&gpio_config_t::pin_bit_mask, 1ULL << GPIO_NUM_5)))
        .WillOnce(Return(ESP_OK));
    EXPECT_CALL(mock_gpio, set_level(GPIO_NUM_5, false)).WillOnce(Return(ESP_OK)); // inverted

    EXPECT_EQ(ESP_OK, pc1.init());
    EXPECT_EQ(ESP_OK, pc2.init());

    // Verify each instance is in the correct state
    EXPECT_FALSE(pc1.is_on());
    EXPECT_TRUE(pc2.is_on());

    // Independently turn on/off
    EXPECT_CALL(mock_gpio, set_level(GPIO_NUM_4, true)).WillOnce(Return(ESP_OK));
    EXPECT_CALL(mock_gpio, set_level(GPIO_NUM_5, true)).WillOnce(Return(ESP_OK)); // inverted OFF = physical HIGH

    EXPECT_EQ(ESP_OK, pc1.turn_on());
    EXPECT_EQ(ESP_OK, pc2.turn_off());

    EXPECT_TRUE(pc1.is_on());
    EXPECT_FALSE(pc2.is_on());

    // Deinit both
    EXPECT_CALL(mock_gpio, set_level(GPIO_NUM_4, 0)).WillOnce(Return(ESP_OK));
    EXPECT_CALL(mock_gpio, reset_pin(GPIO_NUM_4)).WillOnce(Return(ESP_OK));
    EXPECT_CALL(mock_gpio, set_level(GPIO_NUM_5, 0)).WillOnce(Return(ESP_OK));
    EXPECT_CALL(mock_gpio, reset_pin(GPIO_NUM_5)).WillOnce(Return(ESP_OK));

    EXPECT_EQ(ESP_OK, pc1.deinit());
    EXPECT_EQ(ESP_OK, pc2.deinit());
}

TEST_F(PowerControlTest, Sequence_ComplexScenario)
{
    PowerControl pc(mock_gpio, GPIO_NUM_4, false, false);

    // Init
    EXPECT_CALL(mock_gpio, reset_pin(GPIO_NUM_4)).WillOnce(Return(ESP_OK));
    EXPECT_CALL(mock_gpio, config(_)).WillOnce(Return(ESP_OK));
    EXPECT_CALL(mock_gpio, set_level(GPIO_NUM_4, false)).WillOnce(Return(ESP_OK));
    pc.init();

    // Multiple and complex sequences
    EXPECT_CALL(mock_gpio, set_level(GPIO_NUM_4, true)).Times(4).WillRepeatedly(Return(ESP_OK));
    EXPECT_CALL(mock_gpio, set_level(GPIO_NUM_4, false)).Times(3).WillRepeatedly(Return(ESP_OK));

    for (int i = 0; i < 3; i++) {
        pc.turn_on();
        pc.turn_off();
    }
    pc.turn_on(); // Leave on

    // Deinit - must force LOW
    EXPECT_CALL(mock_gpio, set_level(GPIO_NUM_4, 0)).WillOnce(Return(ESP_OK));
    EXPECT_CALL(mock_gpio, reset_pin(GPIO_NUM_4)).WillOnce(Return(ESP_OK));
    pc.deinit();
}

TEST_F(PowerControlTest, HeapAllocation_NoMemoryLeaks)
{
    // Valgrind test
    PowerControl *pc1 = new PowerControl(mock_gpio, GPIO_NUM_4, false, false);
    PowerControl *pc2 = new PowerControl(mock_gpio, GPIO_NUM_5, true, true);

    EXPECT_CALL(mock_gpio, reset_pin(_)).Times(2).WillRepeatedly(Return(ESP_OK));
    EXPECT_CALL(mock_gpio, config(_)).Times(2).WillRepeatedly(Return(ESP_OK));
    EXPECT_CALL(mock_gpio, set_level(_, _)).Times(2).WillRepeatedly(Return(ESP_OK));

    EXPECT_EQ(ESP_OK, pc1->init());
    EXPECT_EQ(ESP_OK, pc2->init());

    EXPECT_CALL(mock_gpio, set_level(_, 0)).Times(2).WillRepeatedly(Return(ESP_OK));
    EXPECT_CALL(mock_gpio, reset_pin(_)).Times(2).WillRepeatedly(Return(ESP_OK));

    EXPECT_EQ(ESP_OK, pc1->deinit());
    EXPECT_EQ(ESP_OK, pc2->deinit());

    delete pc1;
    delete pc2;

    // Run: valgrind --leak-check=full ./build/test_power_control.elf
}