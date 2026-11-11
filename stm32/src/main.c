#include "main.h"
#include "config.h"
#include "display.h"

int main(void){
  HAL_Init();

  SystemClock_Config();

  MX_GPIO_Init();
  UART2_Init();

  HAL_UART_Receive_IT(&huart2, &cmdByteReceived, 1);

  motion_init();
  OLED_I2C_Init();
  OLED_Init();
  while (1){
    if(newCommandSaved){
      newCommandSaved = 0;

      if(cmdReceived[0] == 'M'){
        char type;
        float x, y;
        sscanf(cmdReceived, "%c %f %f", &type, &x. &y);
        motion_process_command(x, y);
      }
      else if(cmdReceived[0] == 'D'){
        char type;
        char txt[20];
        sscanf(cmdReceived, "%c %s", type, txt)
        OLED_WriteString(10, 10, txt);
      }
    }
  }
}