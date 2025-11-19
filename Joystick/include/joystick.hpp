#pragma once

#include <button.hpp>
#include <driver/gpio.h>
#include <esp_adc/adc_oneshot.h>

class Joystick
{
  private:
    Button& button;
    gpio_num_t x_gpio_pin;
    gpio_num_t y_gpio_pin;
    adc_oneshot_unit_handle_t adc_handle = nullptr;
    adc_channel_t x_channel;
    adc_channel_t y_channel;
    int x_value = 0;
    int y_value = 0;

  public:
    /**
     * @brief Creates a Joystick
     * @param button A Button object representing the joystick's press button
     * @param x_gpio_pin The GPIO pin connected to the joystick's X axis
     * @param y_gpio_pin The GPIO pin connected to the joystick's Y axis
     * @param adc_handle The ADC oneshot handle for reading analog values
     */
    Joystick(Button& button, const gpio_num_t x_gpio_pin, const gpio_num_t y_gpio_pin, adc_oneshot_unit_handle_t adc_handle);
    
    /**
     * @brief Check if the joystick button is pressed
     * @return `true` if the button is pressed, `false` otherwise
     */
    bool is_pressed() const;

    /**
     * @brief Read the current X and Y analog values from the joystick.
     */
    void read();

    /**
     * @brief Get the latest X value
     * @return The X value in the range of -90 to 90
     */
    int get_x() const;

    /**
     * @brief Get the latest Y value
     * @return The Y value in the range of -90 to 90
     */
    int get_y() const;

    /**
     * @brief Get the latest X value in millivolts
     * @return The X value in millivolts
     */
    int get_x_mv() const;

    /**
     * @brief Get the latest Y value in millivolts
     * @return The Y value in millivolts
     */
    int get_y_mv() const;
};