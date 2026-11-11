#include "motion.h"
#include "config.h"
#include <math.h>

void motion_init(void){
    motion_stop();
}

void motion_stop(void){
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
    HAL_GPIO_WritePin(MOTOR_LEFT_DIR_PORT, MOTOR_LEFT_DIR_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_RIGHT_DIR_PORT, MOTOR_RIGHT_DIR_PIN, GPIO_PIN_RESET);
}

void motion_process_command(float x, float y){
    if (y > 0.1f) {
        if (x > 0.1f) motion_set_speeds(MOTOR_MAX_POTENCY, (1-x)*MOTOR_MAX_POTENCY);
        else if (x < -0.1f) motion_set_speeds((1-x)*MOTOR_MAX_POTENCY, MOTOR_MAX_POTENCY);
        else motion_set_speeds(MOTOR_MAX_POTENCY, MOTOR_MAX_POTENCY);
    }
    else if (y < 0.1f) {
        if (x > 0.1f) motion_set_speeds(-MOTOR_MAX_POTENCY, -(1-x)*MOTOR_MAX_POTENCY);
        else if (x < -0.1f) motion_set_speeds(-(1-x)*MOTOR_MAX_POTENCY, -MOTOR_MAX_POTENCY);
        else motion_set_speeds(-MOTOR_MAX_POTENCY, -MOTOR_MAX_POTENCY);
    }
    else if (x > 0.5f) motion_set_speeds(MOTOR_MAX_POTENCY, 0);
    else if (x < -0.5f) motion_set_speeds(0, MOTOR_MAX_POTENCY);
    else motion_stop();
}

void motion_set_speeds(float left_power, float right_power) {
    uint32_t left_pwm = (uint32_t)(left_power*(float)MAX_PWM_VALUE);
    uint32_t right_pwm = (uint32_t)(right_power*(float)MAX_PWM_VALUE);

    HAL_GPIO_WritePin(MOTOR_LEFT_DIR_PORT, MOTOR_LEFT_DIR_PIN, left_power > 0 ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_RIGHT_DIR_PORT, MOTOR_RIGHT_DIR_PIN, right_power > 0 ? GPIO_PIN_SET : GPIO_PIN_RESET);

    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, left_pwm);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, right_pwm);
}