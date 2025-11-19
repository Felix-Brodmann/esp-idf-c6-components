#include <dht-11.hpp>

Dht11::Dht11(const gpio_num_t gpio_pin) : gpio_pin(gpio_pin)
{
    gpio_reset_pin(gpio_pin);
}

esp_err_t Dht11::read()
{
    gpio_reset_pin(gpio_pin);
    float humidity = 0.0f;
    float temperature = 0.0f;

    esp_err_t error = dht_read_float_data(DHT_TYPE_DHT11, gpio_pin, &humidity, &temperature);
    if (error == ESP_OK)
    {
        this->humidity = humidity;
        this->temperature = temperature;
    }
    return error;
}

float Dht11::get_temperature() const
{
    return temperature;
}

float Dht11::get_humidity() const
{
    return humidity;
}