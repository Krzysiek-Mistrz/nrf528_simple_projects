#include <stdint.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include "ringbuf.h"
#include "frame_parser.h"

#define RX_STORAGE_SIZE 256 // how many B we can store in rx ring buffer
#define FRAME_PAYLOAD_MAX 64 // how many B we can store in a single frame payload
#define TX_FRAME_MAX (FRAME_PAYLOAD_MAX + 3) // max size of a frame, including start, length, payload, and checksum (so 3 + payload len)

// get device struct 4 uart dev 2 use in uart API calls
static const struct device *uart_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_console)); 
static uint8_t rx_storage[RX_STORAGE_SIZE];
static ringbuf_t rx_rb;
static uint8_t frame_buf[FRAME_PAYLOAD_MAX];
static frame_parser_t parser;


// @brief send frame over uart BbB w/ blocking
static void uart_send_frame(const uint8_t *payload, uint8_t len) {
        uint8_t out[TX_FRAME_MAX]; // buffer 2 hold the encoded frame
        const size_t n = frame_encode(payload, len, out, sizeof out);

        for (size_t i = 0; i < n; i++) {
                uart_poll_out(uart_dev, out[i]);
        }
}

// @brief callback called when frame is received
static void on_frame(const uint8_t *payload, uint8_t len, void *user) {
        ARG_UNUSED(user); // we don t use the user pointer...
        uart_send_frame(payload, len);
}

// @brief uart interr serv routine called when data received
static void uart_isr(const struct device *dev, void *user_data) {
        ARG_UNUSED(user_data);

        // uart_irq_update() ret true if interrupt is pending, 
        // uart_irq_rx_ready() ret true if data is available in the fifo
        while (uart_irq_update(dev) && uart_irq_rx_ready(dev)) {
                uint8_t byte;

                if (uart_fifo_read(dev, &byte, 1) == 1) {
                        ringbuf_push(&rx_rb, byte);
                }
        }
}

int main(void)
{
        if (!device_is_ready(uart_dev)) {
                return -1;
        }

        ringbuf_init(&rx_rb, rx_storage, sizeof rx_storage);
        frame_parser_init(&parser, frame_buf, sizeof frame_buf, on_frame, NULL);
        uart_irq_callback_user_data_set(uart_dev, uart_isr, NULL); // settin uart interrupt callback
        uart_irq_rx_enable(uart_dev); // enabliin uart interrupt

        for (;;) {
                uint8_t byte;

                while (ringbuf_pop(&rx_rb, &byte)) {
                        frame_parser_feed(&parser, byte);
                }

                k_msleep(1); // sleep 4 1 ms so we dont hog cpu...
        }

        return 0;
}
