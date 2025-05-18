/**
 * @file stm32f4_ultrasound.c
 * @brief Portable functions to interact with the ultrasound FSM library. All portable functions must be implemented in this file.
 * @author Lucia Petit
 * @author Mateo Pansard
 * @date 2025-05-18
 */

#ifndef FSM_URBANITE_H_
#define FSM_URBANITE_H_

#include <stdint.h>
#include "fsm_button.h"
#include "fsm_ultrasound.h"
#include "fsm_display.h"

enum FSM_URBANITE {
    OFF = 0,
    MEASURE,
    SLEEP_WHILE_OFF,
    SLEEP_WHILE_ON
};

typedef struct fsm_urbanite_t fsm_urbanite_t;

fsm_urbanite_t * fsm_urbanite_new (fsm_button_t *p_fsm_button, uint32_t on_off_press_time_ms, uint32_t pause_display_time_ms, fsm_ultrasound_t *p_fsm_ultrasound_rear, fsm_display_t *p_fsm_display_rear);
void fsm_urbanite_fire (fsm_urbanite_t *p_fsm);
void fsm_urbanite_destroy (fsm_urbanite_t *p_fsm);

#endif /* FSM_URBANITE_H_ */