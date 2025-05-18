/**
 * @file stm32f4_display.c
 * @brief Portable functions to interact with the display system FSM library. All portable functions must be implemented in this file.
 * @author alumno1
 * @author alumno2
 * @date fecha
 */

/* Standard C includes */
#include <stdio.h>
#include <math.h>

/* HW dependent includes */
#include "port_display.h"
#include "port_system.h"

/* Microcontroller dependent includes */
#include "stm32f4_display.h"
#include "stm32f4_system.h"

/* Defines --------------------------------------------------------------------*/

/* Typedefs --------------------------------------------------------------------*/
/** @brief Structure to define the HW dependencies of a display 
 *
*/
typedef struct
{
    /** @brief GPIO where the RED LED is connected*/
    GPIO_TypeDef *p_port_red;  
    /** @brief Pin where the RED LED is connected*/
    uint8_t pin_red;         
    /** @brief GPIO where the GREEN LED is connected*/
    GPIO_TypeDef *p_port_green; 
    /** @brief Pin where the GREEN LED is connected*/
    uint8_t pin_green;      
    /** @brief GPIO where the BLUE LED is connected*/
    GPIO_TypeDef *p_port_blue; 
    /** @brief Pin where the BLUE LED is connected*/
    uint8_t pin_blue;       
} stm32f4_display_hw_t;

/* Global variables */
/** @brief Array of elements that represents the HW characteristics of the RGB LED of the display systems connected to the STM32F4 platform */
static stm32f4_display_hw_t displays_arr[] = {
    [PORT_REAR_PARKING_DISPLAY_ID] = {
        .p_port_red = STM32F4_REAR_PARKING_DISPLAY_RGB_R_GPIO, 
        .pin_red = STM32F4_REAR_PARKING_DISPLAY_RGB_R_PIN,
        .p_port_green = STM32F4_REAR_PARKING_DISPLAY_RGB_G_GPIO,
        .pin_green = STM32F4_REAR_PARKING_DISPLAY_RGB_G_PIN,
        .p_port_blue = STM32F4_REAR_PARKING_DISPLAY_RGB_B_GPIO,
        .pin_blue = STM32F4_REAR_PARKING_DISPLAY_RGB_B_PIN}};  

/* Private functions -----------------------------------------------------------*/
/**
 * @brief Get the display struct with the given ID. 
 * 
 * @param display_id Button ID.
 * @return stm32f4_display_hw_t* NULL If the display ID is not valid. 
 */
stm32f4_display_hw_t *_stm32f4_display_get(uint32_t display_id)
{
    // Return the pointer to the display with the given ID. If the ID is not valid, return NULL.
    // TO-DO alumnos
    if (display_id < sizeof(displays_arr) / sizeof(displays_arr[0]))
    {
        return &displays_arr[display_id];
    }
    else
    {
        return NULL;
    }
}

/* Public functions -----------------------------------------------------------*/

/**
 * @brief Configure the timer that controls the PWM of each one of the RGB LEDs of the display system. 
 * 
 * This function is called by the port_display_init() public function to configure the timer that controls the PWM of the RGB LEDs of the display.
 * 
 * @param display_id Display system identifier number.
 */
void _timer_pwm_config(uint32_t display_id)
{
    if (_stm32f4_display_get(display_id) != NULL)
    {
        RCC->APB1ENR |= RCC_APB1ENR_TIM4EN; // Enable TIM4 clock
        TIM4->CR1 &= ~TIM_CR1_CEN;          // Disable the TIM4 counter
        TIM4->CR1 |= TIM_CR1_ARPE;          // Enable auto-reload preload
        TIM4->CNT = 0;                      // Reset the counter
        TIM4->ARR = 0xFFFF;                 // Set the auto-reload value to maximum
        TIM4->PSC = 4;                      // Set the prescaler to 4

        TIM4->CCER &= ~TIM_CCER_CC1E; // Disable the output compare for channel 1
        //(TIM4->CCER &= ~TIM_CCER_CC2E; // Disable the output compare for channel 2 no es necesario
        TIM4->CCER &= ~TIM_CCER_CC3E; // Disable the output compare for channel 3
        TIM4->CCER &= ~TIM_CCER_CC4E; // Disable the output compare for channel 4

        TIM4->CCER &= ~TIM_CCER_CC1NP;
        //(TIM4->CCER &= ~TIM_CCER_CC2NP;
        TIM4->CCER &= ~TIM_CCER_CC3NP;
        TIM4->CCER &= ~TIM_CCER_CC4NP;

        TIM4->CCER &= ~TIM_CCER_CC1P;
        //(TIM4->CCER &= ~TIM_CCER_CC2P;
        TIM4->CCER &= ~TIM_CCER_CC3P;
        TIM4->CCER &= ~TIM_CCER_CC4P;

        TIM4->CCMR1 &= ~TIM_CCMR1_OC1M;
        TIM4->CCMR1 |= (TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2); // Set PWM mode 1 for channel 1

        TIM4->CCMR2 &= ~TIM_CCMR2_OC3M;
        TIM4->CCMR2 |= (TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2); // Set PWM mode 1 for channel 3

        TIM4->CCMR2 &= ~TIM_CCMR2_OC4M;
        TIM4->CCMR2 |= (TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2); // Set PWM mode 1 for channel 4

        TIM4->CCMR1 |= TIM_CCMR1_OC1PE; // Enable output compare preload for channel 1
        TIM4->CCMR2 |= TIM_CCMR2_OC3PE; // Enable output compare preload for channel 3
        TIM4->CCMR2 |= TIM_CCMR2_OC4PE; // Enable output compare preload for channel 4

        TIM4->EGR |= TIM_EGR_UG; // Generate an update event to load the new values
    }
}

