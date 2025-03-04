/**
 * @file port_button.h
 * @brief Header for the portable functions to interact with the HW of the buttons. The functions must be implemented in the platform-specific code.
 * @author Lucia Petit
 * @author Mateo Pansard
 * @date fecha
 */

#ifndef PORT_BUTTON_H_
#define PORT_BUTTON_H_

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdint.h>
#include <stdbool.h>

/* Defines and enums ----------------------------------------------------------*/
/* Defines */
#define PORT_PARKING_BUTTON_ID 0 /*!< ID of the parking button */
#define PORT_PARKING_BUTTON_DEBOUNCE_TIME_MS 150 /*!< User button identifier that represents the rear button */

/* Function prototypes and explanation -------------------------------------------------*/

//DOCUMENTAR TODO DOXYGEN

void port_button_init (uint32_t button_id);

bool port_button_get_pressed (uint32_t button_id);

bool port_button_get_value (uint32_t button_id);

void port_button_set_pressed (uint32_t button_id, bool pressed);

bool port_button_get_pending_interrupt (uint32_t button_id);

void port_button_clear_pending_interrupt (uint32_t button_id);

void port_button_disable_interrupts (uint32_t button_id);

#endif