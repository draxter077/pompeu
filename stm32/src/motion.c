#include "motion.h"
#include "config.h"
#include "math.h"

TIM_HandleTypeDef htim2

void motion_init(void){
    __HAL_RCC_TIM2_CLK_ENABLE(); // Habilita o clock do timer 2

    GPIO_InitTypeDef GPIO_InitStruct = {0}; // Configuração dos pinos do PWM
    GPIO_InitStruct.Pin = MOTOR_LEFT_PWM_PIN | MOTOR_RIGHT_PWM_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    htim2.Instance = TIM2; // Configuração do timer
    htim2.Init.Prescaler = 3;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = MAX_PWM_VALUE;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    HAL_TIM_Base_Init(&htim2);

    TIM_OC_InitTypeDef sConfigOC = {0}; // Configuração do canal
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

    HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1); // Inicialização e configuração finais do PWM
    HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);

    GPIO_InitTypeDef GPIO_InitStruct = {0}; // Configuração de pinos digitais
    GPIO_InitStruct.Pin = MOTOR_LEFT_DIR_PIN | MOTOR_RIGHT_DIR_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    motion_stop(); // Começa zerado
}

static void motion_stop(void){ // Para o envio de sinais para os motores
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0); // Zera o PWM
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);
    HAL_GPIO_WritePin(MOTOR_LEFT_DIR_PORT, MOTOR_LEFT_DIR_PIN, GPIO_PIN_RESET); // Zera o envio de sinais de direção
    HAL_GPIO_WritePin(MOTOR_RIGHT_DIR_PORT, MOTOR_RIGHT_DIR_PIN, GPIO_PIN_RESET);
}

void motion_process_command(float x, float y){
    if (y > 0.1f){ // Movimento para frente
        if (x > 0.1f) motion_set_speeds(MOTOR_MAX_POTENCY, (1-x)*MOTOR_MAX_POTENCY, 1, 1); // Vira a direita
        else if (x < -0.1f) motion_set_speeds((1-x)*MOTOR_MAX_POTENCY, MOTOR_MAX_POTENCY, 1, 1); // Vira a esquerda
        else motion_set_speeds(MOTOR_MAX_POTENCY, MOTOR_MAX_POTENCY, 1, 1); // Linha reta
    }
    else if (y < 0.1f){ // Movimento para trás
        if (x > 0.1f) motion_set_speeds(MOTOR_MAX_POTENCY, -(1-x)*MOTOR_MAX_POTENCY, 0, 1); // Vira para a direita
        else if (x < -0.1f) motion_set_speeds(-(1-x)*MOTOR_MAX_POTENCY, MOTOR_MAX_POTENCY, 1, 0); // Vira para a esquerda
        else motion_set_speeds(MOTOR_MAX_POTENCY, MOTOR_MAX_POTENCY, 0, 0); // Linha reta
    }
    else if (x > 0.5f) motion_set_speeds(MOTOR_MAX_POTENCY, 0, 1, 0); // Gira no eixo para a direita
    else if (x < -0.5f) motion_set_speeds(0, MOTOR_MAX_POTENCY, 0, 1); // Gira no eixo para a esquerda
    else motion_stop(); // Caso não tenha valor relevante, zera os motores
}

static void motion_set_speeds(float left_power, float right_power, int left_dir, int right_dir){ // Define potência e sentido dos motores
    uint32_t left_pwm = (uint32_t) (left_power*(float)MAX_PWM_VALUE); // Potência a ser enviada para cada lado
    uint32_t right_pwm = (uint32_t) (right_power*(float)MAX_PWM_VALUE);

    HAL_GPIO_WritePin(MOTOR_LEFT_DIR_PORT, MOTOR_LEFT_DIR_PIN, left_dir ? GPIO_PIN_SET : GPIO_PIN_RESET); // Envio do sentido desejado
    HAL_GPIO_WritePin(MOTOR_RIGHT_DIR_PORT, MOTOR_RIGHT_DIR_PIN, right_dir ? GPIO_PIN_SET : GPIO_PIN_RESET);

    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, left_pwm); // Envia a potência de cada lado
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, right_pwm);
}