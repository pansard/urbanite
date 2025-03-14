/**
 * @file fsm_ultrasound.h
 * @brief Header for fsm_ultrasound.c file.
 * @author alumno1
 * @author alumno2
 * @date fecha
 */

#ifndef FSM_ULTRASOUND_H_
#define FSM_ULTRASOUND_H_

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdint.h>
#include <stdbool.h>

/* Other includes */
#include "fsm.h"

/* Defines and enums ----------------------------------------------------------*/
#define FSM_ULTRASOUND_NUM_MEASUREMENTS 5

enum FSM_ULTRASOUND {
    WAIT_START = 0,
    TRIGGER_START,
    WAIT_ECHO_START,
    WAIT_ECHO_END,
    SET_DISTANCE
};

/* Typedefs --------------------------------------------------------------------*/
typedef struct fsm_ultrasound_t fsm_ultrasound_t;

/* Function prototypes and explanation -------------------------------------------------*/


/**
 * @brief Set the state of the ultrasound FSM.
 *
 * This function sets the current state of the ultrasound FSM.
 *
 * > &nbsp;&nbsp;&nbsp;&nbsp;💡 This function is important because the struct is private and external functions such as those of the unit tests cannot access the state of the FSM directly. \n
 * 
 * @param p_fsm Pointer to an `fsm_ultrasound_t` struct.
 * @param state New state of the ultrasound FSM.
 */
void fsm_ultrasound_set_state(fsm_ultrasound_t *p_fsm, int8_t state);

fsm_ultrasound_t * fsm_ultrasound_new(uint32_t ultrasound_id);

void fsm_ultrasound_destroy(fsm_ultrasound_t *p_fsm);

uint32_t fsm_ultrasound_get_distance(fsm_ultrasound_t *p_fsm);

void fsm_ultrasound_fire(fsm_ultrasound_t *p_fsm);

bool fsm_ultrasound_get_status(fsm_ultrasound_t *p_fsm);

void fsm_ultrasound_set_status(fsm_ultrasound_t *p_fsm, bool status);

bool fsm_ultrasound_get_ready(fsm_ultrasound_t *p_fsm);

bool fsm_ultrasound_get_new_measurement_ready(fsm_ultrasound_t *p_fsm);

void fsm_ultrasound_stop(fsm_ultrasound_t *p_fsm);

void fsm_ultrasound_start(fsm_ultrasound_t *p_fsm);

fsm_t * fsm_ultrasound_get_inner_fsm(fsm_ultrasound_t *p_fsm);

uint32_t fsm_ultrasound_get_state(fsm_ultrasound_t *p_fsm);

bool fsm_ultrasound_check_activity(fsm_ultrasound_t *p_fsm);

#endif /* FSM_ULTRASOUND_H_ */
