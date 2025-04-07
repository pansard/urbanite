/**
 * @file fsm_display.h
 * @brief Header for fsm_display.c file.
 * @author alumno1
 * @author alumno2
 * @date fecha
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
enum FSM_DISPLAY_SYSTEM {
    WAIT_DISPLAY = 0,
    SET_DISPLAY
};

/* Defines and enums ----------------------------------------------------------*/
#define DANGER_MIN_CM 0
#define WARNING_MIN_CM 25
#define NO_PROBLEM_MIN_CM 50
#define INFO_MIN_CM 150
#define OK_MIN_CM 175
#define OK_MAX_CM 200

/* Typedefs --------------------------------------------------------------------*/
typedef struct fsm_display_t fsm_display_t;

/* Function prototypes and explanation -------------------------------------------------*/

#endif /* FSM_DISPLAY_SYSTEM_H_ */