#ifndef PHOTORESISTOR_H
#define PHOTORESISTOR_H

#include "stm32f4xx_hal.h"

#define PHOTORESISTOR_PIN   GPIO_PIN_15
#define LIMIAR_BAIXA_LUZ  400

extern TIM_HandleTypeDef htim3; // Handle do timer 3
extern ADC_HandleTypeDef hadc1; // Handler de ADC

void photoresistorInit(void);

#endif