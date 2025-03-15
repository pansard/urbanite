/**
 * @file fsm_button.h
 * @brief Header for fsm_button.c file.
 * @author Mateo Pansard
 * @author Lucia Petit
 * @date 2025-03-9
 */

#ifndef FSM_BUTTON_H_
#define FSM_BUTTON_H_

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdint.h>
#include <stdbool.h>

/* Other includes */
#include "fsm.h"

/* Defines and enums ----------------------------------------------------------*/
/* Enums */

/**
 * @brief Enumerator for the button finite state machine.

This enumerator defines the different states that the button finite state machine can be in. Each state represents a specific condition or step in the button press process.
 
| Enumerator| |
| --------- | --------- | 
| @ref BUTTON_RELEASED | Starting state. Also comes here when the button has been released  |
| @ref BUTTON_RELEASED_WAIT | State to perform the anti-debounce mechanism for a falling edge  |
| @ref BUTTON_PRESSED | State while the button is being pressed  |
| @ref BUTTON_PRESSED_WAIT | State to perform the anti-debounce mechanism for a rising edge  |

*/
enum FSM_BUTTON
{
    BUTTON_RELEASED = 0,  /**< @brief Starting state. Also comes here when the button has been released. */
    BUTTON_RELEASED_WAIT,      /**< @brief State to perform the anti-debounce mechanism for a falling edge. */
    BUTTON_PRESSED,            /**< @brief State while the button is being pressed. */
    BUTTON_PRESSED_WAIT        /**< @brief State to perform the anti-debounce mechanism for a rising edge. */
};

/* Typedefs --------------------------------------------------------------------*/
/**
 * @brief Structure of the button FSM.
 * 
 */
typedef struct fsm_button_t fsm_button_t;

/* Function prototypes and explanation -------------------------------------------------*/

/**
 * @brief Create a new button FSM.
 * 
 * @param debounce_time_ms
 * @param button_id
 * @return fsm_button_t* 
 */
fsm_button_t *fsm_button_new(uint32_t debounce_time_ms, uint32_t button_id);

/**
 * @brief Delete a button FSM.
 * 
 * @param p_fsm 
 */
void fsm_button_destroy(fsm_button_t *p_fsm);

/**
 * @brief Fire the button FSM.
 * 
 * @param p_fsm 
 */
void fsm_button_fire(fsm_button_t *p_fsm);

/**
 * @brief Get the FSM of the button.
 * 
 * @param p_fsm 
 * @return fsm_t* 
 */
fsm_t *fsm_button_get_inner_fsm(fsm_button_t *p_fsm);

/**
 * @brief Get the state of the button FSM.
 * 
 * @param p_fsm 
 * @return uint32_t 
 */
uint32_t fsm_button_get_state(fsm_button_t *p_fsm);

/**
 * @brief Get the duration of the latest button press.
 * 
 * @param p_fsm 
 * @return uint32_t 
 */
uint32_t fsm_button_get_duration(fsm_button_t *p_fsm);

/**
 * @brief Reset the duration of the latest button press.
 * 
 * @param p_fsm 
 */
void fsm_button_reset_duration(fsm_button_t *p_fsm);

/**
 * @brief Get the debounce time of the button.
 * 
 * @param p_fsm 
 * @return uint32_t 
 */
uint32_t fsm_button_get_debounce_time_ms(fsm_button_t *p_fsm);

/**
 * @brief Check wether the button FSM is active or not.
 * 
 * @param p_fsm 
 * @return true
 * @return false
 */
bool fsm_button_check_activity(fsm_button_t *p_fsm);

#endif