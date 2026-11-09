#include "config.h"

// Instâncias globais dos periféricos (o CubeMX já costuma declarar o htim1, huart2, etc. em main.c, 
// mas caso queira centralizar, garanta que não haja duplicação com o que o CubeMX gerou).
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern UART_HandleTypeDef huart2;

// Função para iniciar os PWMs dos motores e do servo logo após a inicialização da HAL
void Custom_Peripherals_Start(void) {
    // Inicia os canais PWM da Ponte H (Lado Esquerdo e Direito)
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);

    // Inicia o PWM do Servo Motor (Base do sensor de proximidade)
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
}

// Funções auxiliares de controle de velocidade e direção para atender aos seus requisitos:
// - Baixa velocidade para não derrapar e perder precisão
// - Controle independente por lado e giro no próprio eixo (SPIN_L / SPIN_R)

void Set_Motor_Speeds(uint16_t left_speed, uint16_t right_speed) {
    // Limita a velocidade máxima para garantir baixa velocidade (ex: max 400 de 1000)
    if (left_speed > 400)  left_speed = 400;
    if (right_speed > 400) right_speed = 400;

    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, left_speed);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, right_speed);
}

void Robot_Move_Forward(void) {
    // Configura os pinos de direção da ponte H para frente e aplica baixa velocidade
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);   // Exemplo IN1
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET); // Exemplo IN2
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);   // Exemplo IN3
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET); // Exemplo IN4
    Set_Motor_Speeds(300, 300);
}

void Robot_Spin_Left(void) {
    // Giro no próprio eixo para a esquerda (um lado para frente, outro para trás)
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET); // Inverte lado esquerdo
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);   // Mantém lado direito para frente
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
    Set_Motor_Speeds(250, 250);
}

void Robot_Spin_Right(void) {
    // Giro no próprio eixo para a direita
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);   // Mantém lado esquerdo para frente
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET); // Inverte lado direito
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
    Set_Motor_Speeds(250, 250);
}

void Robot_Stop(void) {
    Set_Motor_Speeds(0, 0);
}

void Robot_Move_Backward(void) {
    // Inverte os pinos de direção da Ponte H para trás
    // (Ajuste os pinos conforme a sua ligação física: IN1=RESET, IN2=SET, etc.)
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
    Set_Motor_Speeds(300, 300); // Baixa velocidade
}

void Robot_Turn_Left(void) {
    // Curva suave para a esquerda: lado direito anda mais rápido que o esquerdo (ou para o esquerdo)
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
    Set_Motor_Speeds(150, 350); // Esquerdo mais lento, Direito mais rápido
}

void Robot_Turn_Right(void) {
    // Curva suave para a direita: lado esquerdo anda mais rápido que o direito
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
    Set_Motor_Speeds(350, 150); // Esquerdo mais rápido, Direito mais lento
}