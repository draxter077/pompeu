#ifndef MOTION_H
#define MOTION_H

#include "stm32f4xx_hal.h"

#define MOTOR_LEFT_PWM_PIN    GPIO_PIN_0 // Variáveis lado esquerdo do gateH
#define MOTOR_LEFT_PWM_PORT   GPIOA
#define MOTOR_LEFT_DIR_PIN    GPIO_PIN_1
#define MOTOR_LEFT_DIR_PORT   GPIOA

#define MOTOR_RIGHT_PWM_PIN   GPIO_PIN_2 // Variáveis lado direito do gateH
#define MOTOR_RIGHT_PWM_PORT  GPIOA
#define MOTOR_RIGHT_DIR_PIN   GPIO_PIN_3
#define MOTOR_RIGHT_DIR_PORT  GPIOA

#define MOTOR_MAX_POTENCY     0.75f // Variáveis motor
#define MAX_PWM_VALUE         999

extern TIM_HandleTypeDef htim2; // Handle do timer 2 para o PWM
void motion_init(void); // Inicializa e configura pinos dos motores
void motion_process_command(float x, float y); // Recebe o cmd do ESP32 e transforma em sinal para rodas

#endif