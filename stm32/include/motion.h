#ifndef MOTION_H
#define MOTION_H

#include "stm32f4xx_hal.h"

extern TIM_HandleTypeDef htim1;
void motion_init(void);
void motion_stop(void);
void motion_process_command(float x, float y);
void motion_set_speeds(float left_power, float right_power);

#endif