#include <stddef.h>
#include <stdint.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/sys/printk.h>

static const struct spi_dt_spec spi = SPI_DT_SPEC_GET(DT_NODELABEL(loopback_dev),
                                                        SPI_WORD_SET(8) | SPI_TRANSFER_MSB, 0);

int main(void) {
    if (!spi_is_ready_dt(&spi)) {
        printk("spi device not ready\n");

        return -1;
    }

    // tables 2 sending and receiving
    const uint8_t tx_buf[4] = {0xDE, 0xAD, 0xBE, 0xEF};
    uint8_t rx_buf[4] = {0};
    // spi_buf & spi_buf_set struct 4 tx and rx
    const struct spi_buf tx_spi_buf = {
        .buf = (void *)tx_buf, 
        .len = sizeof tx_buf
    };
    // buf_set 4 packing buffers
    const struct spi_buf_set tx_set = {
        .buffers = &tx_spi_buf, 
        .count = 1
    };
    const struct spi_buf rx_spi_buf = {
        .buf = rx_buf, 
        .len = sizeof rx_buf
    };
    const struct spi_buf_set rx_set = {
        .buffers = &rx_spi_buf, .count = 1
    };

    // send & receive data
    const int rc = spi_transceive_dt(&spi, &tx_set, &rx_set);

    if (rc != 0) {
        printk("spi_transceive_dt failed: %d\n", rc);

        return rc;
    }

    bool match = true;

    // print tx and rx data & check if they match
    for (size_t i = 0; i < sizeof tx_buf; i++) {
        printk("tx[%zu]=0x%02x rx[%zu]=0x%02x\n", i, tx_buf[i], i, rx_buf[i]);
        
        match = match && (tx_buf[i] == rx_buf[i]);
    }

    printk(match ? "loopback ok\n" : "loopback missmatch\n");
    
    return 0;
}