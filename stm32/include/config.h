#ifndef CONFIG_H
#define CONFIG_H

#include "stm32f4xx_hal.h"

#define UART_TYPE             USART2 // Definições das variáveis UART
#define UART_BAUD             115200
#define UART_BUFFER_SIZE      1024
#define UART_TX               GPIO_PIN_4
#define UART_RX               GPIO_PIN_5

#define OLED_SCK              GPIO_PIN_6 // Definições do OLED
#define OLED_SDA              GPIO_PIN_7
#define OLED_I2C_PORT         I2C1
#define OLED_I2C_SPEED        40000
#define DISPLAY_MAX_LEN       20
#define OLED_I2C_ADDR         (0x3C << 1)

extern UART_HandleTypeDef huart2; // Handle do UART2
extern int newCommandSaved; // Variável para dizer se uma nova comunicação foi recebida
extern char cmdReceived[50]; // Conteúdo da comunicação recebida
extern uint8_t cmdByteReceived; // Byte recebido
extern int cmdReceivedIndex; // Posição na string de comando recebido
void UART2_Init(void); // Inicializa e configura a comunicação UART
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart); // Callback de recebimento de comunicação UART
void USART2_IRQHandler(void); // Tratamento da interrupção pelo UART

void SystemClock_Config(void); // Funções básicas
void MX_GPIO_Init(void);

#endif