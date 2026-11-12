#include "config.h"

UART_HandleTypeDef huart2; // Seção de comunicação UART
char cmdReceived[50];
uint8_t cmdByteReceived = 0;
int cmdReceivedIndex = 0;
int newCommandSaved = 0;
void UART2_Init(void){
  __HAL_RCC_USART2_CLK_ENABLE(); // Habilita clock UART

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  GPIO_InitStruct.Pin = UART_TX; // Configuração do pino TX
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = UART_RX; // Configuração do pino RX
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  huart2.Instance = UART_TYPE; // Configuração da comunicação UART
  huart2.Init.BaudRate = UART_BAUD;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;

  HAL_UART_Init(&huart2);

  HAL_NVIC_SetPriority(USART2_IRQn, 0, 0); // Configuração da interrupção externa
  HAL_NVIC_EnableIRQ(USART2_IRQn);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
  if(cmdByteReceived == '\n' || cmdReceivedIndex >= (UART_BUFFER_SIZE - 1)){ // Avalia se o byte recebido é de finalização
    cmdReceived[cmdReceivedIndex] = '\0'; // Fecha a string
    newCommandSaved = 1; // Avisa sobre a conclusão
    cmdReceivedIndex = 0; // Zera o índice para a próxima comunicação
  }
  else if (cmdByteReceived != '/r'){ // Caso não seja um caractere vazio, comum perto da finalização
    cmdReceived[cmdReceivedIndex] = (char) cmdByteReceived;
    cmdReceivedIndex++;
  }

  HAL_UART_Receive_IT(huart, &cmdByteReceived, 1); // Restarta a interrupção
}

void USART2_IRQHandler(void){
  HAL_UART_IRQHandler(&huart2); // Trata a interrupção
}

void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configura o regulador de tensão interno do chip */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Inicializa os Osciladores (Utilizando o HSI - Oscilador Interno de 16MHz) */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  // Configuração do PLL para multiplicar 16MHz até atingir 84MHz
  // Fórmula: (16MHz / PLLM) * PLLN / PLLP = 84MHz -> (16 / 16) * 336 / 4 = 84 MHz
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    while(1); // Erro ao configurar oscilador
  }

  /** Inicializa os clocks da CPU, barramentos AHB, APB1 e APB2 */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                              | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2; // APB1 limitado a 42MHz no F411
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
    while(1); // Erro ao configurar divisores de clock
  }
}


void MX_GPIO_Init(void) {
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
}