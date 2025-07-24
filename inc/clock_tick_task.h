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

#ifndef CLOCK_TICK_TASK_H_
#define CLOCK_TICK_TASK_H_

/** @file show.h
 ** @brief Declaraciones de la biblioteca para realizar el tick del reloj y escribir la hora - Electrónica 4 2025
 **/

/* === Headers files inclusions ==================================================================================== */

#include "event_groups.h"
#include "semphr.h"

#include "clock.h"
#include "digital_output.h"
/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

#define CLOCK_TICK_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE)

/* === Public data type declarations =============================================================================== */

typedef struct clock_tick_task_arg_s {
    SemaphoreHandle_t clock_mutex;
    EventGroupHandle_t event_group;
    int second_event;
    int ms_counter;
    clock_p clock;
    digital_output_p led;
} * clock_tick_task_arg_p;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

void ClockTickTask(void * arguments);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* CLOCK_TICK_TASK_H_ */
