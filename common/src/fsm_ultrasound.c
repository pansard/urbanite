/**
 * @file fsm_ultrasound.c
 * @brief Ultrasound sensor FSM main file.
 * @author Lucía Petit
 * @author Mateo Pansard
 * @date 21/03/2025
 */

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdlib.h>
#include <string.h>

/* HW dependent includes */
#include "port_ultrasound.h"
#include "port_system.h"
#include "fsm_ultrasound.h"

/* Project includes */
#include "fsm.h"

/* Typedefs --------------------------------------------------------------------*/

/**
 * @brief Structure to define the Ultrasound FSM.
 *
 */
struct fsm_ultrasound_t
{
    fsm_t f;
    uint32_t distance_cm;
    bool status;
    bool new_measurement;
    uint32_t ultrasound_id;
    uint32_t distance_arr[FSM_ULTRASOUND_NUM_MEASUREMENTS];
    uint32_t distance_idx;
};

/* Private functions -----------------------------------------------------------*/

// Comparison function for qsort
int _compare(const void *a, const void *b)
{
    return (*(uint32_t *)a - *(uint32_t *)b);
}

/* State machine input or transition functions */

/**
 * @brief Check if the ultrasound sensor is active and ready to start a new measurement.
 *
 * @param p_this Pointer to an `fsm_t´ struct that contains an `fsm_ultrasound_t`.
 * @return true
 * @return false
 */
static bool check_on(fsm_t *p_this)
{
    if (port_ultrasound_get_trigger_ready(((fsm_ultrasound_t *)p_this)->ultrasound_id))
    {
        return true;
    }
    return false;
}

/**
 * @brief Check if the ultrasound sensor has been set to be inactive (OFF).
 *
 * @param p_this Pointer to an `fsm_t´ struct that contains an `fsm_ultrasound_t`.
 * @return true
 * @return false
 */
static bool check_off(fsm_t *p_this)
{
    bool stat = ((fsm_ultrasound_t *)p_this)->status;
    return !stat;
}

/**
 * @brief Check if the ultrasound sensor has finished the trigger signal.
 *
 * @param p_this Pointer to an `fsm_t´ struct that contains an `fsm_ultrasound_t`.
 * @return true
 * @return false
 */
static bool check_trigger_end(fsm_t *p_this)
{
    return port_ultrasound_get_trigger_end(((fsm_ultrasound_t *)p_this)->ultrasound_id);
}

/**
 * @brief Check if the ultrasound sensor has received the init (rising edge in the input capture) of the echo signal.
 *
 * @param p_this Pointer to an `fsm_t´ struct that contains an `fsm_ultrasound_t`.
 * @return true
 * @return false
 */
