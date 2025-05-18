/**
 * @file fsm_button.c
 * @brief Button FSM main file.
 * @author Mateo Pansard
 * @author Lucia Petit
 * @date 2025-03-9
 */

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdlib.h>
/* HW dependent includes */
#include "port_button.h"
#include "port_system.h"

/* Project includes */
#include "fsm_button.h"
#include "fsm.h"

/**
 * @brief Structure of the Button FSM.
*/
struct fsm_button_t {
    /** @brief Base struct for FSMs */
    fsm_t f; 
    /** @brief Debounce time in ms */
    uint32_t debounce_time_ms; 
    /** @brief Next timeout for the FSM */
    uint32_t next_timeout; 
    /** @brief Tick when the button was pressed  */
    uint32_t tick_pressed;
    /** @brief Duration of the button pressed */
    uint32_t duration; 
    /** @brief Button ID */
    uint32_t button_id; 
};

/* State machine input or transition functions */

/**
 * @brief Checks wether the button has been pressed or not.
 * 
 * @param p_this 
 * @return true 
 * @return false 
 */
static bool check_button_pressed(fsm_t * p_this)
{
    return port_button_get_pressed(((fsm_button_t *)p_this)->button_id);
}

/**
 * @brief Checks if the button has been released or not.
 * 
 * @param p_this 
 * @return true 
 * @return false 
 */
static bool check_button_released(fsm_t * p_this)
{
    return !port_button_get_pressed(((fsm_button_t *)p_this)->button_id);
}

/**
 * @brief Checks if the debounce time has been reached.
 * 
 * @param p_this 
 * @return true 
 * @return false 
 */
static bool check_timeout(fsm_t * p_this)
{
    return port_system_get_millis() >= ((fsm_button_t *)p_this)->next_timeout;
}

/* State machine output or action functions */

/**
 * @brief Stores the tick when the button was pressed.
 * 
 * @param p_this 
 */
static void do_store_tick_pressed(fsm_t * p_this)
{
    fsm_button_t * estado = ((fsm_button_t *)p_this);
    estado -> tick_pressed = port_system_get_millis();
    estado -> next_timeout = port_system_get_millis() + estado->debounce_time_ms;
}

/**
 * @brief Stores the duration of the button press.
 * 
 * @param p_this 
 */
static void do_set_duration(fsm_t * p_this)
{
    fsm_button_t * estado = ((fsm_button_t *)p_this);
    estado->duration = port_system_get_millis() - estado->tick_pressed;
    estado->next_timeout = port_system_get_millis() + estado->debounce_time_ms;
}

/**
 * @brief Array of transitions for the button FSM.
 * 
 * @image html docs/assets/imgs/fsm_button.png "Texto alternativo"
 */
static fsm_trans_t fsm_trans_button[] = {
    { BUTTON_RELEASED, check_button_pressed, BUTTON_PRESSED_WAIT, do_store_tick_pressed },
    { BUTTON_PRESSED_WAIT, check_timeout, BUTTON_PRESSED, NULL},
    { BUTTON_PRESSED, check_button_released, BUTTON_RELEASED_WAIT, do_set_duration},
    { BUTTON_RELEASED_WAIT, check_timeout, BUTTON_RELEASED, NULL},
    { -1, NULL, -1, NULL }
};

/* Other auxiliary functions */
uint32_t fsm_button_get_duration(fsm_button_t *p_fsm)
{
    return p_fsm->duration;
}

void fsm_button_reset_duration(fsm_button_t *p_fsm)
{
    p_fsm->duration = 0;
}

uint32_t fsm_button_get_debounce_time_ms(fsm_button_t *p_fsm)
{
    return p_fsm->debounce_time_ms;
}

/**
 * @brief Initializes the button FSM.
 * 
 * @param p_fsm_button 
 * @param debounce_time 
 * @param button_id 
 */
static void fsm_button_init(fsm_button_t *p_fsm_button, uint32_t debounce_time, uint32_t button_id)
{
    fsm_init(&p_fsm_button->f, fsm_trans_button);

    /* TODO alumnos: */
    /* Initialize the FSM with the proper parameters */
    p_fsm_button->debounce_time_ms = debounce_time;
    p_fsm_button->button_id = button_id;
    p_fsm_button->tick_pressed = 0;
    p_fsm_button->duration = 0;
    p_fsm_button->next_timeout = 0;
    port_button_init(button_id);
}

/* Public functions -----------------------------------------------------------*/
fsm_button_t *fsm_button_new(uint32_t debounce_time_ms, uint32_t button_id)
{
    fsm_button_t *p_fsm_button = malloc(sizeof(fsm_button_t)); /* Do malloc to reserve memory of all other FSM elements, although it is interpreted as fsm_t (the first element of the structure) */
    fsm_button_init(p_fsm_button, debounce_time_ms, button_id);   /* Initialize the FSM */
    return p_fsm_button;                                       /* Composite pattern: return the fsm_t pointer as a fsm_button_t pointer */
}

/* FSM-interface functions. These functions are used to interact with the FSM */
void fsm_button_fire(fsm_button_t *p_fsm)
{
    fsm_fire(&p_fsm->f);
    //fsm_fire((fsm_t *)p_fsm);
}

void fsm_button_destroy(fsm_button_t *p_fsm)
{
    free(&p_fsm->f);
}

fsm_t *fsm_button_get_inner_fsm(fsm_button_t *p_fsm)
{
    return &p_fsm->f;
}

uint32_t fsm_button_get_state(fsm_button_t *p_fsm)
{
    return p_fsm->f.current_state;
}

bool fsm_button_check_activity(fsm_button_t *p_fsm)
{
    if (p_fsm->f.current_state == BUTTON_RELEASED){
        return false;
    } else {
        return true;
    }
}