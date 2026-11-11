#ifndef CONFIG_H
#define CONFIG_H

#include "stm32f4xx_hal.h"

#define UART_TYPE             USART2
#define UART_BAUD             115200
#define UART_BUFFER_SIZE      1024
#define UART_TX               GPIO_PIN_4
#define UART_RX               GPIO_PIN_5

#define MOTOR_LEFT_PWM_PIN    GPIO_PIN_0
#define MOTOR_LEFT_PWM_PORT   GPIOA
#define MOTOR_LEFT_DIR_PIN    GPIO_PIN_1
#define MOTOR_LEFT_DIR_PORT   GPIOA

#define MOTOR_RIGHT_PWM_PIN   GPIO_PIN_2
#define MOTOR_RIGHT_PWM_PORT  GPIOA
#define MOTOR_RIGHT_DIR_PIN   GPIO_PIN_3
#define MOTOR_RIGHT_DIR_PORT  GPIOA
#define MOTOR_MAX_POTENCY     0.75f

#define MAX_PWM_VALUE         999

#define OLED_SCK              GPIO_PIN_6
#define OLED_SDA              GPIO_PIN_7
#define OLED_I2C_PORT         I2C1
#define OLED_I2C_SPEED        40000
#define DISPLAY_MAX_LEN       20
#define OLED_I2C_ADDR         (0x3C << 1)

extern UART_HandleTypeDef huart2;
extern int newCommandSaved;
extern char cmdReceived[50];
extern uint8_t* cmdByteReceived;
extern int cmdReceivedIndex;
void UART2_Init(void);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void USART2_IRQHandler(void);

#endif