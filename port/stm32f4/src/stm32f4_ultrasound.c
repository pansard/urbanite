/**
 * @file stm32f4_ultrasound.c
 * @brief Portable functions to interact with the ultrasound FSM library. All portable functions must be implemented in this file.
 * @author Lucia Petit
 * @author Mateo Pansard
 * @date 2025-03-21
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
/** @brief Structure to define the HW dependencies of aN ultrasound sensor*/

typedef struct
{
    /** @brief GPIO where the trigger signal is connected. */
    GPIO_TypeDef *p_trigger_port; 
    /** @brief GPIO where the echo signal is connected. */
    GPIO_TypeDef *p_echo_port;
    /** @brief Pin/line where the trigger signal is connected*/
    uint8_t trigger_pin;
    /** @brief Pin/line where the echo signal is connected */
    uint8_t echo_pin;
    /** @brief Alternate function for the echo signal*/
    uint8_t echo_alt_fun;
    /** @brief Flag to indicate that a new measurement can be started */
    bool trigger_ready;
    /** @brief Flag to indicate that the trigger signal has ended */
    bool trigger_end;
    /** @brief Flag to indicate that the echo signal has been received */
    bool echo_received;
    /** @briefTick time when the echo signal was received */
    uint32_t echo_init_tick;
    /** @brief Tick time when the echo signal ended */
    uint32_t echo_end_tick;
    /** @brief Number of overflows of the echo signal */
    uint32_t echo_overflows;
} stm32f4_ultrasound_hw_t;

/* Global variables */
/** @brief Array of elements that represents the HW characteristics of the ultrasounds connected to the STM32F4 platform.s */
static stm32f4_ultrasound_hw_t ultrasounds_arr[] = {
    [PORT_REAR_PARKING_SENSOR_ID] = {
        .p_trigger_port = STM32F4_REAR_PARKING_SENSOR_TRIGGER_GPIO,
        .p_echo_port = STM32F4_REAR_PARKING_SENSOR_ECHO_GPIO,
        .trigger_pin = STM32F4_REAR_PARKING_SENSOR_TRIGGER_PIN,
        .echo_pin = STM32F4_REAR_PARKING_SENSOR_ECHO_PIN,
    }};

/* Private functions ----------------------------------------------------------*/
/**
 * @brief Get the ultrasound struct with the given ID.
 *
 * @param ultrasound_id Ultrasound sensor ID.
 *
 * @return Pointer to the ultrasound sensor struct.
 * @return NULL If the ultrasound sensor ID is not valid.
 */
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
}

/**
 * @brief Configure the timer that controls the duration of the trigger signal.
 */
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

/**
 * @brief Configure the timer that controls the duration of the echo signal.
 */
static void _timer_echo_setup()
{
    // RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // enable clock for GPIOA
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // enable clock for TIM2

    TIM2->PSC = 15;     // prescaler
    TIM2->ARR = 0xFFFF; // auto reload

    TIM2->CR1 |= TIM_CR1_ARPE;                                        // enable auto reload preload
    TIM2->EGR |= TIM_EGR_UG;                                          // update generation
    TIM2->CCMR1 &= ~TIM_CCMR1_CC2S;                                   // poner el canal 2 en modo de entrada
    TIM2->CCMR1 |= (0x1 << TIM_CCMR1_CC2S_Pos);                       // filtro de entrada
    TIM2->CCER |= (1 << TIM_CCER_CC2P_Pos | 1 << TIM_CCER_CC2NP_Pos); // flanco de subida y bajada
    TIM2->CCMR1 &= ~TIM_CCMR1_IC2F;                                   // filtro de entrada
    TIM2->CCMR1 &= ~TIM_CCMR1_IC2PSC;                                 // prescaler a 0
    TIM2->CCER |= TIM_CCER_CC2E;                                      // habilitar el canal 2
    TIM2->DIER |= TIM_DIER_CC2IE;                                     // habilitar la interrupción del canal 2
    TIM2->DIER |= TIM_DIER_UIE;                                       // habilitar la interrupción de actualización
    // TIM2->SR = ~TIM_SR_UIF;                                                             // limpiar la bandera de actualización
    // TIM2->SR = ~TIM_SR_CC2IF;                                                           // limpiar la bandera de captura
    NVIC_SetPriority(TIM2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 3, 0)); // prioridad 3
}

