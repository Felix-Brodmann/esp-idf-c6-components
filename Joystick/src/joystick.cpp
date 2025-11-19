#include <joystick.hpp>

Joystick::Joystick(Button& button, const gpio_num_t x_gpio_pin, const gpio_num_t y_gpio_pin, adc_oneshot_unit_handle_t adc_handle) :
    button(button),
    x_gpio_pin(x_gpio_pin),
    y_gpio_pin(y_gpio_pin),
    adc_handle(adc_handle)
{
    switch(x_gpio_pin)
    {
        case GPIO_NUM_0: x_channel = ADC_CHANNEL_0; break;
        case GPIO_NUM_1: x_channel = ADC_CHANNEL_1; break;
        case GPIO_NUM_2: x_channel = ADC_CHANNEL_2; break;
        case GPIO_NUM_3: x_channel = ADC_CHANNEL_3; break;
        case GPIO_NUM_4: x_channel = ADC_CHANNEL_4; break;
        case GPIO_NUM_5: x_channel = ADC_CHANNEL_5; break;
        default: x_channel = ADC_CHANNEL_6; break;
    }
    switch(y_gpio_pin)
    {
        case GPIO_NUM_0: y_channel = ADC_CHANNEL_0; break;
        case GPIO_NUM_1: y_channel = ADC_CHANNEL_1; break;
        case GPIO_NUM_2: y_channel = ADC_CHANNEL_2; break;
        case GPIO_NUM_3: y_channel = ADC_CHANNEL_3; break;
        case GPIO_NUM_4: y_channel = ADC_CHANNEL_4; break;
        case GPIO_NUM_5: y_channel = ADC_CHANNEL_5; break;
        default: y_channel = ADC_CHANNEL_6; break;
    }
    adc_oneshot_chan_cfg_t chan_config =
    {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_12
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, x_channel, &chan_config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, y_channel, &chan_config));
}

bool Joystick::is_pressed() const
{
    return button.is_pressed();
}

void Joystick::read()
{
    ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, x_channel, &x_value));
    ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, y_channel, &y_value));
}

int Joystick::get_x() const
{
    int value = 0;
    if (x_value < 1681)
    {
        value = (x_value - 1681) * 90 / (1681 - 22);
    }
    else
    {
        value = (x_value - 1681) * 90 / (3305 - 1681);
    }
    value = value < -90 ? -90 : value;
    value = value > 90 ? 90 : value;
    return value;
}

int Joystick::get_y() const
{
    int value = 0;
    if (y_value < 1640)
    {
        value = (y_value - 1640) * 90 / (1640 - 22);
    }
    else
    {
        value = (y_value - 1640) * 90 / (3305 - 1640);
    }
    value = value < -90 ? -90 : value;
    value = value > 90 ? 90 : value;
    return value;
}

int Joystick::get_x_mv() const
{
    return static_cast<int>(x_value * 3300.0 / 4095.0);
}

int Joystick::get_y_mv() const
{
    return static_cast<int>(y_value * 3300.0 / 4095.0);
}