void ultrasonicInit(uint8_t sensor_ch, uint64_t max_pulse_us)
{
    //Obtain timer clock base frequency, check tech documentation p.217
    RCC_ClocksTypeDef RCC_Clocks;
    RCC_GetClocksFreq (&RCC_Clocks);
    uint32_t tim_freq = (RCC_Clocks.HCLK_Frequency / RCC_Clocks.PCLK2_Frequency) == 1 ? 
                        RCC_Clocks.PCLK2_Frequency : RCC_Clocks.PCLK2_Frequency * 2;

    //calculate required ARR and prescaler given max pulse width
    uint64_t arr_val = tim_freq / 1e6 * max_pulse_us;
    uint32_t prescaler = (arr_val / 0xFFFF);

    if(prescaler != 0)
    {
        arr_val = 0xFFFF;
        prescaler--;
    }

    //Trigger Pin Config
    GPIO_InitTypeDef GPIO_InitDef;
    GPIO_InitDef.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitDef.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitDef.GPIO_OType = GPIO_OType_PP;
    GPIO_InitDef.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitDef.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitDef);

    //Timer for Echo Init
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = arr_val;
    TIM_TimeBaseStructure.TIM_Prescaler = prescaler;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM9, &TIM_TimeBaseStructure);
    TIM_Cmd(TIM9, ENABLE);

    //Echo Pin Config
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStruct);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource5, GPIO_AF_TIM9);

    Hcsr04InitConfig_t hcsr04_config;
    hcsr04_config.trigger_gpio_port = GPIOE;
    hcsr04_config.trigger_gpio_pin = 6;
    hcsr04_config.echo_capture_timer_port = TIM9;
    hcsr04_config.echo_capture_timer_channel = 1;
    hcsr04_config.clock_speed_hz = tim_freq / prescaler;

    Hcsr04InitStatus_t status = hcsr04_driver_init(sensor_ch, &hcsr04_config);
}

int main()
{

    uint8_t channel = 0;
    int16_t distance;
    unt64_t max_pulse_us = 12e4;
    
    ultrasonicInit(channel, max_pulse_us);

    while(true)
    {
        distance = hcsr04_measure_range_cm(channel, 200);
    }
}