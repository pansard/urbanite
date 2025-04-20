/**
 * @file port_ultrasound.h
 * @brief Header for the portable functions to interact with the HW of the ultrasound sensors. The functions must be implemented in the platform-specific code.
 * @author Mateo Pansard
 * @author Lucia Petit
 * @date 2025-03-9
 */
#ifndef PORT_ULTRASOUND_H_
#define PORT_ULTRASOUND_H_

/* Includes ------------------------------------------------------------------*/

/* Standard C includes */
#include <stdint.h>
#include <stdbool.h>

/* Defines and enums ----------------------------------------------------------*/
#define PORT_REAR_PARKING_SENSOR_ID 0  /*!<Rear parking sensor identifier */
#define PORT_PARKING_SENSOR_TRIGGER_UP_US 10 /*!< Duration in microseconds of the trigger signal */
#define PORT_PARKING_SENSOR_TIMEOUT_MS 100 /*!< Time in ms to wait for the next measurement */
#define SPEED_OF_SOUND_MS 343         /*!< Speed of sound in air in m/s */

/* Function prototypes and explanation -------------------------------------------------*/

/**
 * @brief Initializes the ultrasound sensor. Configures the HW specs.
 * 
 * @param ultrasound_id 
 */
void port_ultrasound_init(uint32_t ultrasound_id);

/**
 * @brief Starts a measurement for the ultrasound sensor.
 * 
 * @param ultrasound_id 
 */
void port_ultrasound_start_measurement(uint32_t ultrasound_id);

/**
 * @brief Stops the timer of the trigger signal.
 * 
 * @param ultrasound_id 
 */
void port_ultrasound_stop_trigger_timer(uint32_t ultrasound_id);

/**
 * @brief Starts the timer of the echo signal.
 * 
 * @param ultrasound_id 
 */
void port_ultrasound_stop_echo_timer(uint32_t ultrasound_id);

/**
 * @brief Starts the timer that controls a new measurement for the ultrasound sensor.
 * 
 */
void port_ultrasound_start_new_measurement_timer(void);

/**
 * @brief Stops the timer that controls a new measurement for the ultrasound sensor.
 * 
 */
void port_ultrasound_stop_new_measurement_timer(void);

/**
 * @brief Resets the ticks of the echo signal.
 * 
 * @param ultrasound_id 
 */
void port_ultrasound_reset_echo_ticks(uint32_t ultrasound_id);

/**
 * @brief Stops all timers of the ultrasound and resets echo ticks.
 * 
 * @param ultrasound_id 
 */
void port_ultrasound_stop_ultrasound(uint32_t ultrasound_id);

/**
 * @brief Checks if the ultrasound is ready or not.
 *
 * @param ultrasound_id
 * @return true
 * @return false
 */
bool port_ultrasound_get_trigger_ready(uint32_t ultrasound_id);

/**
 * @brief Sets the trigger ready status of the ultrasound sensor.
 * 
 * @param ultrasound_id 
 * @param trigger_ready 
 */
void port_ultrasound_set_trigger_ready(uint32_t ultrasound_id, bool trigger_ready);

/**
 * @brief Checks the status of the trigger signal.
 * 
 * @param ultrasound_id 
 * @return true 
 * @return false 
 */
bool port_ultrasound_get_trigger_end(uint32_t ultrasound_id);

/**
 * @brief Sets the status of the trigger signal.
 * 
 * @param ultrasound_id 
 * @param trigger_end 
 */
void port_ultrasound_set_trigger_end(uint32_t ultrasound_id, bool trigger_end);

/**
 * @brief Gets the tick when the beginning of the echo signal was received.
 * 
 * @param ultrasound_id 
 * @return uint32_t 
 */
uint32_t port_ultrasound_get_echo_init_tick(uint32_t ultrasound_id);

/**
 * @brief Sets the tick when the beginning of the echo signal was received.
 * 
 * @param ultrasound_id 
 * @param echo_init_tick 
 */
void port_ultrasound_set_echo_init_tick(uint32_t ultrasound_id, uint32_t echo_init_tick);

/**
 * @brief Gets the tick when the end of the echo signal was received.
 * 
 * @param ultrasound_id 
 * @return uint32_t 
 */
uint32_t port_ultrasound_get_echo_end_tick(uint32_t ultrasound_id);

/**
 * @brief Sets the tick when the end of the echo signal was received.
 * 
 * @param ultrasound_id 
 * @param echo_end_tick 
 */
void port_ultrasound_set_echo_end_tick(uint32_t ultrasound_id, uint32_t echo_end_tick);

/**
 * @brief Checks if the echo signal has been received or not.
 * 
 * @param ultrasound_id 
 * @return true
 * @return false
 */
bool port_ultrasound_get_echo_received(uint32_t ultrasound_id);

/**
 * @brief Sets the status of the echo signal.
 * 
 * @param ultrasound_id 
 * @param echo_received 
 */
void port_ultrasound_set_echo_received(uint32_t ultrasound_id, bool echo_received);

/**
 * @brief Gets the number of overflows of the echo signal timer.
 * 
 * @param ultrasound_id 
 * @return uint32_t 
 */
uint32_t port_ultrasound_get_echo_overflows(uint32_t ultrasound_id);

/**
 * @brief Sets the number of overflows of the echo signal timer.
 * 
 * @param ultrasound_id 
 * @param echo_overflows 
 */
void port_ultrasound_set_echo_overflows(uint32_t ultrasound_id, uint32_t echo_overflows);

#endif /* PORT_ULTRASOUND_H_ */
