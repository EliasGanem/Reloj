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

#ifndef BUTTON_TASK_H_
#define BUTTON_TASK_H_

/** @file button_task.h
 ** @brief Declaraciones de la biblioteca para la gestión de los botones - Electrónica 4 2025
 **/

/* === Headers files inclusions ==================================================================================== */

#include "FreeRTOS.h"
#include "digital_input.h"
#include "event_groups.h"

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

#define BUTTONS_EVENT_BUTTON_0 (1 << 0)
#define BUTTONS_EVENT_BUTTON_1 (1 << 1)
#define BUTTONS_EVENT_BUTTON_2 (1 << 2)
#define BUTTONS_EVENT_BUTTON_3 (1 << 3)
#define BUTTONS_EVENT_BUTTON_4 (1 << 4)
#define BUTTONS_EVENT_BUTTON_5 (1 << 5)
#define BUTTONS_EVENT_BUTTON_6 (1 << 6)
#define BUTTONS_EVENT_BUTTON_7 (1 << 7)

#define BUTTON_TASK_STACK_SIZE (2 * configMINIMAL_STACK_SIZE)

/* === Public data type declarations =============================================================================== */

typedef struct button_task_arg_s {
    digital_input_p button;
    EventGroupHandle_t event_group;
    int event_bit;
} * button_task_arg_p;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

void ButtonTask(void * arguments);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* BUTTON_TASK_H_ */
