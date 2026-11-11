#ifndef PERIPHERALS_H
#define PERIPHERALS_H

#include "stm32f4xx_hal.h"

void peripherals_init(void);
void peripherals_display_write(const char *text);
void peripherals_buzzer_trigger(void);
void peripherals_process(void);

#endif