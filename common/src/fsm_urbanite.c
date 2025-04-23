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

