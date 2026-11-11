#include "peripherals.h"
#include "config.h"
#include <string.h>

static uint32_t last_buzzer_tick = 0;
static uint8_t buzzer_locked = 0;
static char display_buffer[DISPLAY_MAX_LEN + 1];

static uint8_t melody_active = 0;
static uint8_t current_note_index = 0;
static uint32_t note_start_tick = 0;

extern I2C_HandleTypeDef hi2c1;

#define OLED_I2C_ADDRESS (0x3C << 1)

typedef struct {
    uint16_t frequency_delay_us;
    uint16_t duration_ms;
} Note_t;

static const Note_t startup_melody[] = {
    {851,  200}, // Ré (D5) - Nota inicial longa da introdução
    {1012, 100}, // Si (B4)
    {1276, 100}, // Sol (G4)
    {851,  100}, // Ré (D5)
    {1012, 100}, // Si (B4)
    {1276, 100}, // Sol (G4)
    {1012, 100}, // Si (B4)
    {956,  100}, // Dó (C5)
    {851,  100}, // Ré (D5)
    {758,  100}, // Mi (E5)
    {675,  100}, // Fá# (F#5)
    {637,  200}  // Sol (G5) - Resolução da frase inicial
};
#define MELODIA_TOTAL_NOTAS (sizeof(startup_melody) / sizeof(Note_t))

static void OLED_SendCommand(uint8_t cmd){
    uint8_t buf[2];
    buf[0] = 0x00;
    buf[1] = cmd;
    HAL_I2C_Master_Transmit(&hi2c1, OLED_I2C_ADDRESS, buf, 2, HAL_MAX_DELAY);
}

static void OLED_Init_Physical(void){
    OLED_SendCommand(0xAE);
    OLED_SendCommand(0xD5);
    OLED_SendCommand(0x80);
    OLED_SendCommand(0xA8);
    OLED_SendCommand(0x3F);
    OLED_SendCommand(0xD3);
    OLED_SendCommand(0x00);
    OLED_SendCommand(0x40);
    OLED_SendCommand(0x8D);
    OLED_SendCommand(0x14);
    OLED_SendCommand(0x20);
    OLED_SendCommand(0x00);
    OLED_SendCommand(0xA1);
    OLED_SendCommand(0xC8);
    OLED_SendCommand(0xDA);
    OLED_SendCommand(0x12);
    OLED_SendCommand(0x81);
    OLED_SendCommand(0xCF);
    OLED_SendCommand(0xD9);
    OLED_SendCommand(0xF1);
    OLED_SendCommand(0xDB);
    OLED_SendCommand(0x40);
    OLED_SendCommand(0xA4);
    OLED_SendCommand(0xA6);
    OLED_SendCommand(0xAF);
}

void peripherals_init(void){
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitStruct.Pin = BUZZER_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(BUZZER_PORT, &GPIO_InitStruct);

    last_buzzer_tick = 0;
    buzzer_locked = 0;
    melody_active = 0;
    current_note_index = 0;
    memset(display_buffer, 0, sizeof(display_buffer));

    OLED_Init_Physical();
}

void peripherals_display_write(const char *text){
    if (text == NULL) return;

    strncpy(display_buffer, text, DISPLAY_MAX_LEN);
    display_buffer[DISPLAY_MAX_LEN] = '\0';

    OLED_SendCommand(0x21);
    OLED_SendCommand(0);
    OLED_SendCommand(127);
    OLED_SendCommand(0x22);
    OLED_SendCommand(0);
    OLED_SendCommand(7);

    for (uint8_t i = 0; i < 128; i++){
        uint8_t data[2] = {0x40, 0x00};
        HAL_I2C_Master_Transmit(&hi2c1, OLED_I2C_ADDRESS, data, 2, HAL_MAX_DELAY);
    }
}

void peripherals_buzzer_trigger(void){
    uint32_t current_tick = HAL_GetTick();

    if (buzzer_locked) {
        if ((current_tick - last_buzzer_tick) >= BUZZER_COOLDOWN_MS) buzzer_locked = 0;
        else return;
    }

    melody_active = 1;
    current_note_index = 0;
    note_start_tick = current_tick;

    last_buzzer_tick = current_tick;
    buzzer_locked = 1;
}

void peripherals_process(void){
    uint32_t current_tick = HAL_GetTick();

    if (buzzer_locked && ((current_tick - last_buzzer_tick) >= BUZZER_COOLDOWN_MS)) buzzer_locked = 0;

    if (melody_active) {
        if (current_note_index < MELODIA_TOTAL_NOTAS){
            Note_t note = startup_melody[current_note_index];

            HAL_GPIO_TogglePin(BUZZER_PORT, BUZZER_PIN);
            for(volatile uint16_t i = 0; i < note.frequency_delay_us; i++);

            if ((current_tick - note_start_tick) >= note.duration_ms){
                current_note_index++;
                note_start_tick = current_tick;
            }
        }
        else{
            HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_RESET);
            melody_active = 0;
        }
    }
}