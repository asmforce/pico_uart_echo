#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/uart.h"

#define BAUD_RATE 9600

#define UART_TX_PIN 0
#define UART_RX_PIN 1

#define PACKET_SIZE 1
#define RX_TIMEOUT_MS 1000

static volatile uint32_t rx_count = 0;
static volatile uint32_t tx_count = 0;

static uint32_t reported_rx_count = 0;
static uint32_t reported_tx_count = 0;

void monitor_traffic() {
    multicore_fifo_push_blocking(0);

    while (true) {
        sleep_ms(500);

        if (reported_rx_count < rx_count || reported_tx_count < tx_count) {
            reported_rx_count = rx_count;
            reported_tx_count = tx_count;

            printf("rx (%u) tx (%u)\n", reported_rx_count, reported_tx_count);
        }
    }
}

int main() {
    stdio_usb_init();

    uart_init(uart0, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    multicore_launch_core1(monitor_traffic);
    multicore_fifo_pop_blocking();

    uint32_t count = 0;
    uint8_t packet[PACKET_SIZE];

    while (true) {
        while (count < PACKET_SIZE && uart_is_readable_within_us(uart0, RX_TIMEOUT_MS * 1000)) {
            packet[count++] = uart_getc(uart0);
            rx_count++;
        }

        for (uint32_t index = 0; index < count; index++) {
            uart_putc_raw(uart0, packet[index]);
            tx_count++;
        }

        count = 0;
    }
}
