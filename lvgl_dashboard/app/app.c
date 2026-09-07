#include "app.h"
#include <stdlib.h>

static VehicleState state;

void app_init(void)
{
    state.speed = 0;
    state.rpm = 800;
    state.fuel = 75;
    state.coolant_temp = 85;
    state.gear = GEAR_P;
    state.left_turn = false;
    state.right_turn = false;
    state.headlight = false;
    state.brightness = 80;
}

VehicleState* app_get_state(void)
{
    return &state;
}

void app_set_speed(uint16_t speed)
{
    state.speed = speed > 240 ? 240 : speed;
}

void app_set_rpm(uint16_t rpm)
{
    state.rpm = rpm > 8000 ? 8000 : rpm;
}

void app_set_fuel(uint8_t fuel)
{
    state.fuel = fuel > 100 ? 100 : fuel;
}

void app_set_coolant_temp(uint8_t temp)
{
    state.coolant_temp = temp > 120 ? 120 : temp;
}

void app_set_gear(GearType gear)
{
    state.gear = gear;
}

void app_toggle_left_turn(void)
{
    state.left_turn = !state.left_turn;
    if (state.left_turn) state.right_turn = false;
}

void app_toggle_right_turn(void)
{
    state.right_turn = !state.right_turn;
    if (state.right_turn) state.left_turn = false;
}

void app_toggle_headlight(void)
{
    state.headlight = !state.headlight;
}

void app_set_brightness(uint8_t level)
{
    state.brightness = level > 100 ? 100 : level;
}

void app_simulate_step(void)
{
    static int16_t speed_delta = 1;

    state.speed += speed_delta;
    if (state.speed >= 120) speed_delta = -1;
    if (state.speed == 0) speed_delta = 1;

    state.rpm = 800 + state.speed * 25 + (rand() % 100);
    if (state.rpm > 8000) state.rpm = 8000;
}
