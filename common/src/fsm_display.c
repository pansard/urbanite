/**
 * @file fsm_display.c
 * @brief Display system FSM main file.
 * @author alumno1
 * @author alumno2
 * @date fecha
 */

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdlib.h>
#include <stdio.h>
/* HW dependent includes */
#include "port_display.h"
#include "port_system.h"

/* Project includes */
#include "fsm.h"
#include "fsm_display.h"
/* Typedefs --------------------------------------------------------------------*/
struct  fsm_display_t{
    fsm_t f; /* Base struct for FSMs */
    int32_t distance_cm; /* Distance in cm */
    bool new_color; /* New color flag */
    bool status; /* Status of the display */
    bool idle; /* Idle flag */
    uint32_t display_id; /* Display ID */
};
/* Private functions -----------------------------------------------------------*/
void _compute_display_levels (rgb_color_t *p_color, int32_t distance_cm){
    if (distance_cm <=  WARNING_MIN_CM && distance_cm >= DANGER_MIN_CM){
        *p_color = COLOR_RED;
    } else if (distance_cm <= NO_PROBLEM_MIN_CM && distance_cm > WARNING_MIN_CM){
        *p_color = COLOR_YELLOW;
    } else if (distance_cm <= INFO_MIN_CM && distance_cm > NO_PROBLEM_MIN_CM){
        *p_color = COLOR_GREEN;
    } else if (distance_cm <= OK_MIN_CM && distance_cm > INFO_MIN_CM){
        *p_color = COLOR_TURQUOISE;
    } else if (distance_cm > OK_MIN_CM && distance_cm <= OK_MAX_CM){
        *p_color = COLOR_BLUE;
    } else 
        *p_color = COLOR_OFF;
} 
/* State machine input or transition functions */
static bool check_set_new_color (fsm_t *p_this)
{
    fsm_display_t *p_fsm_display = (fsm_display_t *)p_this;
    return p_fsm_display->new_color;
}

static bool check_active (fsm_t *p_this){
    fsm_display_t *p_fsm_display = (fsm_display_t *)p_this;
    return p_fsm_display->status;
}

static bool check_off (fsm_t *p_this){
    fsm_display_t *p_fsm_display = (fsm_display_t *)p_this;
    return !p_fsm_display->status;
}

/* State machine output or action functions */
static void do_set_on (fsm_t *p_this){
    fsm_display_t *p_fsm_display = (fsm_display_t *)p_this;
    port_display_set_rgb(p_fsm_display->display_id, COLOR_OFF); //all duty cycle 0 es asi?
}

static void do_set_color (fsm_t *p_this){
    fsm_display_t *p_fsm_display = (fsm_display_t *)p_this;
    rgb_color_t color;
    _compute_display_levels(&color, p_fsm_display->distance_cm);
    port_display_set_rgb(p_fsm_display->display_id, color);
    p_fsm_display->new_color = false;
    p_fsm_display->idle = true;
}

static void do_set_off (fsm_t *p_this){
    fsm_display_t *p_fsm_display = (fsm_display_t *)p_this;
    port_display_set_rgb(p_fsm_display->display_id, COLOR_OFF); //este esta bien
    p_fsm_display->idle = false; //puede que le falte algo
}
/* Other auxiliary functions */
static fsm_trans_t fsm_trans_display[] = {
    { WAIT_DISPLAY, check_active, SET_DISPLAY, do_set_on },
    { SET_DISPLAY, check_set_new_color,  SET_DISPLAY, do_set_color},
    { SET_DISPLAY, check_off, WAIT_DISPLAY, do_set_off},
    { -1, NULL, -1, NULL }
};

static void fsm_display_init (fsm_display_t *p_fsm_display, uint32_t display_id){
    fsm_init(&p_fsm_display->f, fsm_trans_display);
    p_fsm_display->display_id = display_id;
    p_fsm_display->distance_cm = 10000; //inicializar a un valor que no se use
    p_fsm_display->new_color = false;
    p_fsm_display->status = false;
    p_fsm_display->idle = false;
    port_display_init(display_id);
}
/* Public functions -----------------------------------------------------------*/
fsm_display_t *fsm_display_new(uint32_t display_id)
{
    fsm_display_t *p_fsm_display = malloc(sizeof(fsm_display_t)); /* Do malloc to reserve memory of all other FSM elements, although it is interpreted as fsm_t (the first element of the structure) */
    fsm_display_init(p_fsm_display, display_id); /* Initialize the FSM */
    return p_fsm_display;
};

void fsm_display_destroy (fsm_display_t * p_fsm){
    free(p_fsm);
}

void fsm_display_fire (fsm_display_t * p_fsm){
    fsm_fire(&p_fsm->f);
}


void fsm_display_set_distance (fsm_display_t *p_fsm, uint32_t distance_cm){
    p_fsm->distance_cm = distance_cm;
    p_fsm->new_color = true;
}

bool fsm_display_get_status (fsm_display_t *p_fsm){
    return p_fsm->status;
}

void fsm_display_set_status (fsm_display_t *p_fsm, bool pause){
    p_fsm->status = pause;
}

bool fsm_display_check_activity (fsm_display_t *p_fsm){
    return (p_fsm->status && !p_fsm->idle); //triplazo
}

fsm_t * fsm_display_get_inner_fsm (fsm_display_t *p_fsm){
    return &p_fsm->f;
}

uint32_t fsm_display_get_state (fsm_display_t *p_fsm){
    return fsm_get_state(&p_fsm->f);
}

void fsm_display_set_state (fsm_display_t *p_fsm, int8_t state){
    fsm_set_state(&p_fsm->f, state);
}