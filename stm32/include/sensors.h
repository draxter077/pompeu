#ifndef SENSORS_H
#define SENSORS_H

#include "stm32f4xx_hal.h"

// ==========================================
// 1. CONSTANTES E CONFIGURAÇÕES DE SENSORES
// ==========================================
#define ULTRASONIC_SAMPLES      10      // Quantidade de leituras para cálculo da média móvel
#define SAMPLE_INTERVAL_MS      5       // Intervalo de tempo entre as amostras (ms)
#define OBSTACLE_THRESHOLD_CM   30      // Distância limite (cm) para detecção de obstáculos

// ==========================================
// 2. PROTÓTIPOS DAS FUNÇÕES DE SENSORES
// ==========================================

// Inicializa os pinos de Trigger/Echo do ultrassônico e o PWM do servo-motor
void sensors_init(void);

// Dispara um pulso síncrono para o sensor ultrassônico
void sensors_trigger_ultrasonic(void);

// Realiza a leitura e aplica o filtro de média móvel (10 leituras espaçadas por 5ms)
uint32_t sensors_get_filtered_distance(void);

// Posiciona o servo-motor no ângulo especificado para varredura do ambiente
void sensors_set_servo_angle(int16_t angle);

// Função de atualização periódica para leitura contínua e processamento de background
void sensors_process(void);

#endif // SENSORS_H