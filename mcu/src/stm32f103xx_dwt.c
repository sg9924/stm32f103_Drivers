#include"stm32f103xx_dwt.h"
#include"stm32f103xx_debug.h"


void DWT_init()
{
    //Trace Enable
    //Set TRACENA in DEMCR in Core Debug
    COREDEBUG_TRACE_ENABLE();

    //Reset DWT CYCCNT
    DWT_CYCCNT_RESET();

    //Enable DWT to start cycle count
    DWT_CYCCNT_ENABLE();
}




uint32_t DWT_GetCount()
{
    return DWT->CYCCNT;
}