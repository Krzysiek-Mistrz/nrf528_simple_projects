#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

#define BLINK_PERIOD_MS 500

// gettin dev struct 4 led0 4m devicetree...
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

int main(void)
{
    if (!device_is_ready(led.port)) {
        return -1;
    }

    // cponfig led pin as output w/ init val of 0
    gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);

    for (;;) {
        gpio_pin_toggle_dt(&led); // togling -> on / of with time interval of BLINK_PERIOD_MS
        k_msleep(BLINK_PERIOD_MS);
    }

    return 0;
}
