#include "config.h"
#include "display.h"

I2C_HandleTypeDef hi2c1;
void OLED_I2C_Init(void) {
  __HAL_RCC_I2C1_CLK_ENABLE(); // Habilita clock do i2c

  GPIO_InitTypeDef GPIO_InitStruct = {0}; // Configuração dos pinos
  GPIO_InitStruct.Pin = OLED_SCK | OLED_SDA;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF4_I2C1; // Verifique o AF correto no datasheet do seu STM32 (ex: AF4 para F4)
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  hi2c1.Instance = OLED_I2C_PORT; // Configuração da comunicação i2c
  hi2c1.Init.ClockSpeed = OLED_I2C_SPEED;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  HAL_I2C_Init(&hi2c1);
}

static void OLED_WriteCommand(uint8_t cmd){ // Envia comunicações do tipo configuração/modo
    uint8_t buffer[2];
    buffer[0] = 0x00; // Co-Byte
    buffer[1] = cmd;
    HAL_I2C_Master_Transmit(&hi2c1, OLED_I2C_ADDR, buffer, 2, HAL_MAX_DELAY); // Usando o i2c, envia a comunicação
}

static void OLED_WriteData(uint8_t* data, uint16_t size){ // Envia comunicações do tipo dados de imagem/pixel
    uint8_t buffer[129];
    buffer[0] = 0x40; // Co-byte
    for (uint16_t i = 0; i < size; i++) {
        buffer[i + 1] = data[i];
    }
    HAL_I2C_Master_Transmit(&hi2c1, OLED_I2C_ADDR, buffer, size + 1, HAL_MAX_DELAY); // Usando o i2c, envia a comunicação
}

void OLED_Init(void) { // Inicialização do OLED
    HAL_Delay(100); // Estabilização do OLED
    
    OLED_WriteCommand(0xAE); // Desliga o display
    OLED_WriteCommand(0xD5); // Configuração do clock
    OLED_WriteCommand(0x80); 
    OLED_WriteCommand(0xA8); // Define proporção
    OLED_WriteCommand(0x3F); 
    OLED_WriteCommand(0xD3); // Zera o ponteiro do texto
    OLED_WriteCommand(0x00); 
    OLED_WriteCommand(0x40);
    OLED_WriteCommand(0x8D); // Energiza os LEDs
    OLED_WriteCommand(0x14);
    OLED_WriteCommand(0x20); // Configurações de apresentação
    OLED_WriteCommand(0x00);
    OLED_WriteCommand(0xA1);
    OLED_WriteCommand(0xC8);
    OLED_WriteCommand(0xDA);
    OLED_WriteCommand(0x12);
    OLED_WriteCommand(0x81); // Ajustes de imagem e cor
    OLED_WriteCommand(0xCF);
    OLED_WriteCommand(0xD9);
    OLED_WriteCommand(0xF1);
    OLED_WriteCommand(0xDB);
    OLED_WriteCommand(0x40);
    OLED_WriteCommand(0xA4);
    OLED_WriteCommand(0xA6);
    OLED_WriteCommand(0xAF); // Liga o display
}

static void OLED_Clear(void){ // Limpa a tela e memória
    OLED_WriteCommand(0x21); // Define modos de exclusão
    OLED_WriteCommand(0);
    OLED_WriteCommand(127);

    OLED_WriteCommand(0x22); // Apaga cada uma das linhas
    OLED_WriteCommand(0);
    OLED_WriteCommand(7);

    uint8_t zero_buffer[128];
    for(int i = 0; i < 128; i++) zero_buffer[i] = 0x00; // Buffer zerado
    for (uint8_t i = 0; i < 8; i++) OLED_WriteData(zero_buffer, 128); // Zera os LEDs
}

