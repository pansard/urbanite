/**
 * @file port_display.h
 * @brief Header for the portable functions to interact with the HW of the display system. The functions must be implemented in the platform-specific code.
 * @author alumno1
 * @author alumno2
 * @date fecha
 */
#ifndef PORT_DISPLAY_SYSTEM_H_
#define PORT_DISPLAY_SYSTEM_H_

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdint.h>

/* Typedefs --------------------------------------------------------------------*/
typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
}rgb_color_t;

/* Defines and enums ----------------------------------------------------------*/
/* Defines */
#define PORT_REAR_PARKING_DISPLAY_ID 0
#define PORT_DISPLAY_RGB_MAX_VALUE 255
#define COLOR_RED (rgb_color_t){255, 0, 0}
#define COLOR_GREEN (rgb_color_t){0, 255, 0}
#define COLOR_BLUE (rgb_color_t){0, 0, 255}
#define COLOR_YELLOW (rgb_color_t){94, 94, 0}
#define COLOR_TURQUOISE (rgb_color_t){26, 89, 82}
#define COLOR_OFF (rgb_color_t){0, 0, 0}

/* Function prototypes and explanation -------------------------------------------------*/
void port_display_init(uint32_t display_id);

void port_display_set_rgb(uint32_t display_id, rgb_color_t color);

#endif /* PORT_DISPLAY_SYSTEM_H_ */