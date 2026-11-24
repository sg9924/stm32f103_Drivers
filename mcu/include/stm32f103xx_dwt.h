#ifndef INC_stm32F103xx_DWT_H
#define INC_stm32F103xx_DWT_H

#include"stm32f103xx_core.h"
#include"stm32f103xx.h"
#include<stdint.h>


typedef struct
{
    volatile uint32_t CTRL;
    volatile uint32_t CYCCNT;
    volatile uint32_t CPICNT;
    volatile uint32_t EXCCNT;
    volatile uint32_t SLEEPCNT;
    volatile uint32_t LSUCNT;
}DWT_RegDef;

//DWT Peripheral
#define DWT                          ((DWT_RegDef*) DWT_BASE_ADDR)




//DWT Register Definitions
#define DWT_CCYCENA                  0





//Macros
#define DWT_CYCCNT_RESET()           (DWT->CYCCNT = 0)
#define DWT_CYCCNT_ENABLE()          SET_BIT(DWT->CTRL, DWT_CCYCENA)
#define DWT_CYCCNT_DISABLE()         RESET_BIT(DWT->CTRL, DWT_CCYCENA)



//APIs
void DWT_init();
uint32_t DWT_GetCount();

#endif