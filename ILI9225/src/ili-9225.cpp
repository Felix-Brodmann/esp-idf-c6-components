#include <ili-9225.hpp>
#include <stdio.h>

Ili9225::Ili9225(const gpio_num_t clk_gpio_pin,
                 const gpio_num_t sda_gpio_pin,
                 const gpio_num_t rs_gpio_pin,
                 const gpio_num_t rst_gpio_pin,
                 const gpio_num_t cs_gpio_pin,
                 const uint16_t width,
                 const uint16_t height)
{
    lgfx::Bus_SPI::config_t bus_config = bus_instance.config();
    bus_config.pin_sclk = clk_gpio_pin;
    bus_config.pin_mosi = sda_gpio_pin;
    bus_config.pin_dc = rs_gpio_pin;
    bus_instance.config(bus_config);
    panel_instance.setBus(&bus_instance);

    lgfx::Panel_Device::config_t panel_config = panel_instance.config();
    panel_config.pin_cs = cs_gpio_pin;
    panel_config.pin_rst = rst_gpio_pin;
    panel_config.memory_width = width;
    panel_config.memory_height = height;
    panel_config.panel_width = width;
    panel_config.panel_height = height;
    panel_instance.config(panel_config);
    setPanel(&panel_instance);
}

void Ili9225::draw_joystick(const Joystick& joystick, 
                   const int32_t center_x, 
                   const int32_t center_y, 
                   const int area_size,
                   const int32_t dot_radius)
{
    // x and y are swapped because of the display rotation
    // adjust sign and swap to match your display coordinates
    int x = (joystick.get_y() * (area_size / 2) / area_size) + center_x;
    int y = -(joystick.get_x() * (area_size / 2) / area_size) + center_y;

    this->drawRect(center_x - (area_size / 2), center_y - (area_size / 2), area_size, area_size, TFT_WHITE);
    this->fillCircle(x, y, dot_radius, TFT_RED);
}