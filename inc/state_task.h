/*********************************************************************************************************************
Copyright (c) 2025, Elías Ganem <eliasgfac@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit
persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

SPDX-License-Identifier: MIT
*********************************************************************************************************************/

#ifndef STATE_TASK_H_
#define STATE_TASK_H_

/** @file state_task.h
 ** @brief Declaraciones de la biblioteca para la gestión de estados - Electrónica 4 2025
 **/

/* === Headers files inclusions ==================================================================================== */

#include "event_groups.h"
#include "semphr.h"

#include <stdint.h>
#include "digital_output.h"
#include "display.h"
#include "clock.h"

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

#define STATE_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE)

#define STATE_SIZE            sizeof(int)

/* === Public data type declarations =============================================================================== */

typedef enum {
    invalid_time,
    valid_time,
    adjust_time_hours,
    adjust_time_minutes,
    adjust_alarm_hours,
    adjust_alarm_minutes,
} states_e;

typedef struct state_task_arg_s {
    SemaphoreHandle_t display_mutex;
    EventGroupHandle_t buttons_event_group;
    EventGroupHandle_t other_event_group;
    digital_output_p buzzer;
    display_p display;
    clock_p clock;
    clock_time_u new_time;
    states_e state;
} * state_task_arg_p;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

void StateTask(void * arguments);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* STATE_TASK_H_ */
