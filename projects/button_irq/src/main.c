#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include "debounce.h"

#define SAMPLE_PERIOD_MS 5
#define STABLE_SAMPLES 3

static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static struct gpio_callback button_cb_data;
static volatile bool edge_seen;
static debounce_t debouncer;
static struct k_timer sample_timer;

// @brief callback called when button state changed
static void button_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins) {
        ARG_UNUSED(dev);
        ARG_UNUSED(cb);
        ARG_UNUSED(pins);

        edge_seen = true;
}

// @brief callback called when sample timer expired
static void sample_timer_handler(struct k_timer *timer) {
        ARG_UNUSED(timer);

        const bool raw_pressed = gpio_pin_get_dt(&button) > 0;
        const debounce_event_t ev = debounce_update(&debouncer, raw_pressed);

        if (ev == DEBOUNCE_EVT_PRESSED) {
                gpio_pin_set_dt(&led, 1);
        } else if (ev == DEBOUNCE_EVT_RELEASED) {
                gpio_pin_set_dt(&led, 0);
        }
}

int main(void)
{
        if (!device_is_ready(button.port) || !device_is_ready(led.port)) {
                return -1;
        }

        gpio_pin_configure_dt(&button, GPIO_INPUT);
        gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
        gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_BOTH); // config interrupt on both edges
        gpio_init_callback(&button_cb_data, button_isr, BIT(button.pin)); // init callback 4 button pin w/ button_isr callback, BIT returns 1 << button.pin
        gpio_add_callback(button.port, &button_cb_data); // adding callback 2 button
        debounce_init(&debouncer, STABLE_SAMPLES);
        k_timer_init(&sample_timer, sample_timer_handler, NULL); // init sample timer w/ sample_timer_handler callback int happens after each period -> 5sec
        k_timer_start(&sample_timer, K_MSEC(SAMPLE_PERIOD_MS), K_MSEC(SAMPLE_PERIOD_MS)); // start sample timer w/ period SAMPLE_PERIOD_MS K_MSEC converts ms 2 kernel ticks

        for (;;) {
                k_sleep(K_FOREVER);
        }

        return 0;
}