static const uint8_t Font5x7[][5] = { // Dicionário de caracteres ASCII
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 32: Espaço
    {0x00, 0x00, 0x5f, 0x00, 0x00}, // 33: !
    {0x00, 0x07, 0x00, 0x07, 0x00}, // 34: "
    {0x14, 0x7f, 0x14, 0x7f, 0x14}, // 35: #
    {0x24, 0x2a, 0x7f, 0x2a, 0x12}, // 36: $
    {0x23, 0x13, 0x08, 0x64, 0x62}, // 37: %
    {0x36, 0x49, 0x55, 0x22, 0x50}, // 38: &
    {0x00, 0x05, 0x03, 0x00, 0x00}, // 39: '
    {0x00, 0x1c, 0x22, 0x41, 0x00}, // 40: (
    {0x00, 0x41, 0x22, 0x1c, 0x00}, // 41: )
    {0x14, 0x08, 0x3e, 0x08, 0x14}, // 42: *
    {0x08, 0x08, 0x3e, 0x08, 0x08}, // 43: +
    {0x00, 0x50, 0x30, 0x00, 0x00}, // 44: ,
    {0x08, 0x08, 0x08, 0x08, 0x08}, // 45: -
    {0x00, 0x60, 0x60, 0x00, 0x00}, // 46: .
    {0x20, 0x10, 0x08, 0x04, 0x02}, // 47: /
    {0x3e, 0x51, 0x49, 0x45, 0x3e}, // 48: 0
    {0x00, 0x42, 0x7f, 0x40, 0x00}, // 49: 1
    {0x42, 0x61, 0x51, 0x49, 0x46}, // 50: 2
    {0x21, 0x41, 0x45, 0x4b, 0x31}, // 51: 3
    {0x18, 0x14, 0x12, 0x7f, 0x10}, // 52: 4
    {0x27, 0x45, 0x45, 0x45, 0x39}, // 53: 5
    {0x3c, 0x4a, 0x49, 0x49, 0x30}, // 54: 6
    {0x01, 0x71, 0x09, 0x05, 0x03}, // 55: 7
    {0x36, 0x49, 0x49, 0x49, 0x36}, // 56: 8
    {0x06, 0x49, 0x49, 0x29, 0x1e}, // 57: 9
    {0x00, 0x36, 0x36, 0x00, 0x00}, // 58: :
    {0x00, 0x56, 0x36, 0x00, 0x00}, // 59: ;
    {0x08, 0x14, 0x22, 0x41, 0x00}, // 60: <
    {0x14, 0x14, 0x14, 0x14, 0x14}, // 61: =
    {0x00, 0x41, 0x22, 0x14, 0x08}, // 62: >
    {0x02, 0x01, 0x51, 0x09, 0x06}, // 63: ?
    {0x32, 0x49, 0x79, 0x41, 0x3e}, // 64: @
    {0x7e, 0x11, 0x11, 0x11, 0x7e}, // 65: A
    {0x7f, 0x49, 0x49, 0x49, 0x36}, // 66: B
    {0x3e, 0x41, 0x41, 0x41, 0x22}, // 67: C
    {0x7f, 0x41, 0x41, 0x22, 0x1c}, // 68: D
    {0x7f, 0x49, 0x49, 0x49, 0x41}, // 69: E
    {0x7f, 0x09, 0x09, 0x09, 0x01}, // 70: F
    {0x3e, 0x41, 0x49, 0x49, 0x7a}, // 71: G
    {0x7f, 0x08, 0x08, 0x08, 0x7f}, // 72: H
    {0x00, 0x41, 0x7f, 0x41, 0x00}, // 73: I
    {0x20, 0x40, 0x41, 0x3f, 0x01}, // 74: J
    {0x7f, 0x08, 0x14, 0x22, 0x41}, // 75: K
    {0x7f, 0x40, 0x40, 0x40, 0x40}, // 76: L
    {0x7f, 0x02, 0x0c, 0x02, 0x7f}, // 77: M
    {0x7f, 0x04, 0x08, 0x10, 0x7f}, // 78: N
    {0x3e, 0x41, 0x41, 0x41, 0x3e}, // 79: O
    {0x7f, 0x09, 0x09, 0x09, 0x06}, // 80: P
    {0x3e, 0x41, 0x51, 0x21, 0x5e}, // 81: Q
    {0x7f, 0x09, 0x09, 0x19, 0x66}, // 82: R
    {0x26, 0x49, 0x49, 0x49, 0x32}, // 83: S
    {0x01, 0x01, 0x7f, 0x01, 0x01}, // 84: T
    {0x3f, 0x40, 0x40, 0x40, 0x3f}, // 85: U
    {0x1f, 0x20, 0x40, 0x20, 0x1f}, // 86: V
    {0x3f, 0x40, 0x38, 0x40, 0x3f}, // 87: W
    {0x63, 0x14, 0x08, 0x14, 0x63}, // 88: X
    {0x07, 0x08, 0x70, 0x08, 0x07}, // 89: Y
    {0x61, 0x51, 0x49, 0x45, 0x43}  // 90: Z
};

void OLED_WriteString(uint8_t page, uint8_t col, const char* str){
    OLED_Clear();

    OLED_WriteCommand(0x21); // Define a coluna da escrita
    OLED_WriteCommand(col);
    OLED_WriteCommand(127);

    OLED_WriteCommand(0x22); // Define a linha
    OLED_WriteCommand(page);
    OLED_WriteCommand(7);

    while(*str){ // Envia a string
        char c = *str++;
        uint8_t index = 0;

        if (c >= 32 && c <= 90) index = c - 32; // Confirma ser caractere válido para mapeamento na lista

        uint8_t char_data[5];
        for (int i = 0; i < 5; i++) char_data[i] = Font5x7[index][i]; // Salva os 5 bytes do caractere
        OLED_WriteData(char_data, 5); // Envia o caractere
        uint8_t space = 0x00;
        OLED_WriteData(&space, 1); // Adiciona um espaço entre as letras
    }
}