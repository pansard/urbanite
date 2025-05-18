/**
 * @file fsm_display.h
 * @brief Header for fsm_display.c file.
 * @author Lucia Petit
 * @author Mateo Pansard
 * @date 2025-03-9
 */

#ifndef FSM_DISPLAY_SYSTEM_H_
#define FSM_DISPLAY_SYSTEM_H_

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdint.h>
#include <stdbool.h>
#include "fsm.h"

/* Defines and enums ----------------------------------------------------------*/
/* Enums */
/**
 * @brief Enumerator for the display system finite state machine.
 */
enum FSM_DISPLAY_SYSTEM {
    WAIT_DISPLAY = 0,   /*!< Starting state. Also comes here when there is no status of display, i.e. the display system is inactive*/
    SET_DISPLAY , /*!< State to show the status of the display system*/
};

/* Defines and enums ----------------------------------------------------------*/
#define DANGER_MIN_CM 0 /*!< Minimum distance in cm for the danger state */
#define WARNING_MIN_CM 25     /*!<  Minimum distance in cm for the warning state */
#define NO_PROBLEM_MIN_CM 50    /*!<  Minimum distance in cm for the no problem state */
#define INFO_MIN_CM 150   /*!<  Minimum distance in cm for the info state */
#define OK_MIN_CM 175      /*!<  Minimum distance in cm for the ok state */
#define OK_MAX_CM 200       /*!<  Maximum distance in cm for the ok state */

/* Typedefs --------------------------------------------------------------------*/
/**
 * @brief Structure of the display FSM.
 */
typedef struct fsm_display_t fsm_display_t;

/* Function prototypes and explanation -------------------------------------------------*/

/**
 * @brief Create a new display FSM.
 * 
 *  This function creates a new display FSM with the given display ID.
 * @param display_id Display ID. Must be unique.

 * @return fsm_display_t*  Pointer to the display FSM.
 */
fsm_display_t * fsm_display_new (uint32_t display_id);

/**
 * @brief Delete a display FSM.
 * 
 * This function deletes the display FSM and frees the allocated memory.
 * @param p_fsm Pointer to the display FSM .
 */
void fsm_display_destroy (fsm_display_t * p_fsm);

/**
 * @brief Set the display system to show the distance in cm.
 *
 * This function is used to set the display system to show the distance in cm.
 * @param p_fsm Pointer to the display FSM.
 * @param distance_cm Distance in cm to be displayed.
 */
void fsm_display_set_distance (fsm_display_t *p_fsm, uint32_t distance_cm);

/**
 * @brief Fire the display FSM.
 *
 * Fire the display FSM.
 * @param p_fsm Pointer to the display FSM.
 */
void fsm_display_fire (fsm_display_t *p_fsm);

/**
 * @brief Get the display status.
 *
 * This function returns the display status.
 * @param p_fsm Pointer to the display FSM.
 * @return true if the display is active, false if paused.
 */
bool fsm_display_get_status (fsm_display_t *p_fsm);

/**
 * @brief Set the display status.
 *
 * This function sets the display status.Indicating if the display system is active or paused.
 * @param p_fsm Pointer to the display FSM.
 * @param pause Status of the display system. true if the display system is paused, false if the display system is active.
 */
void fsm_display_set_status (fsm_display_t *p_fsm, bool pause);

/**
 * @brief Check if the display system is active.
 *
 * This function checks if the display system is active.
 * @param p_fsm Pointer to the display FSM.
 * @return true if the display system is active, false if paused.
 */
bool fsm_display_check_activity (fsm_display_t *p_fsm);

/**
 * @brief Get the inner FSM of the display.
 *
 * This function returns the inner FSM of the display.
 * @param p_fsm Pointer to the display FSM.
 * @return Pointer to the inner FSM.
 */
fsm_t * fsm_display_get_inner_fsm (fsm_display_t *p_fsm);

/**
 * @brief Get the state of the display FSM.
 *
 * This function returns the state of the display FSM.
 * @param p_fsm Pointer to the display FSM.
 * @return State of the display FSM.
 */
uint32_t fsm_display_get_state (fsm_display_t *p_fsm);

/**
 * @brief Set the state of the display FSM.
 *
 * This function sets the state of the display FSM.
 * @param p_fsm Pointer to the display FSM.
 * @param state New state of the display FSM.
 */
void fsm_display_set_state (fsm_display_t *p_fsm, int8_t state);
 
#endif /* FSM_DISPLAY_SYSTEM_H_ */