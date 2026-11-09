#ifndef CONFIG_H
#define CONFIG_H

#include "stm32f4xx_hal.h"

void Custom_Peripherals_Start(void);
void Set_Motor_Speeds(uint16_t left_speed, uint16_t right_speed);
void Robot_Move_Forward(void);
void Robot_Spin_Left(void);
void Robot_Spin_Right(void);
void Robot_Stop(void);
void Robot_Move_Backward(void);
void Robot_Turn_Left(void);
void Robot_Turn_Right(void);

#endif