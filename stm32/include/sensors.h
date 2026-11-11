#ifndef SENSORS_H
#define SENSORS_H

#include "stm32f4xx_hal.h"

void sensors_init(void);
void sensors_trigger_ultrasonic(void);
uint32_t sensors_get_filtered_distance(void);
void sensors_set_servo_angle(int16_t angle);
void sensors_process(void);

#endif