void port_display_set_rgb(uint32_t display_id, rgb_color_t color)
{
    if (_stm32f4_display_get(display_id) != NULL)
    {
        if (display_id == PORT_REAR_PARKING_DISPLAY_ID)
        {
            TIM4->CR1 &= ~TIM_CR1_CEN; // Disable the timer by clearing the enable bit

            if (color.r == color.g && color.b == color.r && color.r == 0)
            {
                TIM4->CCER &= ~TIM_CCER_CC1E;
                TIM4->CCER &= ~TIM_CCER_CC3E;
                TIM4->CCER &= ~TIM_CCER_CC4E;
                return;
            }
            else
            {
                if (color.r == 0)
                {
                    TIM4->CCER &= ~TIM_CCER_CC1E; // Disable the output compare for channel 1
                }
                else
                {
                    TIM4->CCER |= TIM_CCER_CC1E;      // Enable the output compare for channel 1
                    double r = color.r * TIM4->ARR / 255;   
                    TIM4->CCR1 = round(r); // Set the duty cycle for channel 1
                }

                if (color.g == 0)
                {
                    TIM4->CCER &= ~TIM_CCER_CC3E; // Disable the output compare for channel 3
                }
                else
                {
                    TIM4->CCER |= TIM_CCER_CC3E;      // Enable the output compare for channel 3
                    double g = color.g * TIM4->ARR / 255;
                    TIM4->CCR3 = round(g); // Set the duty cycle for channel 3
                }

                if (color.b == 0)
                {
                    TIM4->CCER &= ~TIM_CCER_CC4E; // Disable the output compare for channel 4
                }
                else
                {
                    TIM4->CCER |= TIM_CCER_CC4E;      // Enable the output compare for channel 4
                    double b = color.b * TIM4->ARR / 255;
                    TIM4->CCR4 = round(b); // Set the duty cycle for channel 4
                }

                TIM4->EGR |= TIM_EGR_UG;  // Generate an update event to load the new values
                TIM4->CR1 |= TIM_CR1_CEN; // Enable the timer
                return;
            }
        }
        else
        {
            return;
        }
    }
}

void port_display_init(uint32_t display_id)
{
    // Retrieve the display struct using the private function and the display ID
    stm32f4_display_hw_t *p_display = _stm32f4_display_get(display_id);

    stm32f4_system_gpio_config(p_display->p_port_red, p_display->pin_red, STM32F4_GPIO_MODE_AF, STM32F4_GPIO_PUPDR_NOPULL);
    stm32f4_system_gpio_config(p_display->p_port_green, p_display->pin_green, STM32F4_GPIO_MODE_AF, STM32F4_GPIO_PUPDR_NOPULL);
    stm32f4_system_gpio_config(p_display->p_port_blue, p_display->pin_blue, STM32F4_GPIO_MODE_AF, STM32F4_GPIO_PUPDR_NOPULL);

    stm32f4_system_gpio_config_alternate(p_display->p_port_red, p_display->pin_red, STM32F4_AF2);
    stm32f4_system_gpio_config_alternate(p_display->p_port_green, p_display->pin_green, STM32F4_AF2);
    stm32f4_system_gpio_config_alternate(p_display->p_port_blue, p_display->pin_blue, STM32F4_AF2);

    _timer_pwm_config(display_id);
    port_display_set_rgb(display_id, COLOR_OFF);
}