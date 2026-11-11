#ifndef MOTION_H
#define MOTION_H

#include "stm32f4xx_hal.h"

#define MAX_PWM_VALUE       999
#define WHEEL_BASE_WIDTH    15.0f
#define WHEEL_RADIUS        3.0f

void motion_init(void);
void motion_set_speeds(float left_power, float right_power);
void motion_process_joystick(float norm_x, float norm_y, float angle);
void motion_goto_target(float target_x, float target_y);
void motion_update_odometry(void);
void motion_stop(void);

#endif