/**
 * @file fsm_ultrasound.c
 * @brief Ultrasound sensor FSM main file.
 * @author alumno1
 * @author alumno2
 * @date fecha
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
struct fsm_ultrasound_t {
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

static bool check_on(fsm_t *p_this) 
{
    if (port_ultrasound_get_trigger_ready(((fsm_ultrasound_t *)p_this)->ultrasound_id))
    {
        return true;
    }
    return false;
}

static bool check_off(fsm_t *p_this) 
{
    
}

static bool check_trigger_end(fsm_t *p_this) 
{

}

static bool check_echo_init(fsm_t *p_this) 
{

}

static bool check_echo_received(fsm_t *p_this) 
{

}

static bool check_new_measurement(fsm_t *p_this) 
{

}

/* State machine output or action functions */

static void do_start_measurement(fsm_t *p_this) 
{

}

static void do_start_new_measurement(fsm_t *p_this) 
{

}

static void do_stop_trigger(fsm_t *p_this) 
{

}

static void do_set_distance(fsm_t *p_this) 
{

}

static void do_stop_measurement(fsm_t *p_this) 
{

}

/* Other auxiliary functions */
void fsm_ultrasound_init(fsm_ultrasound_t *p_fsm_ultrasound, uint32_t ultrasound_id)
{
    // Initialize the FSM
    fsm_init(&p_fsm_ultrasound->f, fsm_trans_ultrasound);

    /* TODO alumnos: */
    // Initialize the fields of the FSM structure
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