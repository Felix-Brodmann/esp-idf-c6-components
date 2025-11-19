#include <ky-038.hpp>

void Ky038::isr_handler(void* args)
{
    Ky038* ky_038 = reinterpret_cast<Ky038*>(args);
    ky_038->handle_interrupt();
}

void Ky038::handle_interrupt()
{
    int64_t currentTime = esp_timer_get_time();
    if (currentTime - last_interrupt_time < DEBOUNCE_TIME) return;

    last_interrupt_time = currentTime;
    
    if (queue)
    {
        int pin = (int)digital_gpio_pin;
        xQueueSendFromISR(queue, &pin, NULL);
    }
}

Ky038:: Ky038(const gpio_num_t digital_gpio_pin, const gpio_num_t analog_gpio_pin, adc_oneshot_unit_handle_t adc_handle) : 
    digital_gpio_pin(digital_gpio_pin),
    analog_gpio_pin(analog_gpio_pin),
    adc_handle(adc_handle)
{
    gpio_reset_pin(digital_gpio_pin);
    gpio_pulldown_en(digital_gpio_pin);
    gpio_set_direction(digital_gpio_pin, GPIO_MODE_INPUT);
    gpio_set_intr_type(digital_gpio_pin, GPIO_INTR_POSEDGE);

    gpio_isr_handler_add(digital_gpio_pin, isr_handler, (void*)this);

    switch(analog_gpio_pin)
    {
        case GPIO_NUM_0: channel = ADC_CHANNEL_0; break;
        case GPIO_NUM_1: channel = ADC_CHANNEL_1; break;
        case GPIO_NUM_2: channel = ADC_CHANNEL_2; break;
        case GPIO_NUM_3: channel = ADC_CHANNEL_3; break;
        case GPIO_NUM_4: channel = ADC_CHANNEL_4; break;
        case GPIO_NUM_5: channel = ADC_CHANNEL_5; break;
        default: channel = ADC_CHANNEL_6; break;
    }

    adc_oneshot_chan_cfg_t chan_config =
    {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_12
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, channel, &chan_config));
}

Ky038::~Ky038()
{
    if (adc_handle)
        adc_oneshot_del_unit(adc_handle);
}

void Ky038::attach_queue(QueueHandle_t queue)
{
    this->queue = queue;
}

bool Ky038::is_sound_detected() const
{
    return (gpio_get_level(digital_gpio_pin) == 1);
}

int Ky038::measure_amplitude(size_t samples)
{
    int minVal = 4095;
    int maxVal = 0;

    for (size_t i = 0; i < samples; ++i)
    {
        int value = 0;
        esp_err_t err = adc_oneshot_read(adc_handle, channel, &value);
        if (err == ESP_OK)
        {
            if (value < minVal) minVal = value;
            if (value > maxVal) maxVal = value;
        }
        esp_rom_delay_us(100);
    }

    return maxVal - minVal;
}

uint8_t Ky038::scale_to_byte(int adc_value)
{
    if (adc_value < 0) adc_value = 0;
    if (adc_value > 4095) adc_value = 4095;
    return static_cast<uint8_t>((adc_value * 255 + 2047) / 4095);
}

int Ky038::scale_to_adc(uint8_t byte_value)
{
    return static_cast<int>((byte_value * 4095 + 127) / 255);
}