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

/**
 * @brief Configure the HW specifications of a given button.
 * 
 * @param button_id Button ID. This index is used to select the element of the buttons_arr[] array
 */
void port_button_init (uint32_t button_id);

/**
 * @brief Get the status of a button (pressed or not).
 * 
 * @param button_id Button ID. This index is used to get the correct button status struct.
 * @return bool True if the button has been pressed, false otherwise
 */
bool port_button_get_pressed (uint32_t button_id);

/**
 * @brief Get the value of the GPIO connected to the button
 * 
 * @param button_id Button ID. This index is used to select the element of the buttons_arr[] array
 * @return bool true or false depending on the value of the GPIO
 */
bool port_button_get_value (uint32_t button_id);

/**
 * @brief Set the status of a button (pressed or not).
 * 
 * @param button_id Button ID. This index is used to get the correct button struct of the buttons_arr[] array.
 * @param pressed status of the button
 */
void port_button_set_pressed (uint32_t button_id, bool pressed);

/**
 * @brief Get the status of the interrupt line connected to the button.
 * 
 * @param button_id Button ID. This index is used to select the element of the buttons_arr[] array
 */
bool port_button_get_pending_interrupt (uint32_t button_id);

/**
 * @brief Clear the pending interrupt of the button
 * 
 * @param button_id Button ID. This index is used to select the element of the buttons_arr[] array
 */
void port_button_clear_pending_interrupt (uint32_t button_id);

/**
 * @brief Disable the interrupts of the button
 * 
 * @param button_id Button ID. This index is used to select the element of the buttons_arr[] array
 */
void port_button_disable_interrupts (uint32_t button_id);

#endif