/**
 * @brief Configure the timer that controls the duration of the new measurement.
 */
static void _timer_new_measurement_setup() // SI ALGO SALE MAL REVISAR
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
    TIM5->CR1 &= ~TIM_CR1_CEN;
    TIM5->CR1 |= TIM_CR1_ARPE;
    TIM5->CNT = 0;

    double f = 16000.0;
    double arr = 65535.0;
    double psc = f * PORT_PARKING_SENSOR_TIMEOUT_MS / (arr + 1.0) - 1.0;
    psc = round(psc);
    arr = f * PORT_PARKING_SENSOR_TIMEOUT_MS / (psc + 1.0) - 1.0;
    arr = round(arr);
    if (arr > 65535.0)
    {
        psc = psc + 1.0;
        arr = f * PORT_PARKING_SENSOR_TIMEOUT_MS / (psc + 1.0) - 1.0;
        arr = round(arr);
    }

    TIM5->PSC = (uint32_t)psc;
    TIM5->ARR = (uint32_t)arr;

    TIM5->EGR = TIM_EGR_UG;
    TIM5->SR = ~TIM_SR_UIF;
    TIM5->DIER |= TIM_DIER_UIE;
    NVIC_SetPriority(TIM5_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 5, 0));
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

    stm32f4_system_gpio_config(p_ultrasound->p_echo_port, p_ultrasound->echo_pin, STM32F4_GPIO_MODE_AF, STM32F4_GPIO_PUPDR_NOPULL);
    stm32f4_system_gpio_config_alternate(p_ultrasound->p_echo_port, p_ultrasound->echo_pin, STM32F4_AF1);
    _timer_echo_setup();

    _timer_new_measurement_setup();
}

void port_ultrasound_stop_trigger_timer(uint32_t ultrasound_id)
{
    stm32f4_ultrasound_hw_t *p_ultrasound = _stm32f4_ultrasound_get(ultrasound_id);
    stm32f4_system_gpio_write(p_ultrasound->p_trigger_port, p_ultrasound->trigger_pin, 0);
    TIM3->CR1 &= ~TIM_CR1_CEN;
    // TIM3->SR = ~TIM_SR_UIF; // clear update interrupt flag
    // NVIC_DisableIRQ(TIM3_IRQn); // disable timer
}

void port_ultrasound_stop_echo_timer(uint32_t ultrasound_id)
{
    if (ultrasound_id == PORT_REAR_PARKING_SENSOR_ID) // revisar condición
    {
        TIM2->CR1 &= ~TIM_CR1_CEN; // disable timer
    }
    // nuestro
    // TIM2->SR = ~TIM_SR_UIF;    // clear update interrupt flag
    // TIM2->SR = ~TIM_SR_CC2IF;  // clear capture/compare interrupt flag
    
    //NVIC_DisableIRQ(TIM2_IRQn); // disable timer
}

