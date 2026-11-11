#include "sensors.h"
#include "config.h"

static uint32_t sensor_samples[ULTRASONIC_SAMPLES];
static uint8_t sample_cursor = 0;
static uint32_t last_sample_timestamp = 0;

void sensors_init(void){
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitStruct.Pin = ULTRASONIC_TRIG_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(ULTRASONIC_TRIG_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = ULTRASONIC_ECHO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(ULTRASONIC_ECHO_PORT, &GPIO_InitStruct);

    sample_cursor = 0;
    last_sample_timestamp = 0;
    for (uint8_t i = 0; i < ULTRASONIC_SAMPLES; i++) sensor_samples[i] = 0;

    sensors_set_servo_angle(0);
}

void sensors_trigger_ultrasonic(void){
    HAL_GPIO_WritePin(ULTRASONIC_TRIG_PORT, ULTRASONIC_TRIG_PIN, GPIO_PIN_RESET);
    for(volatile uint32_t i = 0; i < 10; i++);
    HAL_GPIO_WritePin(ULTRASONIC_TRIG_PORT, ULTRASONIC_TRIG_PIN, GPIO_PIN_SET);
    for(volatile uint32_t i = 0; i < 72; i++);
    HAL_GPIO_WritePin(ULTRASONIC_TRIG_PORT, ULTRASONIC_TRIG_PIN, GPIO_PIN_RESET);
}

uint32_t sensors_get_filtered_distance(void){
    uint32_t current_tick = HAL_GetTick();

    if ((current_tick - last_sample_timestamp) >= SAMPLE_INTERVAL_MS){
        last_sample_timestamp = current_tick;

        sensors_trigger_ultrasonic();

        while(HAL_GPIO_ReadPin(ULTRASONIC_ECHO_PORT, ULTRASONIC_ECHO_PIN) == GPIO_PIN_RESET);
        uint32_t start_time = HAL_GetTick();
        
        while(HAL_GPIO_ReadPin(ULTRASONIC_ECHO_PORT, ULTRASONIC_ECHO_PIN) == GPIO_PIN_SET);
        uint32_t end_time = HAL_GetTick();

        uint32_t duration = end_time - start_time;
        uint32_t raw_distance = (duration * 343) / 2000;

        if (raw_distance == 0 || raw_distance > 400) raw_distance = 400;

        sensor_samples[sample_cursor] = raw_distance;
        sample_cursor = (sample_cursor + 1) % ULTRASONIC_SAMPLES;
    }

    uint32_t cumulative_sum = 0;
    for (uint8_t i = 0; i < ULTRASONIC_SAMPLES; i++) cumulative_sum += sensor_samples[i];

    distanciaObjeto = cumulative_sum / ULTRASONIC_SAMPLES;
    return distanciaObjeto;
}

void sensors_set_servo_angle(int16_t angle){
    uint32_t pwm_compare_value = 1500;

    if (angle == -90) pwm_compare_value = 1000;
    else if (angle == 90) pwm_compare_value = 2000;

    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, pwm_compare_value);
}

void sensors_process(void) {
    (void)sensors_get_filtered_distance();
}