#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <driver/gpio.h>
#include <esp_adc/adc_oneshot.h>
#include <esp_err.h>
#include <esp_timer.h>

class Ky038
{
  private:
    static const int64_t DEBOUNCE_TIME = 5000; // 5ms

    gpio_num_t digital_gpio_pin;
    gpio_num_t analog_gpio_pin;
    QueueHandle_t queue = nullptr;
    int64_t last_interrupt_time = 0;
    adc_oneshot_unit_handle_t adc_handle = nullptr;
    adc_channel_t channel;

    static void isr_handler(void* args);
    void handle_interrupt();

  public:
    /**
     * @brief Creates a Ky038
     * @details This class uses the digital and analog outpin of the Ky038 to provide
     * utility functionality
     * @param digital_gpio_pin The digital output
     * @param analog_gpio_pin The analog output
     */
    Ky038(const gpio_num_t digital_gpio_pin, const gpio_num_t analog_gpio_pin, adc_oneshot_unit_handle_t adc_handle);

    /**
     * @brief Destroys a Ky038
     * @details Because the Ky038 uses the ADC
     */
    ~Ky038();

    /**
     * @brief Attaches the Ky038 to a queue
     * @details The Ky038 sends events to the queue when it detects a sound
     * It debounces the Ky038 internally, so it already sends good signals
     * @param queue The queue, to which the Ky038 should attach to
     */
    void attach_queue(QueueHandle_t queue);

    /**
     * @brief Check if the Ky038 detected a sound
     * @return `true` if the Ky038 detected a sound, `false` otherwise
     */
    bool is_sound_detected() const;

    /**
     * @brief Measures the current sound amplitude (peak-to-peak)
     * @param samples Number of ADC samples to collect (default: 128)
     * @return Amplitude (difference between max and min sample)
     */
    int measure_amplitude(size_t samples = 128);

    /**
     * @brief Scales a 12-bit ADC value (0–4095) into an 8-bit range (0–255)
     */
    static uint8_t scale_to_byte(int adc_value);

    /**
     * @brief Scales an 8-bit value (0–255) back into 12-bit ADC range (0–4095)
     */
    static int scale_to_adc(uint8_t byte_value);
};