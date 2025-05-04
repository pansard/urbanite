#include <stdlib.h>
#include <stdio.h>
#include "port_system.h"
#include "fsm.h"
#include "fsm_urbanite.h"

struct fsm_urbanite_t {
    fsm_t f;
    fsm_button_t * p_fsm_button;
    uint32_t on_off_press_time_ms;
    uint32_t pause_display_time_ms;
    bool is_paused;
    fsm_ultrasound_t *p_fsm_ultrasound_rear;
    fsm_display_t *p_fsm_display_rear;
};

/* STATE MACHINE INPUT FUNCTIONS */

static bool check_on (fsm_t *p_this) {
    fsm_button_t * state = ((fsm_button_t *)p_this);
    
    if (fsm_button_get_duration(state) > 0) {
        return true;
    } else {
        return false;
    }
}

static bool check_off (fsm_t *p_this) {
    check_on(p_this);
}

static bool check_new_measure (fsm_t *p_this) {
    fsm_ultrasound_t * measure = ((fsm_ultrasound_t *)p_this);
    return fsm_ultrasound_get_new_measurement_ready(measure);
}

static bool check_pause_display (fsm_t *p_this) {
    fsm_button_t * display = ((fsm_button_t *)p_this);
    if (fsm_button_get_duration(display) > 0) { // igual hacen falta más condiciones
        return true;
    } else {
        return false;
    }
}

static bool check_activity (fsm_t *p_this) {
    if (check_on(p_this) || check_pause_display(p_this) || check_new_measure(p_this)) { // REVISAR CONDICIÓN
        return true;
    } else {
        return false;
    }
}

static void check_no_activity (fsm_t *p_this) {
    return !check_activity(p_this);
}

static bool check_no_activity (fsm_t *p_this) {
    return check_new_measure(p_this);
}

/* STATE MACHINE OUTPUT FUNCTIONS */

static void do_start_up_measure(fsm_t *p_this) {
    fsm_button_t * button = ((fsm_button_t *)p_this);
    fsm_button_reset_duration(button);
    fsm_ultrasound_start_measurement(((fsm_urbanite_t *)p_this)->p_fsm_ultrasound_rear); //???
    fsm_display_start_measurement(((fsm_urbanite_t *)p_this)->p_fsm_display_rear); // ???
    fsm_display_set_status(((fsm_urbanite_t *)p_this)->p_fsm_display_rear, fsm_display_get_status(((fsm_urbanite_t *)p_this)->p_fsm_display_rear)); //???
    printf("[URBANITE][%ld] Urbanite system ON\n", port_system_get_millis());
}

static void do_display_distance(fsm_t *p_this) {
    fsm_ultrasound_t * ultrasound = ((fsm_ultrasound_t *)p_this);
    uint32_t dist = fsm_ultrasound_get_distance(ultrasound);
    if (dist < WARNING_MIN_CM / 2) {
        
    }
}