#ifndef HCSR04_DRIVER_H_
#define HCSR04_DRIVER_H_

#include "stm32f4xx.h"

#define HCSR04_NUM_CHANNEL (1)

#define HCSR04_RANGING_TIMEOUT (-1)
#define HCSR04_INCORRECT_CHANNEL (-2)

typedef enum Hcsr04InitStatus_t
{
    HCSR04_INIT_FAILURE = 0,
    HCSR04_INIT_SUCCESS = 1
} Hcsr04InitStatus_t;

typedef struct
{
    GPIO_TypeDef* trigger_gpio_port;
    uint16_t trigger_gpio_pin;
    TIM_TypeDef* echo_capture_timer_port;
    uint8_t echo_capture_timer_channel;
    uint64_t clock_speed_hz;
} Hcsr04InitConfig_t;

Hcsr04InitStatus_t hcsr04_driver_init(uint8_t channel, Hcsr04InitConfig_t* init_config);
int16_t hcsr04_measure_range_cm(uint8_t channel, uint32_t timeout_ms);

#endif
