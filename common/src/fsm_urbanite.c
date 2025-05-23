/**
 * @file fsm_urbanite.c
 * @brief Urbanite FSM main file.
 * @author Lucía Petit
 * @author Mateo Pansard
 * @date 18/05/2025
 */

#include <stdlib.h>
#include <stdio.h>
#include "port_system.h"
#include "fsm.h"
#include "fsm_urbanite.h"
#include "port_led.h"

/**
 * @brief Structure of the Urbanite FSM.
 * 
 */
struct fsm_urbanite_t
{
    /** @brief Base struct for FSMs */
    fsm_t f; 
    /** @brief Pointer to the button FSM */
    fsm_button_t *p_fsm_button; 
    /** @brief Time in milliseconds for the button press to turn on/off the system */
    uint32_t on_off_press_time_ms; 
    /** @brief Time in milliseconds for the button press to pause/resume the display */
    uint32_t pause_display_time_ms; 
    /** @brief Flag to indicate if the display is paused */
    bool is_paused; 
    /** @brief Pointer to the ultrasound FSM */
    fsm_ultrasound_t *p_fsm_ultrasound_rear;  
    /** @brief Pointer to the display FSM */
    fsm_display_t *p_fsm_display_rear; 
};

/* STATE MACHINE INPUT FUNCTIONS */

/**
 * @brief Check if the button is pressed for a certain time to turn on the system.
 * 
 * @param p_this 
 * @return true 
 * @return false 
 */
static bool check_on(fsm_t *p_this)
{
    fsm_urbanite_t *urbanite = ((fsm_urbanite_t *)p_this);
    fsm_button_t *state = urbanite->p_fsm_button;
    uint32_t valor = fsm_button_get_duration(state);

    return (valor > 0 && valor > urbanite->on_off_press_time_ms);
}

/**
 * @brief Check if the button is pressed for a certain time to turn off the system.
 * 
 * @param p_this 
 * @return true 
 * @return false 
 */
static bool check_off(fsm_t *p_this)
{
    return check_on(p_this);
}

/**
 * @brief Check if a new measurement is ready from the ultrasound sensor.
 * 
 * @param p_this 
 * @return true 
 * @return false 
 */
static bool check_new_measure(fsm_t *p_this)
{
    fsm_urbanite_t *urbanite = ((fsm_urbanite_t *)p_this);
    fsm_ultrasound_t *measure = urbanite->p_fsm_ultrasound_rear;
    return fsm_ultrasound_get_new_measurement_ready(measure);
}

/**
 * @brief Check if the button is pressed for a certain time to pause the display.
 * 
 * @param p_this 
 * @return true 
 * @return false 
 */
