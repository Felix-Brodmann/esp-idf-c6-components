#pragma once

#include <driver/gpio.h>
#include <dht.h>

class Dht11
{
  private:
    gpio_num_t gpio_pin;
    float temperature;
    float humidity;

  public:
    /**
     * @brief Creates a DHT11
     * @details This class allows reading temperature and humidity from a DHT11 sensor.
     * @param gpio_pin The GPIO pin connected to the DHT11 data pin
     */
    Dht11(const gpio_num_t gpio_pin);

    /**
     * @brief Reads data from the DHT11 sensor
     * @return ESP_OK on success, or an error code on failure
     */
    esp_err_t read();

    /**
     * @brief Gets the last read temperature
     * @return The temperature in Celsius
     */
    float get_temperature() const;

    /**
     * @brief Gets the last read humidity
     * @return The humidity in percentage
     */
    float get_humidity() const;
};