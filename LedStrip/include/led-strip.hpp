#pragma once

#include <map>
#include <driver/gpio.h>
#include <led_strip.h>
#include <esp_timer.h>

/**
 * @brief A RGB struct whcih stores values for red, green and blue
 */
struct RGB
{
    uint32_t red, green, blue;
    RGB(uint32_t red, uint32_t green, uint32_t blue) : red(red), green(green), blue(blue) {}
    RGB() : red(0), green(0), blue(0) {}
};

const RGB RED(50, 0, 0);
const RGB YELLOW(50, 50, 0);
const RGB GREEN(0, 50, 0);
const RGB BLUE(0, 0, 50);

class LedStrip
{
  private:
    led_strip_handle_t led_handle;
    std::map<uint32_t, RGB> current_color_configuration;
    bool on = false;
    bool is_running = false;

  public:
    /**
     * @brief Creates a LedStrip
     * @param gpio_pin The pin connected to a LedStrip
     * @param max_leds The number of Led's on the LedStrip `default: 1`
     * @param model The Led model `default: LED_MODEL_WS2812`
     * @param color_order The Led color order `default: LED_STRIP_COLOR_COMPONENT_FMT_GRB`
     * @param invert_output If the output should be inverted `default: false`
     */
    LedStrip(const gpio_num_t gpio_pin, 
                const uint32_t max_leds = 1, 
                const led_model_t model = LED_MODEL_WS2812, 
                const led_color_component_format_t color_order = LED_STRIP_COLOR_COMPONENT_FMT_GRB, 
                const bool invert_output = false);

    /**
     * @brief Sets the color of the provided Led
     * @details This function just sets the color in memory, in order to see
     * it need to call turnOn()
     * @param led The Led on the LedSrip to set the color
     * @param color The color to set
     */
    void set_color(const uint32_t led, const RGB color);

    /**
     * @brief Turns the LedStrip off
     */
    void turn_off();

    /**
     * @brief Turns the LedStrip on
     */
    void turn_on();

    /**
     * @brief Turns the LedStrip on for the provided duration in seconds
     * @details This funktion uses a xTaskCreate to turn on the LedStrip non blocking.
     * If it is running, every new try to call it will be ignored
     * @param seconds The duration in seconds
     */
    void turn_on_for_s(uint32_t seconds);

};