#ifndef BUTTON_H
#define BUTTON_H

#include "stm32f4xx_hal.h"

#define BUTTON_PIN      GPIO_PIN_13
#define BUTTON_PORT     GPIOC

void buttonInit(void); // Inicializa o botão, criando a interrupção externa

#endif