#include "uart_bridge.h"
#include <string.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"

void uart_bridge_init(void){ // Função para inicialização do recurso
    const uart_config_t uart_config = { // Configuração do UART usando biblioteca do ESP
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    
    uart_param_config(UART_NUM_STM, &uart_config);
    uart_set_pin(UART_NUM_STM, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE); // Define pinos UART
    uart_driver_install(UART_NUM_STM, UART_BUF_SIZE * 2, 0, 0, NULL, 0); // Alocação final de recursos
}

void uart_send_command(const char *cmd_str){ // Função para enviar comunicação
    uart_write_bytes(UART_NUM_STM, cmd_str, strlen(cmd_str)); // Envia os bytes da comunicação
    uart_write_bytes(UART_NUM_STM, "\n", 1); // Envia byte de finalização
}