#ifndef PERIPHERALS_H
#define PERIPHERALS_H

#include "stm32f4xx_hal.h"

// ==========================================
// 1. CONSTANTES E CONFIGURAÇÕES DE PERIFÉRICOS
// ==========================================
#define DISPLAY_MAX_LEN     20      // Tamanho máximo da string suportada pelo display
#define BUZZER_COOLDOWN_MS  25000   // Período de bloqueio/cooldown do buzzer (25 segundos)

// ==========================================
// 2. PROTÓTIPOS DAS FUNÇÕES DE PERIFÉRICOS
// ==========================================

// Inicialização dos pinos e timers associados ao buzzer e ao display
void peripherals_init(void);

// Função para enviar e renderizar texto no display (limitado a 20 caracteres)
void peripherals_display_write(const char *text);

// Função para acionar o buzzer passivo, respeitando a regra de bloqueio de 25s
void peripherals_buzzer_trigger(void);

// Função de atualização periódica (deve ser chamada no loop principal ou tick para gerenciar o temporizador do buzzer)
void peripherals_process(void);

#endif // PERIPHERALS_H