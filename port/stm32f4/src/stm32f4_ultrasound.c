/**
 * @file stm32f4_ultrasound.c
 * @brief Portable functions to interact with the ultrasound FSM library. All portable functions must be implemented in this file.
 * @author alumno1
 * @author alumno2
 * @date date
 */

/* Standard C includes */
#include <stdio.h>
#include <math.h>

/* HW dependent includes */
#include "port_ultrasound.h"
#include "port_system.h"

/* Microcontroller dependent includes */
#include "stm32f4_system.h"
#include "stm32f4_ultrasound.h"

/* Typedefs --------------------------------------------------------------------*/
typedef struct
{
    GPIO_TypeDef *p_trigger_port;
    GPIO_TypeDef *p_echo_port;
    uint8_t trigger_pin;
    uint8_t echo_pin;
    uint8_t echo_alt_fun;
    bool trigger_ready;
    bool trigger_end;
    bool echo_received;
    uint32_t echo_init_tick;
    uint32_t echo_end_tick;
    uint32_t echo_overflows;
} stm32f4_ultrasound_hw_t;

/* Global variables */
static stm32f4_ultrasound_hw_t ultrasounds_arr[] = {
    [PORT_REAR_PARKING_SENSOR_ID] = {
        .p_trigger_port = STM32F4_REAR_PARKING_SENSOR_TRIGGER_GPIO,
        .p_echo_port = STM32F4_REAR_PARKING_SENSOR_ECHO_GPIO,
        .trigger_pin = STM32F4_REAR_PARKING_SENSOR_TRIGGER_PIN,
        .echo_pin = STM32F4_REAR_PARKING_SENSOR_ECHO_PIN,
    }};

/* Private functions ----------------------------------------------------------*/
stm32f4_ultrasound_hw_t *_stm32f4_ultrasound_get(uint32_t ultrasound_id)
{
    if (ultrasound_id < sizeof(ultrasounds_arr) / sizeof(ultrasounds_arr[0]))
    {
        return &ultrasounds_arr[ultrasound_id];
    }
    else
    {
        return NULL;
    }

    /*
    if (button_id < sizeof(buttons_arr) / sizeof(buttons_arr[0]))
    {
        return &buttons_arr[button_id];
    }
    else
    {
        return NULL;
    }*/
}

static void _timer_trigger_setup()
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    TIM3->CR1 &= ~TIM_CR1_CEN;
    TIM3->CR1 |= TIM_CR1_ARPE;
    TIM3->CNT = 0;

    double f = 16.0;
    double arr = 65535.0;
    double psc = f * PORT_PARKING_SENSOR_TRIGGER_UP_US / (arr + 1.0) - 1.0;
    psc = round(psc);
    arr = f * PORT_PARKING_SENSOR_TRIGGER_UP_US / (psc + 1.0) - 1.0;
    arr = round(arr);
    if (arr > 65535.0)
    {
        psc = psc + 1.0;
        arr = f * PORT_PARKING_SENSOR_TRIGGER_UP_US / (psc + 1.0) - 1.0;
        arr = round(arr);
    }

    TIM3->PSC = (uint32_t)psc;
    TIM3->ARR = (uint32_t)arr;
    TIM3->EGR = TIM_EGR_UG;
    TIM3->SR = ~TIM_SR_UIF;
    TIM3->DIER |= TIM_DIER_UIE;
    NVIC_SetPriority(TIM3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 4, 0));
}

