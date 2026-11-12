#ifndef DISPLAY_H
#define DISPLAY_H

extern I2C_HandleTypeDef hi2c1; // Handler para comunicação  i2c
void OLED_I2C_Init(void); // Inicialização do i2c
void OLED_Init(void); // Inicialização do OLED
void OLED_WriteString(uint8_t x, uint8_t y, const char* str); // Escrita do texto no display

#endif