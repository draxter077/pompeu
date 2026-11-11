#ifndef CONFIG_H
#define CONFIG_H

#include "stm32f4xx_hal.h"

#define UART_ESP              USART2
#define UART_ESP_BAUD         115200

#define MOTOR_LEFT_PWM_PIN    GPIO_PIN_0
#define MOTOR_LEFT_PWM_PORT   GPIOA
#define MOTOR_LEFT_DIR_PIN    GPIO_PIN_1
#define MOTOR_LEFT_DIR_PORT   GPIOA

#define MOTOR_RIGHT_PWM_PIN   GPIO_PIN_2
#define MOTOR_RIGHT_PWM_PORT  GPIOA
#define MOTOR_RIGHT_DIR_PIN   GPIO_PIN_3
#define MOTOR_RIGHT_DIR_PORT  GPIOA

#define ULTRASONIC_TRIG_PIN   GPIO_PIN_13
#define ULTRASONIC_TRIG_PORT  GPIOC
#define ULTRASONIC_ECHO_PIN   GPIO_PIN_14
#define ULTRASONIC_ECHO_PORT  GPIOC

#define SERVO_PWM_PIN         GPIO_PIN_6
#define SERVO_PWM_PORT        GPIOB

#define BUZZER_PIN            GPIO_PIN_8
#define BUZZER_PORT           GPIOB

#define MAX_PWM_VALUE         999
#define ULTRASONIC_SAMPLES    10
#define SAMPLE_INTERVAL_MS    5
#define OBSTACLE_THRESHOLD_CM 30
#define BUZZER_COOLDOWN_MS    25000
#define DISPLAY_MAX_LEN       20

extern UART_HandleTypeDef huart2;
extern I2C_HandleTypeDef hi2c1;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;

extern volatile float current_x;
extern volatile float current_y;

extern volatile uint32_t distanciaObjeto;

extern volatile uint8_t modo_autonomo_ativo;
extern volatile uint8_t modo_obstaculo_ativo;
extern volatile uint8_t parada_emergencia_ativa;

extern volatile float target_x;
extern volatile float target_y;

void MX_GPIO_Init(void);
void MX_USART2_UART_Init(void);
void MX_TIM_PWM_Init(void);
void Robot_Init(void);

void motion_init(void);
void motion_stop(void);
void motion_set_speeds(float left_power, float right_power);
void motion_process_joystick(float norm_x, float norm_y, float angle);
void motion_goto_target(float t_x, float t_y);
void motion_update_odometry(void);

void sensors_init(void);
void sensors_process(void);
uint32_t sensors_get_filtered_distance(void);
void sensors_set_servo_angle(int16_t angle);

void obstacle_init(void);
void obstacle_state_machine_run(void);

void peripherals_init(void);
void peripherals_process(void);
void peripherals_display_write(const char *text);
void peripherals_buzzer_trigger(void);

#endif