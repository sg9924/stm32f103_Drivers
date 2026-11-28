#include "stm32f103xx_init.h"
#include "stm32f103xx_serial.h"


int main()
{
    board_init();
    SET_LOG_LEVEL(LOG_LEVEL_INFO);
    
    while(1)
	{
        led_toggle();
        Serialprintln("LED at Pin C13 is being toggled!", INFO);
        tim_delay_ms(1000);
    }
}