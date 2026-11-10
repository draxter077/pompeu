#include "motion.h"
#include "config.h"
#include <math.h>

// ==========================================
// 1. VARIÁVEIS LOCAIS DE CONTROLE DE MOVIMENTO
// ==========================================
static float target_linear_speed = 0.0f;
static float target_angular_speed = 0.0f;

// ==========================================
// 2. IMPLEMENTAÇÃO DAS FUNÇÕES DE MOVIMENTO
// ==========================================

// Inicializa os timers de PWM e os pinos de direção da Ponte H
void motion_init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin = MOTOR_LEFT_DIR_PIN | MOTOR_RIGHT_DIR_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(MOTOR_LEFT_DIR_PORT, &GPIO_InitStruct);

    // Inicializa os motores em estado de parada total
    motion_stop();
}

// Controla diretamente as velocidades e sentidos de cada roda (-1.0 a 1.0)
void motion_set_speeds(float left_power, float right_power) {
    // Verifica se a parada de emergência está ativa
    if (parada_emergencia_ativa) {
        motion_stop();
        return;
    }

    // Limitação de segurança dos valores normalizados (-1.0 a 1.0)
    if (left_power > 1.0f) left_power = 1.0f;
    if (left_power < -1.0f) left_power = -1.0f;
    if (right_power > 1.0f) right_power = 1.0f;
    if (right_power < -1.0f) right_power = -1.0f;

    // Define a direção do motor esquerdo
    if (left_power >= 0.0f) {
        HAL_GPIO_WritePin(MOTOR_LEFT_DIR_PORT, MOTOR_LEFT_DIR_PIN, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(MOTOR_LEFT_DIR_PORT, MOTOR_LEFT_DIR_PIN, GPIO_PIN_RESET);
        left_power = -left_power;
    }

    // Define a direção do motor direito
    if (right_power >= 0.0f) {
        HAL_GPIO_WritePin(MOTOR_RIGHT_DIR_PORT, MOTOR_RIGHT_DIR_PIN, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(MOTOR_RIGHT_DIR_PORT, MOTOR_RIGHT_DIR_PIN, GPIO_PIN_RESET);
        right_power = -right_power;
    }

    // Aplica o PWM correspondente (convertendo de 0.0-1.0 para o range do timer 0-999)
    uint32_t pwm_left = (uint32_t)(left_power * MAX_PWM_VALUE);
    uint32_t pwm_right = (uint32_t)(right_power * MAX_PWM_VALUE);

    // Exemplo de aplicação nos canais do Timer
    // __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, pwm_left);
    // __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, pwm_right);
}

// Processa o comando de joystick enviado pelo ESP32 (M X Y ANG)
void motion_process_joystick(float norm_x, float norm_y, float angle) {
    // Se o robô estiver em modo autônomo ou emergência, ignora o joystick manual
    if (modo_autonomo_ativo || parada_emergencia_ativa) {
        return;
    }

    // Cinemática diferencial básica baseada em mixagem de translação (norm_y) e rotação (norm_x)
    float left_power = norm_y + norm_x;
    float right_power = norm_y - norm_x;

    // Normalização caso ultrapassem os limites da faixa
    float max_val = fmaxf(fabsf(left_power), fabsf(right_power));
    if (max_val > 1.0f) {
        left_power /= max_val;
        right_power /= max_val;
    }

    motion_set_speeds(left_power, right_power);
}

// Rotina de navegação autônoma para atingir a coordenada alvo (GOTO X Y)
void motion_goto_target(float t_x, float t_y) {
    if (parada_emergencia_ativa) {
        motion_stop();
        return;
    }

    // Cálculo da distância e ângulo de erro até o alvo
    float dx = t_x - current_x;
    float dy = t_y - current_y;
    float distance = sqrtf(dx * dx + dy * dy);

    // Condição de chegada ao destino (margem de tolerância de 2 cm)
    if (distance < 2.0f) {
        motion_stop();
        modo_autonomo_ativo = 0;
        return;
    }

    // Lógica de alinhamento e avanço gradual em direção ao ponto (X, Y)
    float forward_speed = 0.5f; // Velocidade controlada para o modo autônomo
    motion_set_speeds(forward_speed, forward_speed);
}

// Atualiza a odometria e calcula a nova posição cartesiana (X, Y)
void motion_update_odometry(void) {
    float delta_distance = 0.1f; // Exemplo de incremento por ciclo de tick
    float current_heading = 0.0f; // Orientação atual do robô em radianos

    current_x += delta_distance * cosf(current_heading);
    current_y += delta_distance * sinf(current_heading);
}

// Interrompe imediatamente todos os motores
void motion_stop(void) {
    motion_set_speeds(0.0f, 0.0f);
}