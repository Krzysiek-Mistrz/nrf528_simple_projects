#include <stdint.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>

#define PERIOD_NS 1000000 // 1 ms is the period of the PWM signal
#define STEP_NS 10000 // 10 us is the step size 4 which we itarate pulse ns...

static const struct pwm_dt_spec pwm_led = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led0));

int main(void) {
    if (!device_is_ready(pwm_led.dev)) {
        return -1;
    }

    uint32_t pulse_ns = 0; // pulse_ns is pulse width in ns (voltage is given 2 diaode)
    int32_t step = STEP_NS;

    for (;;) {
        pwm_set_dt(&pwm_led, PERIOD_NS, pulse_ns);

        pulse_ns += (uint32_t)step;

        if (pulse_ns >= PERIOD_NS || pulse_ns == 0U) {
            step = -step;
        }

        k_msleep(10);
    }
    
    return 0;
}