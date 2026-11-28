#include "stm32f103xx_init.h"
#include "stm32f103xx_serial.h"
#include "stm32f103xx_exti.h"
#include "stm32f103xx_nvic.h"
#include "stm32f103xx_dwt.h"

GPIO_Handle G1, G2;
volatile float    distance_cm = 0;
volatile uint32_t start       = 0;
volatile uint32_t diff        = 0;
volatile bool     data_rcv    = false;

typedef enum edge_state
{
    RISING  = 0,
    FALLING = 1
}edge_state_t;

volatile edge_state_t s = RISING;

int main()
{
    board_init();
    DWT_init();
    SET_LOG_LEVEL(LOG_LEVEL_INFO);

    //GPIO - A7 for receiving the Echo Pulse
    GPIO_Config(&G1, GPIOA, GPIO_MODE_INTRPT_FERE_TRIG, GPIO_CONFIG_PD, GPIO_PIN7, GPIO_OP_SPEED_0);
    GPIO_Init(&G1);

    //GPIO - B0 for sending the Trigger Pulse
    GPIO_Config(&G2, GPIOB, GPIO_MODE_OP, GPIO_CONFIG_GP_OP_PP, GPIO_PIN0, GPIO_OP_SPEED_50);
    GPIO_Init(&G2);

    //Enable GPIO7 IRQ in NVIC
    nvic_intrpt_enable(nvic_irq_gpio_port_map(&G1));
    //Enable GPIO7 IRQ in EXTI for falling and rising edge
    EXTI_Intrpt_Config(GPIO_PIN7, GPIO_MODE_INTRPT_FERE_TRIG, ENABLE);
    
    while(1)
	{
        DISABLE_IRQ();
        //Reset states
        s        = RISING;
        data_rcv = false;
        ENABLE_IRQ();

        //Send 10us pulse
        GPIO_BIT_SET(G2.pGPIOx, G2.GPIOx_PinConfig.PinNo);
        tim_delay_us(10);
        GPIO_BIT_RESET(G2.pGPIOx, G2.GPIOx_PinConfig.PinNo);

        //wait for data
        while(!data_rcv);

        //Print distance
        if(data_rcv)
        {
            Serialprintln("Distance (cm): %f", INFO, distance_cm);
            data_rcv = false;
        }

        tim_delay_ms(2000);
    }
}


//A7 - Input
void EXTI9_5_IRQHandler(void)
{
    //Clear the IRQ Pending
    EXTI_Pend_Clear(7);

    //Rising Edge
    if(s == RISING)
    {
        start    = DWT_GetCount();
        diff     = 0;
        data_rcv = false;

        s = FALLING;
    }
    //Falling Edge
    else if(s == FALLING)
    {
        diff        = (uint32_t)(DWT_GetCount() - start);
        distance_cm = (diff/(72.0f * 58.0f));
        start       = 0;
        data_rcv    = true;

        s = RISING;
    }
}