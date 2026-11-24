#ifndef INC_stm32F103xx_DEBUG_H
#define INC_stm32F103xx_DEBUG_H


#include"stm32f103xx_core.h"
#include<stdint.h>

#define CORE_DEBUG_BASE_ADDR          (SCB_BASE_ADDR + 0xF0U)


typedef struct
{
    volatile uint32_t DHCSR;    /*<Debug Halting Control and Status Register>*/
    volatile uint32_t DCRSR;    /*<Debug Core Register Selector Register>*/
    volatile uint32_t DCRDR;    /*<Debug Core Register Data Register>*/
    volatile uint32_t DEMCR;    /*<Debug Exception and Monitor Control Register>*/
}CoreDebug_RegDef;


#define COREDEBUG                             ((CoreDebug_RegDef*) CORE_DEBUG_BASE_ADDR)

//DEMCR
#define COREDEBUG_DEMCR_VC_CORERESET          0
#define COREDEBUG_DEMCR_VC_MMER               4
#define COREDEBUG_DEMCR_VC_NOCPERR            5
#define COREDEBUG_DEMCR_VC_CHKERR             6
#define COREDEBUG_DEMCR_VC_STATERR            7
#define COREDEBUG_DEMCR_VC_BUSERR             8
#define COREDEBUG_DEMCR_VC_INTERR             9
#define COREDEBUG_DEMCR_VC_HARDERR            10
#define COREDEBUG_DEMCR_MON_EN                16
#define COREDEBUG_DEMCR_MON_PEND              17
#define COREDEBUG_DEMCR_MON_SETP              18
#define COREDEBUG_DEMCR_MON_REQ               19
#define COREDEBUG_DEMCR_TRCENA                24




//Macros
//TRACENA must be set to use DWT, ITM, ETM, TPIU
#define COREDEBUG_TRACE_ENABLE()              COREDEBUG->DEMCR |= (1 << COREDEBUG_DEMCR_TRCENA)
#define COREDEBUG_TRACE_DISABLE()             COREDEBUG->DEMCR &= ~(1 << COREDEBUG_DEMCR_TRCENA)





#endif