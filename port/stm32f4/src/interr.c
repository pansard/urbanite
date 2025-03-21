/**
 * @file interr.c
 * @brief Interrupt service routines for the STM32F4 platform.
 * @author SDG2. Román Cárdenas (r.cardenas@upm.es) and Josué Pagán (j.pagan@upm.es)
 * @date 2025-01-01
 */

// Include HW dependencies:
#include "stm32f4_system.h"
#include "stm32f4_button.h"
#include "stm32f4_ultrasound.h"

// Include headers of different port elements:
#include "port_system.h"
#include "port_button.h"
#include "port_ultrasound.h"

//------------------------------------------------------
// INTERRUPT SERVICE ROUTINES
//------------------------------------------------------
/**
 * @brief Interrupt service routine for the System tick timer (SysTick).
 *
 * @note This ISR is called when the SysTick timer generates an interrupt.
 * The program flow jumps to this ISR and increments the tick counter by one millisecond.
 *
 * > **TO-DO alumnos:**
 * >
 * > ✅ 1. **Increment the System tick counter `msTicks` in 1 count.** To do so, use the function `port_system_get_millis()` and `port_system_set_millis()`.
 *
 * @warning **The variable `msTicks` must be declared volatile!** Just because it is modified by a call of an ISR, in order to avoid [*race conditions*](https://en.wikipedia.org/wiki/Race_condition). **Added to the definition** after *static*.
 *
 */
void SysTick_Handler(void)
{
    uint32_t milli;

    milli = port_system_get_millis();
    port_system_set_millis(milli + 1);
}

/**
 * @brief This function handles Px10-Px15 global interrupts.
 *
 First, this function identifies the line/ pin which has raised the interruption. Then, perform the desired action. Before leaving it cleans the interrupt pending register.
 *
 */
void EXTI15_10_IRQHandler(void)
{
    // ISR parking button
    if (port_button_get_pending_interrupt(PORT_PARKING_BUTTON_ID))
    {
        bool gpio_user = port_button_get_value(PORT_PARKING_BUTTON_ID);
        if (gpio_user)
        {
            port_button_set_pressed(PORT_PARKING_BUTTON_ID, false); // no presionado
        }
        else
        {
            port_button_set_pressed(PORT_PARKING_BUTTON_ID, true); // presionado
        }
        port_button_clear_pending_interrupt(PORT_PARKING_BUTTON_ID);
    }
    EXTI->PR |= BIT_POS_TO_MASK(PORT_PARKING_BUTTON_ID);
}

/**
 * @brief Interrupt service routine for the TIM3 timer.

This timer controls the duration of the trigger signal of the ultrasound sensor. When the interrupt occurs it means that the time of the trigger signal has expired and must be lowered.
 *
 */
void TIM3_IRQHandler(void)
{
    TIM3->SR &= ~TIM_SR_UIF;
    port_ultrasound_set_trigger_end(PORT_REAR_PARKING_SENSOR_ID, true);
}

/**
 * @brief Interrupt service routine for the TIM2 timer.
 *
This timer controls the duration of the echo signal of the ultrasound sensor by means of the input capture mode.
 *
 */
void TIM2_IRQHandler(void)
{
    uint32_t overflows = port_ultrasound_get_echo_overflows(PORT_REAR_PARKING_SENSOR_ID);
    uint32_t echo_init_tick = port_ultrasound_get_echo_init_tick(PORT_REAR_PARKING_SENSOR_ID);
    uint32_t echo_end_tick = port_ultrasound_get_echo_end_tick(PORT_REAR_PARKING_SENSOR_ID);

    if (TIM2->SR & TIM_SR_UIF)
    {
        TIM2->SR &= ~TIM_SR_UIF;
        overflows++;
        port_ultrasound_set_echo_overflows(PORT_REAR_PARKING_SENSOR_ID, overflows);
    }

    if ((TIM2->SR & TIM_SR_CC2IF) != 0)
    {
        if (echo_init_tick == 0 && echo_end_tick == 0)
        {
            port_ultrasound_set_echo_init_tick(PORT_REAR_PARKING_SENSOR_ID, TIM2->CCR2);
        }
        else
        {
            port_ultrasound_set_echo_end_tick(PORT_REAR_PARKING_SENSOR_ID, TIM2->CCR2);
            port_ultrasound_set_echo_received(PORT_REAR_PARKING_SENSOR_ID, true);
        }
    }
}

/**
 * @brief Interrupt service routine for the TIM5 timer. 
 * 
 This timer controls the duration of the measurements of the ultrasound sensor. When the interrupt occurs it means that the time of the a measurement has expired and a new measurement can be started.
 * 
 */
void TIM5_IRQHandler(void)
{
    TIM5->SR &= ~TIM_SR_UIF;
    port_ultrasound_set_trigger_ready(PORT_REAR_PARKING_SENSOR_ID, true);
}