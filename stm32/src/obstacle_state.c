#include "obstacle_state.h"
#include "config.h"
#include "motion.h"

// ==========================================
// 1. VARIÁVEIS LOCAIS E ESTADO DA FSM
// ==========================================
static ObstacleState_t current_obstacle_state = OBSTACLE_STATE_IDLE;
static uint32_t distance_samples[ULTRASONIC_SAMPLES];
static uint8_t sample_index = 0;
static uint32_t last_sample_tick = 0;

static uint32_t right_distance_cm = 0;
static uint32_t left_distance_cm = 0;

// ==========================================
// 2. IMPLEMENTAÇÃO DAS FUNÇÕES DE OBSTÁCULO
// ==========================================

// Inicialização dos pinos, PWM do servo e variáveis de estado
void obstacle_init(void) {
    current_obstacle_state = OBSTACLE_STATE_IDLE;
    sample_index = 0;
    last_sample_tick = 0;

    // Inicializa o servo na posição central (0 graus)
    obstacle_set_servo_angle(0);
}

// Posiciona o servo-motor no ângulo desejado via PWM
void obstacle_set_servo_angle(int16_t angle) {
    uint32_t pwm_pulse = 1500; // Valor padrão neutro (1.5ms)
    
    if (angle == -90) {
        pwm_pulse = 1000;
    } else if (angle == 90) {
        pwm_pulse = 2000;
    }

    // Aplicação no canal do Timer PWM do servo
    // __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, pwm_pulse);
}

// Executa a leitura filtrada por média móvel (10 amostras a cada 5ms)
uint32_t obstacle_get_filtered_distance(void) {
    uint32_t current_tick = HAL_GetTick();

    // Coleta uma nova amostra a cada 5ms
    if ((current_tick - last_sample_tick) >= SAMPLE_INTERVAL_MS) {
        last_sample_tick = current_tick;

        // Disparo e captura do sensor HC-SR04
        HAL_GPIO_WritePin(ULTRASONIC_TRIG_PORT, ULTRASONIC_TRIG_PIN, GPIO_PIN_SET);
        // Delay curto de microsegundos...
        HAL_GPIO_WritePin(ULTRASONIC_TRIG_PORT, ULTRASONIC_TRIG_PIN, GPIO_PIN_RESET);

        uint32_t raw_distance = 35; // Valor simulado de leitura em cm

        distance_samples[sample_index] = raw_distance;
        sample_index = (sample_index + 1) % ULTRASONIC_SAMPLES;
    }

    // Calcula a média móvel das últimas 10 amostras
    uint32_t sum = 0;
    for (uint8_t i = 0; i < ULTRASONIC_SAMPLES; i++) {
        sum += distance_samples[i];
    }

    distanciaObjeto = sum / ULTRASONIC_SAMPLES;
    return distanciaObjeto;
}

// Máquina de Estados Finitos (FSM) para desvio de obstáculos
void obstacle_state_machine_run(void) {
    // Se o modo autônomo ou de desvio não estiver ativo, reseta para IDLE
    if (!modo_autonomo_ativo && !modo_obstaculo_ativo) {
        current_obstacle_state = OBSTACLE_STATE_IDLE;
        return;
    }

    uint32_t current_dist = obstacle_get_filtered_distance();

    switch (current_obstacle_state) {
        case OBSTACLE_STATE_IDLE:
            if (current_dist <= OBSTACLE_THRESHOLD_CM && current_dist > 0) {
                motion_stop();
                current_obstacle_state = OBSTACLE_STATE_DETECTED;
            }
            break;

        case OBSTACLE_STATE_DETECTED:
            obstacle_set_servo_angle(90);
            current_obstacle_state = OBSTACLE_STATE_SCAN_RIGHT;
            break;

        case OBSTACLE_STATE_SCAN_RIGHT:
            right_distance_cm = obstacle_get_filtered_distance();
            obstacle_set_servo_angle(-90);
            current_obstacle_state = OBSTACLE_STATE_SCAN_LEFT;
            break;

        case OBSTACLE_STATE_SCAN_LEFT:
            left_distance_cm = obstacle_get_filtered_distance();
            current_obstacle_state = OBSTACLE_STATE_DECIDE_PATH;
            break;

        case OBSTACLE_STATE_DECIDE_PATH:
            if (right_distance_cm >= left_distance_cm) {
                // Direita está mais livre ou igual
            } else {
                // Esquerda está mais livre
            }
            current_obstacle_state = OBSTACLE_STATE_AVOIDING;
            break;

        case OBSTACLE_STATE_AVOIDING:
            obstacle_set_servo_angle(0);
            current_obstacle_state = OBSTACLE_STATE_RETURN_CENTER;
            break;

        case OBSTACLE_STATE_RETURN_CENTER:
            current_obstacle_state = OBSTACLE_STATE_IDLE;
            break;

        default:
            current_obstacle_state = OBSTACLE_STATE_IDLE;
            break;
    }
}