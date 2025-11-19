#pragma once

#include <driver/gpio.h>
#include <LovyanGFX.hpp>
#include <joystick.hpp>

class Ili9225 : public lgfx::LGFX_Device
{
  private:
    lgfx::Panel_ILI9225 panel_instance;
    lgfx::Bus_SPI bus_instance;

  public:
    /**
     * @brief Creates an ILI9225 display driver
     * @details This class allows controlling an ILI9225 based display via SPI.
     * @param clk_gpio_pin The GPIO pin connected to the SPI clock (SCK)
     * @param sda_gpio_pin The GPIO pin connected to the SPI data (MOSI)
     * @param rs_gpio_pin The GPIO pin connected to the RS (DC) pin of the display
     * @param rst_gpio_pin The GPIO pin connected to the reset (RST) pin of the display
     * @param cs_gpio_pin The GPIO pin connected to the chip select (CS) pin of the display
     * @param width The width of the display in pixels
     * @param height The height of the display in pixels
     */
    Ili9225(const gpio_num_t clk_gpio_pin,
            const gpio_num_t sda_gpio_pin,
            const gpio_num_t rs_gpio_pin,
            const gpio_num_t rst_gpio_pin,
            const gpio_num_t cs_gpio_pin,
            const uint16_t width,
            const uint16_t height);

    /**
     * @brief Draws a joystick representation on the display
     * @details This method draws a visual representation of the joystick state on the display.
     * @param joystick The Joystick object containing the current state
     * @param center_x The x coordinate of the center of the joystick area
     * @param center_y The y coordinate of the center of the joystick area
     * @param area_size The size of the joystick area (width and height)
     * @param dot_radius The radius of the joystick position dot
     */
    void draw_joystick(const Joystick& joystick, 
  					   const int32_t center_x, 
					   const int32_t center_y, 
					   const int area_size,
					   const int32_t dot_radius);
};