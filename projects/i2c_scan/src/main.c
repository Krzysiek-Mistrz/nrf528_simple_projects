#include <stdint.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/printk.h>

static const struct device *i2c_dev = DEVICE_DT_GET(DT_ALIAS(arduino_i2c));

// @brief check if device responds 2 given addr
static bool addr_responds(const struct device *dev, uint8_t addr) {
    uint8_t dummy = 0;

    return i2c_write(dev, &dummy, 0, addr) == 0; // i2c_write writes 0B 2 given addr
}

int main(void) {
    if (!device_is_ready(i2c_dev)) {
        printk("i2c device not ready\n");
        
        return -1;
    }

    printk("scanning i2c bus...\n");

    for (uint8_t addr = 0x08; addr <= 0x77; addr++) {
        if (addr_responds(i2c_dev, addr)) {
            printk("found device at 0x%02x\n", addr);
        }
    }

    printk("scan done\n");

    return 0;
}