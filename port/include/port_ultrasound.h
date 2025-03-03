/**
 * @file port_ultrasound.h
 * @brief Header for the portable functions to interact with the HW of the ultrasound sensors. The functions must be implemented in the platform-specific code.
 * @author alumno1
 * @author alumno2
 * @date fecha
 */
#ifndef PORT_ULTRASOUND_H_
#define PORT_ULTRASOUND_H_

/* Includes ------------------------------------------------------------------*/

/* Standard C includes */
#include <stdint.h>
#include <stdbool.h>

/* Defines and enums ----------------------------------------------------------*/
#define PORT_REAR_PARKING_SENSOR_ID 0
#define PORT_PARKING_SENSOR_TRIGGER_UP_US 10
#define PORT_PARKING_SENSOR_TIMEOUT_MS 100
#define SPEED_OF_SOUND_MS 343

/* MAS MIERDA DEL OTRO ARCHIVO
#define FSM_ULTRASOUND_NUM_MEASUREMENTS 5

typedef struct fsm_ultrasound_t fsm_ultrasound_t;

enum FSM_ULTRASOUND {
    WAIT_START = 0,
    TRIGGER_START,
    WAIT_ECHO_START,
    WAIT_ECHO_END,
    SET_DISTANCE
};*/

/* Function prototypes and explanation -------------------------------------------------*/
void port_ultrasound_init(uint32_t ultrasound_id);

void port_ultrasound_start_measurement(uint32_t ultrasound_id);

void port_ultrasound_stop_trigger_timer(uint32_t ultrasound_id);

void port_ultrasound_stop_echo_timer(uint32_t ultrasound_id);

void port_ultrasound_start_new_measurement_timer(void);

void port_ultrasound_stop_new_measurement_timer(void);

void port_ultrasound_reset_echo_ticks(uint32_t ultrasound_id);

void port_ultrasound_stop_ultrasound(uint32_t ultrasound_id);

bool port_ultrasound_get_trigger_ready(uint32_t ultrasound_id);

void port_ultrasound_set_trigger_ready(uint32_t ultrasound_id, bool trigger_ready);

bool port_ultrasound_get_trigger_end(uint32_t ultrasound_id);

void port_ultrasound_set_trigger_end(uint32_t ultrasound_id, bool trigger_end);

uint32_t port_ultrasound_get_echo_init_tick(uint32_t ultrasound_id);

void port_ultrasound_set_echo_init_tick(uint32_t ultrasound_id, uint32_t echo_init_tick);

uint32_t port_ultrasound_get_echo_end_tick(uint32_t ultrasound_id);

void port_ultrasound_set_echo_end_tick(uint32_t ultrasound_id, uint32_t echo_end_tick);

bool port_ultrasound_get_echo_received(uint32_t ultrasound_id);

void port_ultrasound_set_echo_received(uint32_t ultrasound_id, bool echo_received);

uint32_t port_ultrasound_get_echo_overflows(uint32_t ultrasound_id);

void port_ultrasound_set_echo_overflows(uint32_t ultrasound_id, uint32_t echo_overflows);

// ESTA MIERDA VA EN FSM_ULTRASOUND.H
/*
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

void fsm_ultrasound_set_state(fsm_ultrasound_t *p_fsm, int8_t state);

bool fsm_ultrasound_check_activity(fsm_ultrasound_t *p_fsm);
*/
#endif /* PORT_ULTRASOUND_H_ */
