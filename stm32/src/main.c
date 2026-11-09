/* USER CODE BEGIN Includes */
#include "config.h"
#include <string.h>
#include <stdio.h>
/* USER CODE END Includes */

// Variáveis para leitura do buffer UART vindo do ESP32
uint8_t rx_byte;
char rx_buffer[128];
uint8_t rx_idx = 0;

int main(void) {
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/
  HAL_Init();
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_USART2_UART_Init();

  /* USER CODE BEGIN 2 */
  // Inicia os PWMs gerados pelo CubeMX
  Custom_Peripherals_Start();
  
  // Inicia a recepção por interrupção da UART do ESP32
  HAL_UART_Receive_IT(&huart2, &rx_byte, 1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
      // Aqui entra a sua máquina de estados de obstáculo e tratamento dos comandos recebidos
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/* Callback de recepção UART (disparado quando o ESP32 manda um comando) */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        if (rx_byte == '\n' || rx_byte == '\r'){
            rx_buffer[rx_idx] = '\0';

            // Tratamento dos comandos enviados pelo site do ESP32
            if (strcmp(rx_buffer, "CMD:F") == 0) Robot_Move_Forward();
            else if (strcmp(rx_buffer, "CMD:B") == 0) Robot_Move_Backward();
            else if (strcmp(rx_buffer, "CMD:L") == 0) Robot_Turn_Left();
            else if (strcmp(rx_buffer, "CMD:R") == 0) Robot_Turn_Right();
            else if (strcmp(rx_buffer, "CMD:SPIN_L") == 0) Robot_Spin_Left();
            else if (strcmp(rx_buffer, "CMD:SPIN_R") == 0) Robot_Spin_Right();
            else if (strcmp(rx_buffer, "CMD:S") == 0) Robot_Stop();

            rx_idx = 0;
        }
        else{
            if (rx_idx < sizeof(rx_buffer) - 1) rx_buffer[rx_idx++] = rx_byte;
        }
        HAL_UART_Receive_IT(&huart2, &rx_byte, 1);
    }
}