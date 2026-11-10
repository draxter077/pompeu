#ifndef MOTION_H
#define MOTION_H

#include "stm32f4xx_hal.h"

// ==========================================
// 1. CONSTANTES E CONFIGURAÇÕES DE MOVIMENTO
// ==========================================
#define MAX_PWM_VALUE       999     // Valor máximo para o timer de PWM
#define WHEEL_BASE_WIDTH    15.0f   // Distância entre as rodas (em cm) para cinemática diferencial
#define WHEEL_RADIUS        3.0f    // Raio da roda (em cm)

// ==========================================
// 2. PROTÓTIPOS DAS FUNÇÕES DE MOVIMENTO
// ==========================================

// Inicialização dos timers de PWM e pinos de direção dos motores
void motion_init(void);

// Controle direto e diferencial dos motores (valores de -1.0 a 1.0 ou PWM bruto)
void motion_set_speeds(float left_power, float right_power);

// Processa o comando de movimento enviado pelo ESP32 via joystick (M X Y ANG)
void motion_process_joystick(float norm_x, float norm_y, float angle);

// Rotina de navegação autônoma para atingir uma coordenada alvo (GOTO X Y)
void motion_goto_target(float target_x, float target_y);

// Atualiza a posição cartesiana (X, Y) com base na contagem de encoders / odometria
void motion_update_odometry(void);

// Parada imediata de todos os motores (usada no Stop e Parada de Emergência)
void motion_stop(void);

#endif // MOTION_H