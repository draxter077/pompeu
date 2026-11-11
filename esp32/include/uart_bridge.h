#ifndef UART_BRIDGE_H
#define UART_BRIDGE_H

#include "driver/uart.h"

#define UART_NUM_STM     UART_NUM_2
#define UART_TX_PIN      (17)
#define UART_RX_PIN      (16)
#define UART_BUF_SIZE    (1024)
#define UART_BAUD_RATE   (115200)

extern char latest_coords[64];

void uart_bridge_init(void);
void uart_send_command(const char *cmd_str);
void uart_listener_task(void *pvParameters);

#endif