static void _timer_echo_setup() 
{
    RCC -> AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    double f = 1.0;
    double arr = 65535.0;
    double psc = f * 1 / (arr + 1.0) - 1.0;
    psc = round(psc);
    arr = f * 1 / (psc + 1.0) - 1.0;
    arr = round(arr);
    if (arr > 65535.0)
    {
        psc = psc + 1.0;
        arr = f * 1 / (psc + 1.0) - 1.0;
        arr = round(arr);
    }

    TIM2->PSC = (uint32_t)psc;
    TIM2->ARR = (uint32_t)arr;

    TIM2->CR1 |= TIM_CR1_ARPE;
    TIM2->EGR |= TIM_EGR_UG;
    //TIM2->CCMR1 |= TIM_CCMR1_CC1S_0;
    TIM2->CCMR1 &= ~TIM_CCMR1_CC2S;
    TIM2->CCMR1 |= (0x1 << TIM_CCMR1_CC2S_Pos);//HUELE A QUE ESTÁ MAL
}

/* Public functions -----------------------------------------------------------*/
void port_ultrasound_init(uint32_t ultrasound_id)
{
    /* Get the ultrasound sensor */
    stm32f4_ultrasound_hw_t *p_ultrasound = _stm32f4_ultrasound_get(ultrasound_id);

    /* TO-DO alumnos: */

    /* Trigger pin configuration */
    p_ultrasound->trigger_end = false;
    p_ultrasound->trigger_ready = true;

    /* Echo pin configuration */
    p_ultrasound->echo_received = false;
    p_ultrasound->echo_alt_fun = 0;
    p_ultrasound->echo_overflows = 0;
    p_ultrasound->echo_end_tick = 0;
    p_ultrasound->echo_init_tick = 0;

    /* Configure timers */
    stm32f4_system_gpio_config(p_ultrasound->p_trigger_port, p_ultrasound->trigger_pin, STM32F4_GPIO_MODE_OUT, STM32F4_GPIO_PUPDR_NOPULL);
    _timer_trigger_setup();

}

void port_ultrasound_stop_trigger_timer(uint32_t ultrasound_id)
{
    stm32f4_ultrasound_hw_t *p_ultrasound = _stm32f4_ultrasound_get(ultrasound_id);
    stm32f4_system_gpio_write(p_ultrasound->p_trigger_port, p_ultrasound->trigger_pin, 0);
    TIM3->CR1 &= ~TIM_CR1_CEN;
}



// Getters and setters functions
bool port_ultrasound_get_trigger_ready(uint32_t ultrasound_id)
{
    stm32f4_ultrasound_hw_t *p_ultrasound = _stm32f4_ultrasound_get(ultrasound_id);
    return p_ultrasound->trigger_ready;
}

void port_ultrasound_set_trigger_ready(uint32_t ultrasound_id, bool trigger_ready)
{
    stm32f4_ultrasound_hw_t *p_ultrasound = _stm32f4_ultrasound_get(ultrasound_id);
    p_ultrasound->trigger_ready = trigger_ready;
}

bool port_ultrasound_get_trigger_end(uint32_t ultrasound_id)
{
    stm32f4_ultrasound_hw_t *p_ultrasound = _stm32f4_ultrasound_get(ultrasound_id);
    return p_ultrasound->trigger_end;
}

void port_ultrasound_set_trigger_end(uint32_t ultrasound_id, bool trigger_end)
{
    stm32f4_ultrasound_hw_t *p_ultrasound = _stm32f4_ultrasound_get(ultrasound_id);
    p_ultrasound->trigger_end = trigger_end;
}

// Util
void stm32f4_ultrasound_set_new_trigger_gpio(uint32_t ultrasound_id, GPIO_TypeDef *p_port, uint8_t pin)
{
    stm32f4_ultrasound_hw_t *p_ultrasound = _stm32f4_ultrasound_get(ultrasound_id);
    p_ultrasound->p_trigger_port = p_port;
    p_ultrasound->trigger_pin = pin;
}

void stm32f4_ultrasound_set_new_echo_gpio(uint32_t ultrasound_id, GPIO_TypeDef *p_port, uint8_t pin)
{
    stm32f4_ultrasound_hw_t *p_ultrasound = _stm32f4_ultrasound_get(ultrasound_id);
    p_ultrasound->p_echo_port = p_port;
    p_ultrasound->echo_pin = pin;
}