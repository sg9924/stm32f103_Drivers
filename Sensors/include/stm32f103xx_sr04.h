#ifndef INC_STM32F103XX_SR_04_H
#define INC_STM32F103XX_SR_04_H


#include<stdint.h>

#include"stm32f103xx_dwt.h"
#include"stm32f103xx_gpio.h"
#include"stm32f103xx_utilities.h"

#define SR04_MEASURE_TIMEOUT_MS           (DWT_MS_TO_CYCLES(40))  //40 ms timeout
#define SR04_MIN_VALID_CM                 2.0f                    //sensor's lower range limit: 2cm
#define SR04_MAX_VALID_CM                 400.0f                  //sensor's upper range limit: 400 cm
#define SR04_SAMPLE_SPACING_MIN_MS        60                      //minimum wait between consecutive samples
#define SR04_TRIGGER_DUR_US               10

typedef enum edge_state
{
    RISING  = 0,
    FALLING = 1
}sr04_edgestate_t;


typedef struct sr04
{
    sr04_edgestate_t state;
    bool             data_flag;
    float            distance_cm;
}sr04_t;



void SR04_GPIO_Init(GPIO_Handle* ptrigger, GPIO_Handle* pecho, GPIO_RegDef* trigger_gpio_port, GPIO_RegDef* echo_gpio_port, uint8_t trigger_pin, uint8_t echo_pin);
void SR04_Init(sr04_t* pS);
void SR04_Trigger(GPIO_Handle* ptrigger);
void SR04_Wait(sr04_t* pS);
void SR04_CalculateDistanceCM(sr04_t* pS);



static inline void SR04_ResetStates(sr04_t* pS)
{
    DISABLE_IRQ();
    pS->state     = RISING;
    pS->data_flag = false;
    ENABLE_IRQ();
}



#endif