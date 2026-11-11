#include "config.h"

// Config da UART
UART_HandleTypeDef huart2;
char cmdReceived[50];
uint8_t* cmdByteReceived = 0;
int cmdReceivedIndex = 0;
int newCommandSaved = 0;
void UART2_Init(void){
  __HAL_RCC_USART2_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  // Pino TX GPIO4
  GPIO_InitStruct.Pin = UART_TX;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // Pino RX GPIO5
  GPIO_InitStruct.Pin = UART_RX;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // Configuração do UART
  huart2.Instance = UART_TYPE;
  huart2.Init.BaudRate = UART_BAUD;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;

  HAL_UART_INIT(&huart2);

  // 4. Configuração da Interrupção via código
  HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USART2_IRQn);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
  if (huart->Instance == UART_TYPE){
    if (cmdByteReceived == '\n' || cmdByteReceived >= (UART_BUFFER_SIZE - 1)){
      cmdReceived[cmdReceivedIndex] = '\0';
      newCommandSaved = 1;
      cmdReceivedIndex = 0;
    }
    else{
      cmdReceived[cmdReceivedIndex] = (char)cmdByteReceived;
      cmdReceivedIndex++;
    }

    HAL_UART_Receive_IT(&huart2, &cmdByteReceived, 1);
  }
}

void USART2_IRQHandler(void){
  HAL_UART_IRQHandler(&huart2);
}