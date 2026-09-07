#ifndef APP_H
#define APP_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    GEAR_P = 0,
    GEAR_R,
    GEAR_N,
    GEAR_D,
    GEAR_S
} GearType;

typedef struct {
    uint16_t speed;
    uint16_t rpm;
    uint8_t fuel;
    uint8_t coolant_temp;
    GearType gear;
    bool left_turn;
    bool right_turn;
    bool headlight;
    uint8_t brightness;
} VehicleState;

void app_init(void);
VehicleState* app_get_state(void);
void app_set_speed(uint16_t speed);
void app_set_rpm(uint16_t rpm);
void app_set_fuel(uint8_t fuel);
void app_set_coolant_temp(uint8_t temp);
void app_set_gear(GearType gear);
void app_toggle_left_turn(void);
void app_toggle_right_turn(void);
void app_toggle_headlight(void);
void app_set_brightness(uint8_t level);
void app_simulate_step(void);

#endif
