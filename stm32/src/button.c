#include "button.h"

void buttonInit(void){
    GPIO_InitTypeDef GPIO_InitStruct = {0}; // Configuração do pino
    GPIO_InitStruct.Pin = BUTTON_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(BUTTON_PORT, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 1, 0); // Configuração da interrupção externa
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}