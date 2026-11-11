#include "uart_bridge.h"
#include <string.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"

char latest_coords[64] = "0,0";

void uart_bridge_init(void) {
    const uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    
    uart_param_config(UART_NUM_STM, &uart_config);
    uart_set_pin(UART_NUM_STM, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM_STM, UART_BUF_SIZE * 2, 0, 0, NULL, 0);
}

void uart_send_command(const char *cmd_str) {
    if (cmd_str == NULL) return;
    uart_write_bytes(UART_NUM_STM, cmd_str, strlen(cmd_str));
    uart_write_bytes(UART_NUM_STM, "\n", 1);
}

void uart_listener_task(void *pvParameters) {
    char *line_buf = (char *)malloc(UART_BUF_SIZE);
    if (line_buf == NULL) {
        vTaskDelete(NULL);
        return;
    }

    int idx = 0;
    while (1) {
        uint8_t c;
        int len = uart_read_bytes(UART_NUM_STM, &c, 1, pdMS_TO_TICKS(20));
        
        if (len > 0){
            if (c == '\n' || c == '\r'){
                if (idx > 0){
                    line_buf[idx] = '\0';
                    if (strncmp(line_buf, "POS:", 4) == 0) snprintf(latest_coords, sizeof(latest_coords), "%s", line_buf + 4);
                    idx = 0;
                }
            }
            else{
                if (idx < UART_BUF_SIZE - 1) line_buf[idx++] = c;
            }
        }
    }
    
    free(line_buf);
    vTaskDelete(NULL);
}