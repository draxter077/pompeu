#include "config.h"
#include "display.h"

// Config do display
I2C_HandleTypeDef hi2c1;
void OLED_I2C_Init(void) {
  __HAL_RCC_I2C1_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  // Configuração dos pinos PB6 (SCL) e PB7 (SDA) em Alternate Function Open-Drain
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = OLED_SCK | OLED_SDA;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF4_I2C1; // Verifique o AF correto no datasheet do seu STM32 (ex: AF4 para F4)
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  // Configuração do periferico I2C
  hi2c1.Instance = OLED_I2C_PORT;
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

static void OLED_WriteCommand(uint8_t cmd) {
    uint8_t buffer[2];
    buffer[0] = 0x00; // Co-byte: 0x00 indica que o próximo byte é um comando
    buffer[1] = cmd;
    HAL_I2C_Master_Transmit(&hi2c1, OLED_I2C_ADDR, buffer, 2, HAL_MAX_DELAY);
}

// Função auxiliar para enviar dados (pixels) ao OLED
static void OLED_WriteData(uint8_t* data, uint16_t size) {
    // Aloca um buffer temporário para colocar o co-byte 0x40 (dados) na frente
    uint8_t buffer[129];
    buffer[0] = 0x40; // Co-byte: 0x40 indica que os próximos bytes são dados de imagem
    
    for (uint16_t i = 0; i < size; i++) {
        buffer[i + 1] = data[i];
    }
    HAL_I2C_Master_Transmit(&hi2c1, OLED_I2C_ADDR, buffer, size + 1, HAL_MAX_DELAY);
}

// Inicialização essencial do SSD1306 para ligar o display
void OLED_Init(void) {
    HAL_Delay(100); // Aguarda o display estabilizar após ligar
    
    OLED_WriteCommand(0xAE); // Desliga o display (Display OFF)
    OLED_WriteCommand(0xD5); // Set Display Clock Divide Ratio / Oscillator Frequency
    OLED_WriteCommand(0x80); 
    OLED_WriteCommand(0xA8); // Set Multiplex Ratio (1/64 for 128x64)
    OLED_WriteCommand(0x3F); 
    OLED_WriteCommand(0xD3); // Set Display Offset
    OLED_WriteCommand(0x00); 
    OLED_WriteCommand(0x40); // Set Display Start Line
    OLED_WriteCommand(0x8D); // Charge Pump Setting
    OLED_WriteCommand(0x14); // Enable charge pump during display on
    OLED_WriteCommand(0x20); // Set Memory Addressing Mode
    OLED_WriteCommand(0x00); // Horizontal addressing mode
    OLED_WriteCommand(0xA1); // Set Segment Re-map (Mirror X)
    OLED_WriteCommand(0xC8); // Set COM Output Scan Direction (Mirror Y)
    OLED_WriteCommand(0xDA); // Set COM Pins Hardware Configuration
    OLED_WriteCommand(0x12); 
    OLED_WriteCommand(0x81); // Set Contrast Control
    OLED_WriteCommand(0xCF); 
    OLED_WriteCommand(0xD9); // Set Pre-charge Period
    OLED_WriteCommand(0xF1); 
    OLED_WriteCommand(0xDB); // Set VCOMH Deselect Level
    OLED_WriteCommand(0x40); 
    OLED_WriteCommand(0xA4); // Entire Display On (Resume)
    OLED_WriteCommand(0xA6); // Set Normal Display (Not inverted)
    OLED_WriteCommand(0xAF); // Liga o display (Display ON)
}

// Apaga o conteúdo da tela inteira (limpa a memória de vídeo)
static void OLED_Clear(void) {
    // O SSD1306 128x64 tem 8 páginas de 128 bytes cada (1024 bytes no total)
    OLED_WriteCommand(0x21); // Set Column Address
    OLED_WriteCommand(0);    // Start Column (0)
    OLED_WriteCommand(127);  // End Column (127)

    OLED_WriteCommand(0x22); // Set Page Address
    OLED_WriteCommand(0);    // Start Page (0)
    OLED_WriteCommand(7);    // End Page (7)

    uint8_t zero_buffer[128];
    for(int i = 0; i < 128; i++) zero_buffer[i] = 0x00;

    // Envia zeros para todas as 8 páginas da tela
    for (uint8_t i = 0; i < 8; i++) {
        OLED_WriteData(zero_buffer, 128);
    }
}

// Pequena tabela de fonte 5x7 padrão para caracteres ASCII básicos
static const uint8_t Font5x7[][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00}, // Espaço
    {0x00, 0x00, 0x5f, 0x00, 0x00}, // !
    {0x00, 0x07, 0x00, 0x07, 0x00}, // "
    {0x14, 0x7f, 0x14, 0x7f, 0x14}, // #
    {0x24, 0x2a, 0x7f, 0x2a, 0x12}, // $
    // (Caso precise de mais letras completas, pode expandir a tabela, 
    // mas vamos incluir as letras maiúsculas básicas abaixo para teste)
    {0x7f, 0x08, 0x08, 0x08, 0x7f}, // H
    {0x3e, 0x41, 0x41, 0x41, 0x22}, // C
    {0x7f, 0x09, 0x09, 0x01, 0x7f}, // E
    {0x36, 0x49, 0x49, 0x49, 0x36}, // 0
    {0x00, 0x42, 0x7f, 0x40, 0x00}, // 1
    {0x42, 0x61, 0x51, 0x49, 0x46}, // 2
};

// Função simples para escrever uma palavra na linha/coluna desejada
void OLED_WriteString(uint8_t page, uint8_t col, const char* str) {
    OLED_Clear();

    OLED_WriteCommand(0x21); // Coluna
    OLED_WriteCommand(col);
    OLED_WriteCommand(127);

    OLED_WriteCommand(0x22); // Página (0 a 7)
    OLED_WriteCommand(page);
    OLED_WriteCommand(7);

    while (*str) {
        char c = *str++;
        uint8_t char_data[5] = {0};
        
        // Mapeamento simples para teste (exemplo com H, C, E, 0, 1, 2)
        int index = 0;
        if (c == 'H') index = 5;
        else if (c == 'C') index = 6;
        else if (c == 'E') index = 7;
        else if (c == '0') index = 8;
        else if (c == '1') index = 9;
        else if (c == '2') index = 10;
        
        for(int i=0; i<5; i++) {
            char_data[i] = Font5x7[index][i];
        }

        OLED_WriteData(char_data, 5);
        
        // Espaço de 1 pixel entre as letras
        uint8_t space = 0x00;
        OLED_WriteData(&space, 1);
    }
}