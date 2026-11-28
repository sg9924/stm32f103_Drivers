#include"stm32f103xx_sr04.h"
#include"stm32f103xx_nvic.h"
#include"stm32f103xx_exti.h"
#include"stm32f103xx_init.h"


static volatile uint32_t start = 0;       //Start DWT cycles count


void SR04_GPIO_Init(GPIO_Handle* ptrigger, GPIO_Handle* pecho, GPIO_RegDef* trigger_gpio_port, GPIO_RegDef* echo_gpio_port, uint8_t trigger_pin, uint8_t echo_pin)
{
    //Initialize DWT if not already done
    if(DWT_CHECK() != 1)
        DWT_init();
    
    //GPIO for receiving the Echo Pulse
    GPIO_Config(pecho, echo_gpio_port, GPIO_MODE_INTRPT_FERE_TRIG, GPIO_CONFIG_PD, echo_pin, GPIO_OP_SPEED_0);
    GPIO_Init(pecho);

    //GPIO for sending the Trigger Pulse
    GPIO_Config(ptrigger, trigger_gpio_port, GPIO_MODE_OP, GPIO_CONFIG_GP_OP_PP, trigger_pin, GPIO_OP_SPEED_50);
    GPIO_Init(ptrigger);

    //Enable Interrupt for the Echo Pulse GPIO
    //Enable GPIO IRQ in NVIC
    nvic_intrpt_enable(nvic_irq_gpio_port_map(pecho));
    //Enable GPIO IRQ in EXTI for falling and rising edge
    EXTI_Intrpt_Config(pecho->GPIOx_PinConfig.PinNo, GPIO_MODE_INTRPT_FERE_TRIG, ENABLE);
}



//Get Distance in cm
void SR04_CalculateDistanceCM(sr04_t* pS)
{
    //Rising Edge
    if(pS->state == RISING)
    {
        start = DWT_GetCount();
        pS->distance_cm = 0;
        pS->data_flag = false;
        pS->state = FALLING;
    }
    //Falling Edge
    else if(pS->state == FALLING)
    {
        pS->distance_cm = ((uint32_t)(DWT_GetCount() - start)/(72.0f * 58.0f));
        pS->data_flag = true;
        pS->state = RISING;
    }
}



void SR04_Init(sr04_t* pS)
{
    pS->data_flag   = false;
    pS->distance_cm = 0;
    pS->state       = RISING;
}



//Send Trigger Pulse
void SR04_Trigger(GPIO_Handle* ptrigger)
{
    //Send 10us pulse
    GPIO_BIT_SET(ptrigger->pGPIOx, ptrigger->GPIOx_PinConfig.PinNo);
    tim_delay_us(SR04_TRIGGER_DUR_US);
    GPIO_BIT_RESET(ptrigger->pGPIOx, ptrigger->GPIOx_PinConfig.PinNo);
}



void SR04_Wait(sr04_t* pS)
{
    //wait for data
    while(!pS->data_flag);

    //Reset flag
    pS->data_flag = false;
}