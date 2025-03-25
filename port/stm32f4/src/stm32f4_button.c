/**
 * @file stm32f4_button.c
 * @brief Portable functions to interact with the button FSM library. All portable functions must be implemented in this file.
 * @author Lucía Petit
 * @author Mateo Pansard
 * @date 25-3-2025
 */

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdint.h>
/* HW dependent includes */
#include <stddef.h> // Used to define NULL
#include "port_button.h" // Used to get general information about the buttons (ID, etc.)
#include "port_system.h" // Used to get the system tick

/* Microcontroller dependent includes */
// TO-DO alumnos: include the necessary files to interact with the GPIOs
#include "stm32f4_system.h" // Used to interact with the GPIOs
#include "stm32f4_button.h" // Used to interact with the button FSM library

/* Typedefs --------------------------------------------------------------------*/
/** @brief Structure to define the HW dependencies of a button */
typedef struct
{
    /** @brief GPIO where the button is connected */
    GPIO_TypeDef *p_port;
    /** @brief Pin/line where the button is connected */
    uint8_t pin;
    /** @brief Pull-up/pull-down mode of the button */
    uint8_t pupd_mode;
    /** @brief Flag to indicate that the button is pressed */
    bool flag_pressed;
} stm32f4_button_hw_t;

/* Global variables ------------------------------------------------------------*/
static stm32f4_button_hw_t buttons_arr[] = {
    [PORT_PARKING_BUTTON_ID] = {
        .p_port = STM32F4_PARKING_BUTTON_GPIO, 
        .pin = STM32F4_PARKING_BUTTON_PIN, 
        .pupd_mode = STM32F4_GPIO_PUPDR_NOPULL
    }
}; //DOCUMENTAR DOXYGEN
/* Private functions ----------------------------------------------------------*/
/**
 * @brief Get the button status struct with the given ID.
 *
 * @param button_id Button ID.
 *
 * @return Pointer to the button state struct.
 * @return NULL If the button ID is not valid.
 */
stm32f4_button_hw_t *_stm32f4_button_get(uint32_t button_id)
{
    // Return the pointer to the button with the given ID. If the ID is not valid, return NULL.
    if (button_id < sizeof(buttons_arr) / sizeof(buttons_arr[0]))
    {
        return &buttons_arr[button_id];
    }
    else
    {
        return NULL;
    }
}

/* Public functions -----------------------------------------------------------*/
void port_button_init(uint32_t button_id)
{
    // Retrieve the button struct using the private function and the button ID
    stm32f4_button_hw_t *p_button = _stm32f4_button_get(button_id);


    /* TO-DO alumnos */
    // 1. Configure the GPIO as input with no pull-up or pull-down
    stm32f4_system_gpio_config(p_button->p_port, p_button->pin, STM32F4_GPIO_MODE_IN, p_button->pupd_mode);
    // 2. Enable the external interruption of the GPIO
    stm32f4_system_gpio_config_exti(p_button->p_port, p_button->pin, STM32F4_TRIGGER_BOTH_EDGE | STM32F4_TRIGGER_ENABLE_INTERR_REQ);
    // 3. Enable the interruption of the GPIO
    stm32f4_system_gpio_exti_enable(p_button->pin, 1, 0);
}


void stm32f4_button_set_new_gpio(uint32_t button_id, GPIO_TypeDef *p_port, uint8_t pin)
{
    stm32f4_button_hw_t *p_button = _stm32f4_button_get(button_id);
    p_button->p_port = p_port;
    p_button->pin = pin;
}

bool port_button_get_pressed(uint32_t button_id)
{
    stm32f4_button_hw_t *p_button = _stm32f4_button_get(button_id);
    return p_button->flag_pressed;
}

bool port_button_get_value(uint32_t button_id)
{
    stm32f4_button_hw_t *p_button = _stm32f4_button_get(button_id);
    return stm32f4_system_gpio_read(p_button->p_port, p_button->pin);
}

void port_button_set_pressed(uint32_t button_id, bool pressed)
{
    stm32f4_button_hw_t *p_button = _stm32f4_button_get(button_id);
    p_button->flag_pressed = pressed;
}

bool port_button_get_pending_interrupt(uint32_t button_id)
{
    stm32f4_button_hw_t *p_button = _stm32f4_button_get(button_id);
    uint8_t mipin = p_button->pin;
    return (EXTI->PR & (1 << mipin));
    //return stm32f4_system_gpio_read(p_button->p_port, p_button->pin);
}

void port_button_clear_pending_interrupt(uint32_t button_id)
{
    stm32f4_button_hw_t *p_button = _stm32f4_button_get(button_id);
    stm32f4_system_gpio_write(p_button->p_port, p_button->pin, false);//Igual no es esta función?
}

void port_button_disable_interrupts(uint32_t button_id)
{
    stm32f4_button_hw_t *p_button = _stm32f4_button_get(button_id);
    stm32f4_system_gpio_exti_disable(p_button->pin);
}
