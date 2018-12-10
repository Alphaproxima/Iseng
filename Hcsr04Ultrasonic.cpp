#include "Hcsr04Ultrasonic.h"
#include "main.h"

/************************* Local Variables **********************/
static Hcsr04InitConfig_t config[HCSR04_NUM_CHANNEL] = {0};


/************************* Local Function Prototypes ************/
static void init_capture_port(TIM_TypeDef *timer_module, uint8_t rising_capture_ch);
static uint32_t get_pulse_width_us(uint8_t channel);


/************************* Public Functions *********************/
Hcsr04InitStatus_t hcsr04_driver_init(uint8_t channel, Hcsr04InitConfig_t *init_config)
{
    //Pulse width measurement only available for channel 1 & 2
    if(((init_config->echo_capture_timer_channel != 1) &&
       (init_config->echo_capture_timer_channel != 2)) ||
       (channel >= HCSR04_NUM_CHANNEL))
    {
        return HCSR04_INIT_FAILURE;   
    }

    init_capture_port(init_config->echo_capture_timer_port, init_config->echo_capture_timer_channel);
    config[channel] = *init_config;
    
    return HCSR04_INIT_SUCCESS;
}

int16_t hcsr04_measure_range_cm(uint8_t channel, uint32_t timeout_ms)
{
    if(channel >= HCSR04_NUM_CHANNEL)
    {
        return HCSR04_INCORRECT_CHANNEL;
    }

    uint16_t timer_event_flag = config[channel].echo_capture_timer_channel == 1 ? TIM_FLAG_CC2 : TIM_FLAG_CC1;

    // Activate trigger
    GPIO_SetBits(config[channel].trigger_gpio_port, 0x01 << config[channel].trigger_gpio_pin);

    uint32_t prev_tick = tick;

    // Waiting echo
    while ((tick - prev_tick) < timeout_ms)
    {
        // Turn off trigger
        if (tick > prev_tick)
        {
            GPIO_ResetBits(config[channel].trigger_gpio_port, 0x01 << config[channel].trigger_gpio_pin);
        }

        if (TIM_GetFlagStatus(config[channel].echo_capture_timer_port, timer_event_flag))
        {
            //obtain the value of CCRx
            return get_pulse_width_us(channel) / 58;
        }
    }

    return HCSR04_RANGING_TIMEOUT;
}

/************************* Private Functions *********************/
void init_capture_port(TIM_TypeDef *timer_module, uint8_t rising_capture_ch)
{
    //Input Capture Init, using a pair of capture channel connected to single pin
    TIM_ICInitTypeDef ICInitChannel1;
    TIM_ICInitTypeDef ICInitChannel2;

    TIM_ICStructInit(&ICInitChannel1);
    ICInitChannel1.TIM_Channel = rising_capture_ch == 1 ? TIM_Channel_1 : TIM_Channel_2;
    ICInitChannel1.TIM_ICPolarity = TIM_ICPolarity_Rising;
    ICInitChannel1.TIM_ICSelection = TIM_ICSelection_DirectTI;
    ICInitChannel1.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    ICInitChannel1.TIM_ICFilter = 0;
    TIM_ICInit(timer_module, &ICInitChannel1);

    TIM_ICStructInit(&ICInitChannel2);
    ICInitChannel2.TIM_Channel = rising_capture_ch == 1 ? TIM_Channel_2 : TIM_Channel_1;
    ICInitChannel2.TIM_ICPolarity = TIM_ICPolarity_Falling;
    ICInitChannel2.TIM_ICSelection = TIM_ICSelection_IndirectTI;
    ICInitChannel2.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    ICInitChannel2.TIM_ICFilter = 0;
    TIM_ICInit(timer_module, &ICInitChannel2);

    // Trigger source and mode
    TIM_SelectInputTrigger(timer_module, rising_capture_ch == 1 ? TIM_TS_TI1FP1 : TIM_TS_TI2FP2);
    TIM_SelectSlaveMode(timer_module, TIM_SlaveMode_Reset);
}

uint32_t get_pulse_width_us(uint8_t channel)
{
    uint32_t pulse_ticks = 0;

    switch (config[channel].echo_capture_timer_channel)
    {
    case 1:
        pulse_ticks = TIM_GetCapture2(config[channel].echo_capture_timer_port);
        break;

    case 2:
        pulse_ticks = TIM_GetCapture1(config[channel].echo_capture_timer_port);
        break;
    }

    return pulse_ticks * 1e6 / config[channel].clock_speed_hz;
}