static bool check_echo_init(fsm_t *p_this)
{
    if (port_ultrasound_get_echo_init_tick(((fsm_ultrasound_t *)p_this)->ultrasound_id) > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * @brief Check if the ultrasound sensor has received the end (falling edge in the input capture) of the echo signal.
 *
 * @param p_this Pointer to an `fsm_t´ struct that contains an `fsm_ultrasound_t`.
 * @return true
 * @return false
 */
static bool check_echo_received(fsm_t *p_this)
{
    return port_ultrasound_get_echo_received(((fsm_ultrasound_t *)p_this)->ultrasound_id);
}

/**
 * @brief Check if a new measurement is ready.
 *
 * @param p_this Pointer to an `fsm_t´ struct that contains an `fsm_ultrasound_t`.
 * @return true
 * @return false
 */
static bool check_new_measurement(fsm_t *p_this)
{
    return port_ultrasound_get_trigger_ready(((fsm_ultrasound_t *)p_this)->ultrasound_id);
}

/* State machine output or action functions */

/**
 * @brief Start a measurement of the ultrasound transceiver for the first time after the FSM is started.
 *
 * @param p_this Pointer to an `fsm_t` struct that contains an `fsm_ultrasound_t`.
 */
static void do_start_measurement(fsm_t *p_this)
{
    port_ultrasound_start_measurement(((fsm_ultrasound_t *)p_this)->ultrasound_id);
}

/**
 * @brief Start a new measurement of the ultrasound transceiver.
 *
This function is called when the ultrasound sensor has finished a measurement and is ready to start a new one.
 *
 * @param p_this Pointer to an `fsm_t` struct that contains an `fsm_ultrasound_t`.
 */
static void do_start_new_measurement(fsm_t *p_this)
{
    do_start_measurement(p_this);
}

/**
 * @brief Stop the trigger signal of the ultrasound sensor.
 *
This function is called when the time to trigger the ultrasound sensor has finished. It stops the trigger signal and the trigger timer.
 *
 * @param p_this Pointer to an `fsm_t` struct that contains an `fsm_ultrasound_t`.
 */
static void do_stop_trigger(fsm_t *p_this)
{
    port_ultrasound_stop_trigger_timer(((fsm_ultrasound_t *)p_this)->ultrasound_id);
    port_ultrasound_set_trigger_end(((fsm_ultrasound_t *)p_this)->ultrasound_id, false);
}

/**
 * @brief Set the distance measured by the ultrasound sensor.
 *
 This function is called when the ultrasound sensor has received the echo signal. It calculates the distance in cm and stores it in the array of distances.
 *
 When the array is full, it computes the median of the array and resets the index of the array.
 *
 * @param p_this Pointer to an `fsm_t` struct that contains an `fsm_ultrasound_t`.
 */
static void do_set_distance(fsm_t *p_this)
{
    // port_ultrasound_reset_echo_ticks(((fsm_ultrasound_t *)p_this)->ultrasound_id); // echo signal cleared
    uint32_t init = port_ultrasound_get_echo_init_tick(((fsm_ultrasound_t *)p_this)->ultrasound_id);
    uint32_t echo_end = port_ultrasound_get_echo_end_tick(((fsm_ultrasound_t *)p_this)->ultrasound_id);
    uint32_t overflows = port_ultrasound_get_echo_overflows(((fsm_ultrasound_t *)p_this)->ultrasound_id);

    uint32_t t = overflows * 0xFFFFFFFF + echo_end - init;
    uint32_t distance = t * SPEED_OF_SOUND_MS / 2 / 10000;

    ((fsm_ultrasound_t *)p_this)->distance_arr[((fsm_ultrasound_t *)p_this)->distance_idx] = distance;
    if (((fsm_ultrasound_t *)p_this)->distance_idx >= FSM_ULTRASOUND_NUM_MEASUREMENTS - 1)
    {
        qsort(((fsm_ultrasound_t *)p_this)->distance_arr, FSM_ULTRASOUND_NUM_MEASUREMENTS, sizeof(uint32_t), _compare);
        if (FSM_ULTRASOUND_NUM_MEASUREMENTS % 2 == 0)
        {
            ((fsm_ultrasound_t *)p_this)->distance_cm = (((fsm_ultrasound_t *)p_this)->distance_arr[FSM_ULTRASOUND_NUM_MEASUREMENTS / 2 - 1] + ((fsm_ultrasound_t *)p_this)->distance_arr[FSM_ULTRASOUND_NUM_MEASUREMENTS / 2]) / 2;
        }
        else
        {
            ((fsm_ultrasound_t *)p_this)->distance_cm = ((fsm_ultrasound_t *)p_this)->distance_arr[FSM_ULTRASOUND_NUM_MEASUREMENTS / 2 - 1];
        }
        ((fsm_ultrasound_t *)p_this)->new_measurement = true;
    }
    // NO SABEMOS SI VA DENTRO DEL IF
    ((fsm_ultrasound_t *)p_this)->distance_idx = (((fsm_ultrasound_t *)p_this)->distance_idx + 1) % FSM_ULTRASOUND_NUM_MEASUREMENTS;
    port_ultrasound_stop_echo_timer(((fsm_ultrasound_t *)p_this)->ultrasound_id);
    port_ultrasound_reset_echo_ticks(((fsm_ultrasound_t *)p_this)->ultrasound_id);
}

/**
 * @brief Stop the ultrasound sensor. 
 * 
 This function is called when the ultrasound sensor is stopped. It stops the ultrasound sensor and resets the echo ticks.
 * 
 * @param p_this Pointer to an `fsm_t` struct that contains an `fsm_ultrasound_t`.
 */
static void do_stop_measurement(fsm_t *p_this)
{
    port_ultrasound_stop_ultrasound(((fsm_ultrasound_t *)p_this)->ultrasound_id);
}

/* FSM states */

/**
 * @brief Array representing the transitions table of the FSM ultrasound. 
 * 
 */
fsm_trans_t fsm_trans_ultrasound[] = {
    {WAIT_START, check_on, TRIGGER_START, do_start_measurement},
    {TRIGGER_START, check_trigger_end, WAIT_ECHO_START, do_stop_trigger},
    {WAIT_ECHO_START, check_echo_init, WAIT_ECHO_END, NULL},
    {WAIT_ECHO_END, check_echo_received, SET_DISTANCE, do_set_distance},
    {SET_DISTANCE, check_new_measurement, TRIGGER_START, do_start_new_measurement},
    {SET_DISTANCE, check_off, WAIT_START, do_stop_measurement},
    {-1, NULL, -1, NULL}};

/* Other auxiliary functions */
void fsm_ultrasound_init(fsm_ultrasound_t *p_fsm_ultrasound, uint32_t ultrasound_id)
{
    // Initialize the FSM
    fsm_init(&p_fsm_ultrasound->f, fsm_trans_ultrasound);

    /* TODO alumnos: */
    // Initialize the fields of the FSM structure
    p_fsm_ultrasound->distance_cm = 0;
    p_fsm_ultrasound->status = false;
    p_fsm_ultrasound->new_measurement = false;
    p_fsm_ultrasound->distance_idx = 0;
    p_fsm_ultrasound->ultrasound_id = ultrasound_id; // ESTO ARREGLA COSAS
    // memset(p_fsm_ultrasound->distance_arr, 0, sizeof(uint32_t) * FSM_ULTRASOUND_NUM_MEASUREMENTS);
    for (int i = 0; i < FSM_ULTRASOUND_NUM_MEASUREMENTS; i++)
    {
        p_fsm_ultrasound->distance_arr[i] = 0;
    }
    port_ultrasound_init(ultrasound_id);
}

/* Public functions -----------------------------------------------------------*/
fsm_ultrasound_t *fsm_ultrasound_new(uint32_t ultrasound_id)
{
    fsm_ultrasound_t *p_fsm_ultrasound = malloc(sizeof(fsm_ultrasound_t)); /* Do malloc to reserve memory of all other FSM elements, although it is interpreted as fsm_t (the first element of the structure) */
    fsm_ultrasound_init(p_fsm_ultrasound, ultrasound_id);                  /* Initialize the FSM */
    return p_fsm_ultrasound;
}

// Other auxiliary functions
void fsm_ultrasound_set_state(fsm_ultrasound_t *p_fsm, int8_t state)
{
    p_fsm->f.current_state = state;
}

void fsm_ultrasound_fire(fsm_ultrasound_t *p_fsm)
{
    fsm_fire(&p_fsm->f);
}

void fsm_ultrasound_destroy(fsm_ultrasound_t *p_fsm)
{
    free(&p_fsm->f);
}

fsm_t *fsm_ultrasound_get_inner_fsm(fsm_ultrasound_t *p_fsm)
{
    return &p_fsm->f;
}

uint32_t fsm_ultrasound_get_state(fsm_ultrasound_t *p_fsm)
{
    return p_fsm->f.current_state;
}

uint32_t fsm_ultrasound_get_distance(fsm_ultrasound_t *p_fsm)
{
    p_fsm->new_measurement = false; // revisar
    return p_fsm->distance_cm;
}

void fsm_ultrasound_stop(fsm_ultrasound_t *p_fsm)
{
    p_fsm->status = false; // revisar
    port_ultrasound_stop_ultrasound(p_fsm->ultrasound_id);
}

void fsm_ultrasound_start(fsm_ultrasound_t *p_fsm)
{
    p_fsm->status = true; // revisar
    p_fsm->distance_idx = 0;
    p_fsm->distance_cm = 0;
    port_ultrasound_reset_echo_ticks(p_fsm->ultrasound_id);
    port_ultrasound_set_trigger_ready(p_fsm->ultrasound_id, true);
    port_ultrasound_start_new_measurement_timer();
}

bool fsm_ultrasound_get_status(fsm_ultrasound_t *p_fsm)
{
    return p_fsm->status;
}

bool fsm_ultrasound_get_ready(fsm_ultrasound_t *p_fsm)
{
    return port_ultrasound_get_trigger_ready(p_fsm->ultrasound_id);
}

void fsm_ultrasound_set_status(fsm_ultrasound_t *p_fsm, bool status)
{
    p_fsm->status = status;
}

bool fsm_ultrasound_get_new_measurement_ready(fsm_ultrasound_t *p_fsm)
{
    return p_fsm->new_measurement;
}

bool fsm_ultrasound_check_activity(fsm_ultrasound_t *p_fsm)
{
    return false;
}