#include "sensors.h"
#include "config.h"

// ==========================================
// 1. VARIÁVEIS LOCAIS DE CONTROLE DE SENSORES
// ==========================================
static uint32_t sensor_samples[ULTRASONIC_SAMPLES];
static uint8_t sample_cursor = 0;
static uint32_t last_sample_timestamp = 0;

// ==========================================
// 2. IMPLEMENTAÇÃO DAS FUNÇÕES DE SENSORES
// ==========================================

// Inicializa os pinos do ultrassônico e o PWM do servo-motor
void sensors_init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // Configuração do pino Trigger (Saída)
    GPIO_InitStruct.Pin = ULTRASONIC_TRIG_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(ULTRASONIC_TRIG_PORT, &GPIO_InitStruct);

    // Configuração do pino Echo (Entrada)
    GPIO_InitStruct.Pin = ULTRASONIC_ECHO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(ULTRASONIC_ECHO_PORT, &GPIO_InitStruct);

    // Inicializa variáveis locais
    sample_cursor = 0;
    last_sample_timestamp = 0;
    for (uint8_t i = 0; i < ULTRASONIC_SAMPLES; i++) {
        sensor_samples[i] = 0;
    }

    // Posiciona o servo no centro (0 graus)
    sensors_set_servo_angle(0);
}

// Dispara o pulso de 10us para iniciar a medição do HC-SR04
void sensors_trigger_ultrasonic(void) {
    HAL_GPIO_WritePin(ULTRASONIC_TRIG_PORT, ULTRASONIC_TRIG_PIN, GPIO_PIN_RESET);
    // Delay curto para estabilização (ex: 2us)
    HAL_GPIO_WritePin(ULTRASONIC_TRIG_PORT, ULTRASONIC_TRIG_PIN, GPIO_PIN_SET);
    // Pulso de 10us
    HAL_GPIO_WritePin(ULTRASONIC_TRIG_PORT, ULTRASONIC_TRIG_PIN, GPIO_PIN_RESET);
}

// Realiza a leitura da distância aplicando o filtro de média móvel (10 leituras / 5ms)
uint32_t sensors_get_filtered_distance(void) {
    uint32_t current_tick = HAL_GetTick();

    // Coleta nova amostra apenas se o intervalo de tempo configurado tiver transcorrido
    if ((current_tick - last_sample_timestamp) >= SAMPLE_INTERVAL_MS) {
        last_sample_timestamp = current_tick;

        sensors_trigger_ultrasonic();

        // Leitura simulada/capturada do tempo de eco convertido para centímetros
        uint32_t measured_distance = 40; // Exemplo de valor bruto obtido do sensor

        sensor_samples[sample_cursor] = measured_distance;
        sample_cursor = (sample_cursor + 1) % ULTRASONIC_SAMPLES;
    }

    // Calcula a média aritmética do vetor de amostras
    uint32_t cumulative_sum = 0;
    for (uint8_t i = 0; i < ULTRASONIC_SAMPLES; i++) {
        cumulative_sum += sensor_samples[i];
    }

    distanciaObjeto = cumulative_sum / ULTRASONIC_SAMPLES;
    return distanciaObjeto;
}

// Posiciona o servo-motor associado ao sensor ultrassônico
void sensors_set_servo_angle(int16_t angle) {
    uint32_t pwm_compare_value = 1500; // Neutro (90 graus / centro)

    if (angle == -90) {
        pwm_compare_value = 1000; // Esquerda
    } else if (angle == 90) {
        pwm_compare_value = 2000; // Direita
    }

    // Aplicação do valor no canal do Timer PWM do servo
    // __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, pwm_compare_value);
}

// Função de atualização periódica executada em background
void sensors_process(void) {
    // Atualiza leituras contínuas do sensor sem bloquear o fluxo principal
    (void)sensors_get_filtered_distance();
}