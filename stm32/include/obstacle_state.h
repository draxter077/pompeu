#ifndef OBSTACLE_STATE_H
#define OBSTACLE_STATE_H

#include "stm32f4xx_hal.h"

typedef enum {
    OBSTACLE_STATE_IDLE = 0,
    OBSTACLE_STATE_DETECTED,
    OBSTACLE_STATE_SCAN_RIGHT,
    OBSTACLE_STATE_SCAN_LEFT,
    OBSTACLE_STATE_DECIDE_PATH,
    OBSTACLE_STATE_AVOIDING,
    OBSTACLE_STATE_RETURN_CENTER
} ObstacleState_t;

void obstacle_init(void);
uint32_t obstacle_get_filtered_distance(void);
void obstacle_state_machine_run(void);
void obstacle_set_servo_angle(int16_t angle);

#endif