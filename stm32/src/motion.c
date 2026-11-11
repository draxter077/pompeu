#include "motion.h"
#include "config.h"

static float robot_x = 0.0f;
static float robot_y = 0.0f;

void motion_init(void){
    robot_x = 0.0f;
    robot_y = 0.0f;
    motion_stop();
}

void motion_stop(void){
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
    
    HAL_GPIO_WritePin(MOTOR_LEFT_DIR_PORT, MOTOR_LEFT_DIR_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_RIGHT_DIR_PORT, MOTOR_RIGHT_DIR_PIN, GPIO_PIN_RESET);
}

void motion_set_constant_speed(uint8_t direction_forward){
    uint32_t pwm_75_percent = (MAX_PWM_VALUE * 0.75);

    GPIO_PinState dir_state = direction_forward ? GPIO_PIN_SET : GPIO_PIN_RESET;
    HAL_GPIO_WritePin(MOTOR_LEFT_DIR_PORT, MOTOR_LEFT_DIR_PIN, dir_state);
    HAL_GPIO_WritePin(MOTOR_RIGHT_DIR_PORT, MOTOR_RIGHT_DIR_PIN, dir_state);

    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, pwm_75_percent);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, pwm_75_percent);
}

void motion_process_joystick(float norm_x, float norm_y, float angle){
    if (parada_emergencia_ativa) {
        motion_stop();
        return;
    }

    if (norm_y > 0.1f) motion_set_constant_speed(1);
    else if (norm_y < -0.1f) motion_set_constant_speed(0);
    else motion_stop();
}

void motion_goto_target(float t_x, float t_y){
    if (parada_emergencia_ativa) {
        motion_stop();
        return;
    }

    motion_set_constant_speed(1);
}

void motion_update_odometry(void){
    current_x = robot_x;
    current_y = robot_y;
}