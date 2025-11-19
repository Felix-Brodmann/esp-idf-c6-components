#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <driver/gpio.h>
#include <esp_timer.h>

class Button
{
  private:
    static const int64_t DEBOUNCE_TIME = 20000; // 20ms
    
    gpio_num_t gpio_pin;
    QueueHandle_t queue = nullptr;
    int64_t last_interrupt_time = 0;

    static void isr_handler(void* args);
    void handle_interrupt();

  public:
    /**
     * @brief Creates a Button
     * @details This class uses a pullup on the provided GPIO pin, so for it to work, 
     * the button schould connect to GND if pressed.
     * @param gpio_pin The GPIO pin connected to a button
     */
    Button(const gpio_num_t gpio_pin);

    /**
     * @brief Attaches the button to a queue
     * @details The button sends events to the queue when it is pressed or released.
     * It debounces the button internally, so it already sends good signals
     * @param queue The queue, to which the button should attach to
     */
    void attach_queue(QueueHandle_t queue);
    
    /**
     * @brief Check if the button is pressed
     * @return `true` if the button is pressed, `false` otherwise
     */
    bool is_pressed() const;
};