#include "config.h"

// ==========================================
// 1. INSTANCIAÇÃO DAS VARIÁVEIS GLOBAIS
// ==========================================

volatile float current_x = 0.0f;
volatile float current_y = 0.0f;

volatile uint32_t distanciaObjeto = 0;

volatile uint8_t modo_autonomo_ativo = 0;
volatile uint8_t modo_obstaculo_ativo = 0;
volatile uint8_t parada_emergencia_ativa = 0;

volatile float target_x = 0.0f;
volatile float target_y = 0.0f;

// ==========================================
// 2. IMPLEMENTAÇÃO DAS FUNÇÕES DE CONFIGURAÇÃO
// ==========================================

// Configuração do Clock do Sistema (geralmente gerada pelo CubeMX)
void SystemClock_Config(void) {
    // Configurações de osciladores, PLL e barramentos AHB/APB
}

// Inicialização dos pinos de GPIO (Ponte H, Trigger/Echo, etc.)
void MX_GPIO_Init(void) {
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Configuração dos pinos de direção dos motores (Saídas)
    GPIO_InitStruct.Pin = MOTOR_LEFT_DIR_PIN | MOTOR_RIGHT_DIR_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(MOTOR_LEFT_DIR_PORT, &GPIO_InitStruct);

    // Configuração do pino Trigger do Ultrassônico (Saída)
    GPIO_InitStruct.Pin = ULTRASONIC_TRIG_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(ULTRASONIC_TRIG_PORT, &GPIO_InitStruct);

    // Configuração do pino Echo do Ultrassônico (Entrada)
    GPIO_InitStruct.Pin = ULTRASONIC_ECHO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(ULTRASONIC_ECHO_PORT, &GPIO_InitStruct);
}

// Inicialização da USART2 para comunicação com o ESP32
void MX_USART2_UART_Init(void) {
    // Configuração de BaudRate (115200), word length, parity, stop bits e modo TX/RX
}

// Inicialização dos Timers para PWM (Motores, Servo e Buzzer)
void MX_TIM_PWM_Init(void) {
    // Configuração de prescaler e period (ARR) para gerar os sinais PWM das rodas e do servo
}

// Rotina global unificada de inicialização do robô
void Robot_Init(void) {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART2_UART_Init();
    MX_TIM_PWM_Init();

    // Inicializa os estados iniciais das variáveis
    current_x = 0.0f;
    current_y = 0.0f;
    distanciaObjeto = 0;
    modo_autonomo_ativo = 0;
    modo_obstaculo_ativo = 0;
    parada_emergencia_ativa = 0;
}