#include <button.hpp>

void Button::isr_handler(void* args)
{
    Button* button = reinterpret_cast<Button*>(args);
    button->handle_interrupt();
}

void Button::handle_interrupt()
{
    int64_t currentTime = esp_timer_get_time();
    if (currentTime - last_interrupt_time < DEBOUNCE_TIME) return;

    last_interrupt_time = currentTime;
    
    if (queue)
    {
        int pin = (int)gpio_pin;
        xQueueSendFromISR(queue, &pin, NULL);
    }
}

Button::Button(const gpio_num_t gpio_pin) : gpio_pin(gpio_pin)
{
    gpio_reset_pin(gpio_pin);
    gpio_pullup_en(gpio_pin);
    gpio_set_direction(gpio_pin, GPIO_MODE_INPUT);
    gpio_set_intr_type(gpio_pin, GPIO_INTR_ANYEDGE);

    gpio_isr_handler_add(gpio_pin, isr_handler, (void*)this);
}

void Button::attach_queue(QueueHandle_t queue)
{
    this->queue = queue;
}

bool Button::is_pressed() const
{
    return (gpio_get_level(gpio_pin) == 0);
}