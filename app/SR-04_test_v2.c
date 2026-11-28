#include "stm32f103xx_init.h"
#include "stm32f103xx_exti.h"
#include "stm32f103xx_serial.h"
#include "stm32f103xx_sr04.h"

#define SAMPLE_INTERVAL_MS        1000U

GPIO_Handle trigger, echo;
sr04_t s1;

int main()
{
    board_init();
    SET_LOG_LEVEL(LOG_LEVEL_INFO);

    SR04_GPIO_Init(&trigger, &echo, GPIOB, GPIOA, GPIO_PIN0, GPIO_PIN7);
    SR04_Init(&s1);
    
    while(1)
	{
        SR04_ResetStates(&s1);
        SR04_Trigger(&trigger);
        SR04_Wait(&s1);
        Serialprintln("Distance (cm): %f", INFO, s1.distance_cm);
        led_flash(4, SAMPLE_INTERVAL_MS/(2*4));
        tim_delay_ms(SAMPLE_INTERVAL_MS/2);
    }
}


//Echo GPIO IRQ handler
void EXTI9_5_IRQHandler(void)
{
    //Clear the IRQ Pending
    EXTI_Pend_Clear(7);
    SR04_CalculateDistanceCM(&s1);
}