static bool check_pause_display(fsm_t *p_this)
{
    fsm_urbanite_t *urbanite = ((fsm_urbanite_t *)p_this);
    fsm_button_t *button = urbanite->p_fsm_button;
    uint32_t valor = fsm_button_get_duration(button);
    if (valor > 0 && valor < urbanite->on_off_press_time_ms && valor > urbanite->pause_display_time_ms)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * @brief Check if there is activity in the system.
 * 
 * @param p_this 
 * @return true 
 * @return false 
 */
static bool check_activity(fsm_t *p_this)
{
    fsm_urbanite_t *urbanite = ((fsm_urbanite_t *)p_this);
    //printf("[URBANITE][%ld] Urbanite system activity check\n", fsm_button_get_duration(urbanite->p_fsm_button));
    return (fsm_button_check_activity(urbanite->p_fsm_button) || fsm_display_check_activity(urbanite->p_fsm_display_rear) || fsm_ultrasound_check_activity(urbanite->p_fsm_ultrasound_rear));
}

/**
 * @brief Check if there is no activity in the system.
 * 
 * @param p_this 
 * @return true 
 * @return false 
 */
static bool check_no_activity(fsm_t *p_this)
{
    return !check_activity(p_this);
}

/**
 * @brief Check if there is activity during the measurement state.
 * 
 * @param p_this 
 * @return true 
 * @return false 
 */
static bool check_activity_in_measure(fsm_t *p_this)
{
    return check_new_measure(p_this);
}

/* STATE MACHINE OUTPUT FUNCTIONS */

/**
 * @brief Turn the Urbanite system ON. 
 * 
 * @param p_this Pointer to an fsm_t struct that contains an `fsm_urbanite_t`. 
 */
static void do_start_up_measure(fsm_t *p_this)
{
    fsm_urbanite_t *urbanite = ((fsm_urbanite_t *)p_this);
    fsm_button_t *button = urbanite->p_fsm_button;
    fsm_ultrasound_t *ultrasound = urbanite->p_fsm_ultrasound_rear;
    fsm_display_t *display = urbanite->p_fsm_display_rear;

    port_led_on();
    fsm_button_reset_duration(button);
    fsm_ultrasound_start(ultrasound);
    fsm_display_set_status(display, true);
    printf("[URBANITE][%ld] Urbanite system ON\n", port_system_get_millis());
}

/**
 * @brief Turn the Urbanite system OFF. 
 * 
 * @param p_this Pointer to an fsm_t struct that contains an `fsm_urbanite_t`.
 */
static void do_stop_urbanite(fsm_t *p_this)
{
    fsm_urbanite_t *urbanite = ((fsm_urbanite_t *)p_this);
    fsm_button_t *button = urbanite->p_fsm_button;
    fsm_ultrasound_t *ultrasound = urbanite->p_fsm_ultrasound_rear;
    fsm_display_t *display = urbanite->p_fsm_display_rear;

    port_led_off();
    fsm_button_reset_duration(button);
    fsm_ultrasound_stop(ultrasound);
    fsm_display_set_status(display, false);
    urbanite->is_paused = false;
    printf("[URBANITE][%ld] Urbanite system OFF\n", port_system_get_millis());
}

/**
 * @brief Pause or resume the display system. 
 * 
 * @param p_this Pointer to an fsm_t struct that contains an `fsm_urbanite_t`.
 */
static void do_pause_display(fsm_t *p_this)
{
    fsm_urbanite_t *urbanite = ((fsm_urbanite_t *)p_this);
    fsm_button_t *button = urbanite->p_fsm_button;
    fsm_display_t *display = urbanite->p_fsm_display_rear;

    fsm_button_reset_duration(button);
    urbanite->is_paused = !(urbanite->is_paused);
    fsm_display_set_status(display, !urbanite->is_paused);
    
    if (urbanite->is_paused)
    {
        printf("[URBANITE][%ld] Urbanite system display PAUSE\n", port_system_get_millis());
    }
    else
    {
        printf("[URBANITE][%ld] Urbanite system display RESUME\n", port_system_get_millis());
    }
}

/**
 * @brief Display the distance measured by the ultrasound sensor. 
 * 
 * @param p_this Pointer to an fsm_t struct that contains an `fsm_urbanite_t`.
 */
static void do_display_distance(fsm_t *p_this)
{
    fsm_urbanite_t *urbanite = ((fsm_urbanite_t *)p_this);
    fsm_ultrasound_t *ultrasound = urbanite->p_fsm_ultrasound_rear;
    fsm_display_t *display = urbanite->p_fsm_display_rear;

    uint32_t distance_cm = fsm_ultrasound_get_distance(ultrasound);
    if (urbanite->is_paused)
    {
        if (distance_cm < WARNING_MIN_CM / 2)
        {
            fsm_display_set_distance(display, distance_cm);
            fsm_display_set_status(display, true);
        }
        else
        {
            fsm_display_set_status(display, false);
        }
    }
    else
    {
        fsm_display_set_distance(display, distance_cm);
    }
    printf("[URBANITE][%ld] Distance: %ld cm\n", port_system_get_millis(), distance_cm);
}

/**
 * @brief Start the low power mode while the Urbanite is OFF. 
 * 
 * @param p_this Pointer to an fsm_t struct that contains an `fsm_urbanite_t`.
 */
static void do_sleep_off(fsm_t *p_this)
{
    port_led_off();
    port_system_sleep();
}
/**
 * @brief Start the low power mode while the Urbanite is measuring the distance and it is waiting for a new measurement. 
 * 
 * @param p_this Pointer to an fsm_t struct that contains an `fsm_urbanite_t`.
 */
static void do_sleep_while_measure(fsm_t *p_this)
{
    port_system_sleep();
}

/**
 * @brief Start the low power mode while the Urbanite is awakened by a debug breakpoint or similar in the SLEEP_WHILE_OFF state. 
 * 
 * @param p_this Pointer to an fsm_t struct that contains an `fsm_urbanite_t`.
 */
static void do_sleep_while_off(fsm_t *p_this)
{
    port_system_sleep();
}

/**
 * @brief Start the low power mode while the Urbanite is awakened by a debug breakpoint or similar in the SLEEP_WHILE_ON state. 
 * 
 * @param p_this Pointer to an fsm_t struct that contains an `fsm_urbanite_t`.
 */
static void do_sleep_while_on(fsm_t *p_this)
{
    port_system_sleep();
}

/**
 * @brief Array representing the transitions table of the FSM Urbanite. 
 * 
 */
static fsm_trans_t fsm_trans_urbanite[] = {
    {OFF, check_on, MEASURE, do_start_up_measure},
    {OFF, check_no_activity, SLEEP_WHILE_OFF, do_sleep_off}, 
    {MEASURE, check_off, OFF, do_stop_urbanite},
    {MEASURE, check_new_measure, MEASURE, do_display_distance},
    {MEASURE, check_pause_display, MEASURE, do_pause_display},
    {MEASURE, check_no_activity, SLEEP_WHILE_ON, do_sleep_while_measure},
    {SLEEP_WHILE_ON, check_activity_in_measure, MEASURE, NULL},
    {SLEEP_WHILE_ON, check_no_activity, SLEEP_WHILE_ON, do_sleep_while_on},
    {SLEEP_WHILE_OFF, check_activity, OFF, NULL},
    {SLEEP_WHILE_OFF, check_no_activity, SLEEP_WHILE_OFF, do_sleep_while_off}, 
    { -1, NULL, -1, NULL }
};

/**
 * @brief Create a new Urbanite FSM.
 * 
 * @param p_fsm_urbanite Pointer to the Urbanite FSM. 
 * @param p_fsm_button Pointer to the button FSM that activates the system and disables the display if it disturbs the driver. 
 * @param on_off_press_time_ms Button press time in milliseconds to turn the system ON or OFF 
 * @param pause_display_time_ms Time in milliseconds to pause the display after a short press of the button 
 * @param p_fsm_ultrasound_rear Pointer to the ultrasound FSM that measures the distance to the rear obstacle. 
 * @param p_fsm_display_rear Pointer to the display FSM that shows the distance to the rear obstacle.
 */
static void fsm_urbanite_init(fsm_urbanite_t *p_fsm_urbanite,
                              fsm_button_t *p_fsm_button,
                              uint32_t on_off_press_time_ms,
                              uint32_t pause_display_time_ms,
                              fsm_ultrasound_t *p_fsm_ultrasound_rear,
                              fsm_display_t *p_fsm_display_rear)
{
    //encender el led
    fsm_init(&p_fsm_urbanite->f, fsm_trans_urbanite);
    p_fsm_urbanite->p_fsm_button = p_fsm_button;
    p_fsm_urbanite->on_off_press_time_ms = on_off_press_time_ms;
    p_fsm_urbanite->p_fsm_ultrasound_rear = p_fsm_ultrasound_rear;
    p_fsm_urbanite->pause_display_time_ms = pause_display_time_ms;
    p_fsm_urbanite->p_fsm_display_rear = p_fsm_display_rear;
    p_fsm_urbanite->is_paused = false;
};

fsm_urbanite_t *fsm_urbanite_new(fsm_button_t *p_fsm_button,
                                 uint32_t on_off_press_time_ms,
                                 uint32_t pause_display_time_ms,
                                 fsm_ultrasound_t *p_fsm_ultrasound_rear,
                                 fsm_display_t *p_fsm_display_rear)
{
    fsm_urbanite_t *p_fsm_urbanite = (fsm_urbanite_t *)malloc(sizeof(fsm_urbanite_t)); /* Do malloc to reserve memory of all other FSM elements, although it is interpreted as fsm_t (the first element of the structure) */
    fsm_urbanite_init(p_fsm_urbanite, p_fsm_button, on_off_press_time_ms, pause_display_time_ms, p_fsm_ultrasound_rear, p_fsm_display_rear);
    return p_fsm_urbanite;
}

void fsm_urbanite_fire(fsm_urbanite_t *p_fsm_urbanite)
{
    //printf("[URBANITE][%ld] Urbanite system state: %d\n", port_system_get_millis(), p_fsm_urbanite->f.current_state);
    fsm_fire(&p_fsm_urbanite->f);
    //printf("[URBANITE][%ld] Urbanite system activity check\n", fsm_button_get_duration(p_fsm_urbanite->p_fsm_button));
}

void fsm_urbanite_destroy(fsm_urbanite_t *p_fsm_urbanite)
{
    free(&p_fsm_urbanite->f);
}