void port_ultrasound_reset_echo_ticks(uint32_t ultrasound_id)
{
    stm32f4_ultrasound_hw_t *p_ultrasound = _stm32f4_ultrasound_get(ultrasound_id);
    p_ultrasound->echo_init_tick = 0;
    p_ultrasound->echo_end_tick = 0;
    p_ultrasound->echo_overflows = 0;
    p_ultrasound->echo_received = false;
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

uint32_t port_ultrasound_get_echo_init_tick(uint32_t ultrasound_id)
{
    stm32f4_ultrasound_hw_t *p_ultrasound = _stm32f4_ultrasound_get(ultrasound_id);
    return p_ultrasound->echo_init_tick;
}

void port_ultrasound_set_echo_init_tick(uint32_t ultrasound_id, uint32_t echo_init_tick)
{
    stm32f4_ultrasound_hw_t *p_ultrasound = _stm32f4_ultrasound_get(ultrasound_id);
    p_ultrasound->echo_init_tick = echo_init_tick;
}

uint32_t port_ultrasound_get_echo_end_tick(uint32_t ultrasound_id)
{
    stm32f4_ultrasound_hw_t *p_ultrasound = _stm32f4_ultrasound_get(ultrasound_id);
    return p_ultrasound->echo_end_tick;
}

void port_ultrasound_set_echo_end_tick(uint32_t ultrasound_id, uint32_t echo_end_tick)
{
    stm32f4_ultrasound_hw_t *p_ultrasound = _stm32f4_ultrasound_get(ultrasound_id);
    p_ultrasound->echo_end_tick = echo_end_tick;
}

bool port_ultrasound_get_echo_received(uint32_t ultrasound_id)
{
    stm32f4_ultrasound_hw_t *p_ultrasound = _stm32f4_ultrasound_get(ultrasound_id);
    return p_ultrasound->echo_received;
}

void port_ultrasound_set_echo_received(uint32_t ultrasound_id, bool echo_received)
{
    stm32f4_ultrasound_hw_t *p_ultrasound = _stm32f4_ultrasound_get(ultrasound_id);
    p_ultrasound->echo_received = echo_received;
}

uint32_t port_ultrasound_get_echo_overflows(uint32_t ultrasound_id)
{
    stm32f4_ultrasound_hw_t *p_ultrasound = _stm32f4_ultrasound_get(ultrasound_id);
    return p_ultrasound->echo_overflows;
}

void port_ultrasound_set_echo_overflows(uint32_t ultrasound_id, uint32_t echo_overflows)
{
    stm32f4_ultrasound_hw_t *p_ultrasound = _stm32f4_ultrasound_get(ultrasound_id);
    p_ultrasound->echo_overflows = echo_overflows;
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

void port_ultrasound_start_measurement(uint32_t ultrasound_id)
{
    stm32f4_ultrasound_hw_t *p_ultrasound = _stm32f4_ultrasound_get(ultrasound_id);
    p_ultrasound->trigger_ready = false; // SI ALGO FALLA CAMBIAR

    TIM5->CNT = 0;

    if (ultrasound_id == PORT_REAR_PARKING_SENSOR_ID)
    {
        TIM3->CNT = 0;
        TIM2->CNT = 0;
       
    }

    stm32f4_system_gpio_write(p_ultrasound->p_trigger_port, p_ultrasound->trigger_pin, 1);

    NVIC_EnableIRQ(TIM3_IRQn);
    NVIC_EnableIRQ(TIM2_IRQn);
    NVIC_EnableIRQ(TIM5_IRQn);
    
    TIM2->CR1 |= TIM_CR1_CEN;
    TIM3->CR1 |= TIM_CR1_CEN;
    TIM5->CR1 |= TIM_CR1_CEN;
}

void port_ultrasound_start_new_measurement_timer()
{
    NVIC_EnableIRQ(TIM5_IRQn);
    TIM5->CR1 |= TIM_CR1_CEN;
}

void port_ultrasound_stop_new_measurement_timer()
{
    TIM5->CR1 &= ~TIM_CR1_CEN;
    // TIM5->SR = ~TIM_SR_UIF; // clear update interrupt flag
    // NVIC_DisableIRQ(TIM5_IRQn); // disable timer
}

void port_ultrasound_stop_ultrasound(uint32_t ultrasound_id)
{
    // ni idea hulio
    // NVIC_DisableIRQ(TIM5_IRQn);
    // NVIC_DisableIRQ(TIM2_IRQn);
    // NVIC_DisableIRQ(TIM3_IRQn);

    port_ultrasound_stop_trigger_timer(ultrasound_id);
    port_ultrasound_stop_echo_timer(ultrasound_id);
    port_ultrasound_stop_new_measurement_timer();
    port_ultrasound_reset_echo_ticks(ultrasound_id);
}