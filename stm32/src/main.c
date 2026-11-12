#include "string.h"
#include "stdio.h"
#include "config.h"
#include "motion.h"
#include "display.h"
#include "button.h"
#include "photoresistor.h"

int main(void){
  HAL_Init(); // Inicializações do hardware
  SystemClock_Config();
  MX_GPIO_Init();
  UART2_Init();
  HAL_UART_Receive_IT(&huart2, &cmdByteReceived, 1); // Inicialização da interrupção
  motion_init();
  OLED_I2C_Init();
  OLED_Init();
  buttonInit();
  photoresistorInit();

  while(1){
    if(newCommandSaved){ // Lógica de tratamento de comandos
      newCommandSaved = 0;

      if(cmdReceived[0] == 'M'){ // Comandos para motores
        char type;
        float x, y;
        sscanf(cmdReceived, "%c %f %f", &type, &x, &y);
        motion_process_command(x, y);
      }
      else if(cmdReceived[0] == 'D'){ // Comandos de texto para display
        char type;
        char txt[20];
        sscanf(cmdReceived, "%c %s", &type, txt);
        OLED_WriteString(2, 10, txt);
      }

      strcpy(cmdReceived, ""); // Zera comando recebido para próxima ordem
    }
  }
}