#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/sys/printk.h>

static const struct adc_dt_spec adc = ADC_DT_SPEC_GET_BY_IDX(DT_PATH(zephyr_user), 0);

int main(void) {
    if (!adc_is_ready_dt(&adc)) {
        printk("adc device not ready\n");

        return -1;
    }

    adc_channel_setup_dt(&adc);
    int16_t sample_buf;
    
    // sequence is contains buff & buff size 4 the adc read
    struct adc_sequence sequence = { 
        .buffer = &sample_buf, // writing adc read val 2 sample_buf variable type shit...
        .buffer_size = sizeof sample_buf,
    };

    // init sequence with adc device
    adc_sequence_init_dt(&adc, &sequence);

    for (;;) {
        const int rc = adc_read_dt(&adc, &sequence);

        if (rc == 0) {
            int32_t mv = sample_buf;
            adc_raw_to_millivolts_dt(&adc, &mv); // stores the converted value in mv variable
            printk("raw val = %d, mv = %d\n", sample_buf, mv);
        } else {
            printk("adc_read_dt failed: %d\n", rc);
        }

        k_msleep(200);
    }

    return 0;
}