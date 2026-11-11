#ifndef DISPLAY_H
#define DISPLAY_H

extern I2C_HandleTypeDef hi2c1;
void OLED_I2C_Init(void);
void OLED_Init(void);
void OLED_WriteString(uint8_t x, uint8_t y, const char* str);

#endif