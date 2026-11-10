#include "peripherals.h"
#include "config.h"
#include <string.h>

// ==========================================
// 1. VARIÁVEIS LOCAIS DE CONTROLE
// ==========================================
static uint32_t last_buzzer_tick = 0;
static uint8_t buzzer_locked = 0;
static char display_buffer[DISPLAY_MAX_LEN + 1];

// ==========================================
// 2. IMPLEMENTAÇÃO DAS FUNÇÕES DE PERIFÉRICOS
// ==========================================

// Inicialização dos pinos e estados dos periféricos
void peripherals_init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();

    // Configuração do pino do Buzzer como saída digital / PWM
    GPIO_InitStruct.Pin = BUZZER_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(BUZZER_PORT, &GPIO_InitStruct);

    // Inicialização das variáveis de controle
    last_buzzer_tick = 0;
    buzzer_locked = 0;
    memset(display_buffer, 0, sizeof(display_buffer));
}

// Escreve e renderiza texto no display, respeitando o limite de caracteres
void peripherals_display_write(const char *text) {
    if (text == NULL) {
        return;
    }

    // Copia o texto de forma segura limitando ao tamanho máximo do display (20 chars)
    strncpy(display_buffer, text, DISPLAY_MAX_LEN);
    display_buffer[DISPLAY_MAX_LEN] = '\0';

    // Rotina de envio físico para o display (ex: barramento I2C/SPI ou pinos dedicados)
    // lcd_send_string(display_buffer);
}

// Aciona o buzzer passivo respeitando o intervalo de cooldown de 25 segundos
void peripherals_buzzer_trigger(void) {
    uint32_t current_tick = HAL_GetTick();

    // Verifica se o buzzer está em período de bloqueio
    if (buzzer_locked) {
        if ((current_tick - last_buzzer_tick) >= BUZZER_COOLDOWN_MS) {
            buzzer_locked = 0; // Libera após transcorrer o tempo de cooldown
        } else {
            return; // Ignora o comando se ainda estiver no período de proteção
        }
    }

    // Acionamento real do buzzer passivo (via PWM ou oscilação de pino)
    // HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_SET);

    // Registra o momento do acionamento e ativa o bloqueio
    last_buzzer_tick = current_tick;
    buzzer_locked = 1;
}

// Processamento periódico executado no loop principal para gerenciar temporizadores
void peripherals_process(void) {
    uint32_t current_tick = HAL_GetTick();

    // Atualiza o estado de bloqueio do buzzer em background sem travar o processador
    if (buzzer_locked && ((current_tick - last_buzzer_tick) >= BUZZER_COOLDOWN_MS)) {
        buzzer_locked = 0;
    }
}