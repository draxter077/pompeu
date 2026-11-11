#ifndef MOTION_H
#define MOTION_H

#include "stm32f4xx_hal.h"

void motion_init(void);
void motion_process_command(float x, float y);

#endif