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

#ifndef SHOW_H_
#define SHOW_H_

/** @file show.h
 ** @brief Declaraciones de la biblioteca para determinar que mostrar - Electrónica 4 2025
 **/

/* === Headers files inclusions ==================================================================================== */

#include "queue.h"
#include "semphr.h"
#include "display.h"

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

#define DISPLAY_REFRESH_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE)
#define SHOW_STATE_TASK_STACK_SIZE      (10 * configMINIMAL_STACK_SIZE)

/* === Public data type declarations =============================================================================== */

typedef struct display_refresh_task_arg_s {
    SemaphoreHandle_t display_mutex;
    display_p display;
} * display_refresh_task_arg_p;

typedef struct change_state_task_arg_s {
    QueueHandle_t state_queue;
    SemaphoreHandle_t display_mutex;
    display_p display;
} * change_state_task_arg_p;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

void DisplayRefreshTask(void * arguments);

void ShowStateTask(void * pointer);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* SHOW_H_ */
