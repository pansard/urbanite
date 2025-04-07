/**
 * @file stm32f4_display.h
 * @brief Header for stm32f4_display.c file.
 * @author alumno1
 * @author alumno2
 * @date fecha
 */
#ifndef STM32F4_DISPLAY_SYSTEM_H_
#define STM32F4_DISPLAY_SYSTEM_H_

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdint.h>

/* HW dependent includes */
#include "stm32f4xx.h"

/* Defines and enums ----------------------------------------------------------*/
/* Defines */
#define STM32F4_REAR_PARKING_DISPLAY_RGB_R_GPIO GPIOB
#define STM32F4_REAR_PARKING_DISPLAY_RGB_R_PIN 6
#define STM32F4_REAR_PARKING_DISPLAY_RGB_G_GPIO GPIOB
#define STM32F4_REAR_PARKING_DISPLAY_RGB_G_PIN 8
#define STM32F4_REAR_PARKING_DISPLAY_RGB_B_GPIO GPIOB
#define STM32F4_REAR_PARKING_DISPLAY_RGB_B_PIN 9

#endif /* STM32F4_DISPLAY_SYSTEM_H_ */