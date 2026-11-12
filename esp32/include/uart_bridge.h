#ifndef UART_BRIDGE_H
#define UART_BRIDGE_H

#include "driver/uart.h"

// Definição dos pinos dedicados e dos parâmetros da transmissão
#define UART_NUM_STM     UART_NUM_2
#define UART_TX_PIN      (17)
#define UART_RX_PIN      (16)
#define UART_BUF_SIZE    (1024)
#define UART_BAUD_RATE   (115200)

void uart_bridge_init(void); // Função para iniciar a UART
void uart_send_command(const char *cmd_str); // Função para enviar comunicação

#endif