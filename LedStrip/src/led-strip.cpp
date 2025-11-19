#include <led-strip.hpp>

LedStrip::LedStrip(const gpio_num_t gpio_pin, 
                    const uint32_t max_leds, 
                    const led_model_t model, 
                    const led_color_component_format_t color_order, 
                    const bool invert_output)
{
    led_strip_config_t strip_config = 
    {
        .strip_gpio_num = gpio_pin,
        .max_leds = max_leds,
        .led_model = model,
        .color_component_format = color_order,
        .flags = {.invert_out = invert_output} 
    };
    led_strip_rmt_config_t rmt_config =
    {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 10 * 1000 * 1000,
        .mem_block_symbols = 64,
        .flags = {.with_dma = false}
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_handle));
    ESP_ERROR_CHECK(led_strip_clear(led_handle));
}

void LedStrip::set_color(const uint32_t led, const RGB color)
{
    current_color_configuration[led] = color;
    if (on)
    {
        led_strip_set_pixel(led_handle, 
                            led,
                            color.red, 
                            color.green, 
                            color.blue);
        led_strip_refresh(led_handle);
    }
}

void LedStrip::turn_off()
{
    on = false;
    for (auto &[led, _] : current_color_configuration)
    {
        led_strip_set_pixel(led_handle, led, 0, 0, 0);
    }
    led_strip_refresh(led_handle);
}

void LedStrip::turn_on()
{
    on = true;
    for (auto &[led, color] : current_color_configuration)
    {
        led_strip_set_pixel(led_handle, 
                            led,
                            color.red, 
                            color.green, 
                            color.blue);
    }
    led_strip_refresh(led_handle);
}

void LedStrip::turn_on_for_s(uint32_t seconds)
{
    if (is_running) return;

    is_running = true;

    struct TaskParamaters {
        LedStrip* self;
        uint32_t duration;
    };

    auto* params = new TaskParamaters{this, seconds};

    xTaskCreate(
        [](void* arg)
        {
            auto* p = static_cast<TaskParamaters*>(arg);
            p->self->turn_on();
            vTaskDelay(pdMS_TO_TICKS(p->duration * 1000));
            p->self->turn_off();
            p->self->is_running = false;
            delete p;
            vTaskDelete(nullptr);
        },
        "LedStripAutoOff",
        2048,
        params,
        5,
        nullptr
    );
}