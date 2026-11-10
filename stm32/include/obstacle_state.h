#ifndef OBSTACLE_STATE_H
#define OBSTACLE_STATE_H

#include "stm32f4xx_hal.h"

// ==========================================
// 1. CONSTANTES E CONFIGURAÇÕES DO SENSOR/SERVO
// ==========================================
#define ULTRASONIC_SAMPLES      10      // Quantidade de leituras para a média móvel
#define SAMPLE_INTERVAL_MS      5       // Intervalo entre cada leitura (ms)
#define OBSTACLE_THRESHOLD_CM   30      // Distância limite (cm) para considerar um obstáculo

// Estados da Máquina de Estados (FSM) de Desvio
typedef enum {
    OBSTACLE_STATE_IDLE = 0,        // Robô em navegação normal / monitoramento
    OBSTACLE_STATE_DETECTED,        // Obstáculo próximo detectado; inicia varredura
    OBSTACLE_STATE_SCAN_RIGHT,      // Servo varre a direita (ex: 90 graus)
    OBSTACLE_STATE_SCAN_LEFT,       // Servo varre a esquerda (ex: -90 graus)
    OBSTACLE_STATE_DECIDE_PATH,     // Avalia melhor lado livre
    OBSTACLE_STATE_AVOIDING,        // Executa a manobra de contorno
    OBSTACLE_STATE_RETURN_CENTER    // Retorna o servo para 0 graus e retoma o curso
} ObstacleState_t;

// ==========================================
// 2. PROTÓTIPOS DAS FUNÇÕES DE OBSTÁCULO
// ==========================================

// Inicializa os pinos do sensor ultrassônico, o PWM do servo e o timer de amostragem
void obstacle_init(void);

// Executa a rotina de captura de distância com média móvel (10 leituras a cada 5ms)
uint32_t obstacle_get_filtered_distance(void);

// Função principal da Máquina de Estados de Desvio (chamada periodicamente na main/task)
void obstacle_state_machine_run(void);

// Posiciona o servo-motor no ângulo desejado (ex: 0 = frente, +90 = direita, -90 = esquerda)
void obstacle_set_servo_angle(int16_t angle);

#endif // OBSTACLE_STATE_H