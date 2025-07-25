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

/** @file button_tasks.c
 ** @brief Definiciones de la biblioteca para la gestión de los botones - Electrónica 4 2025
 **/

/* === Headers files inclusions ==================================================================================== */

#include "button_tasks.h"
#include "task.h"
#include "config.h"

/* === Macros definitions ========================================================================================== */

#define BUTTON_SCAN_DELAY     20
#define DINDT_PUSH_SCAN_DELAY (BUTTON_SCAN_DELAY * BUTTONS_NUMBER)

/* === Private data type declarations ============================================================================== */

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

/* === Public function definitions ================================================================================= */

void ButtonTask(void * pointer) {
    button_task_arg_p args = pointer;
    TickType_t last_value;

    if (args->hold_time <= BUTTON_SCAN_DELAY) {
        args->hold_time = 100 * BUTTON_SCAN_DELAY;
    } else {
        args->hold_time = args->hold_time / BUTTON_SCAN_DELAY;
    }

    args->time_counter = 0;

    while (1) {
        last_value = xTaskGetTickCount();

        if (DigitalInputGetIsActive(args->button)) {
            if (args->time_counter < args->hold_time) {
                args->time_counter++;
                if (args->time_counter == args->hold_time) {
                    args->time_counter = args->hold_time + 1; // esto indica que está presionado
                    xEventGroupSetBits(args->event_group, args->hold_event);
                }
            }
        } else if (0 < args->time_counter && args->time_counter < args->hold_time) { // solto antes del hold_time
            args->time_counter = 0;
            xEventGroupSetBits(args->event_group, args->push_event);
        } else if (args->time_counter > args->hold_time) { // evita que tome un push inmediatamente despues de un hold
            args->time_counter = 0;
        }

        vTaskDelayUntil(&last_value, pdMS_TO_TICKS(BUTTON_SCAN_DELAY));
    }
}

void DidntPushTask(void * pointer) {
    didnt_prush_task_arg_p args = pointer;
    EventBits_t events;
    args->counter = 0;

    while (1) {
        events = xEventGroupWaitBits(args->event_group, args->buttons, pdFALSE, pdFALSE,
                                     pdMS_TO_TICKS(DINDT_PUSH_SCAN_DELAY));
        if (events & args->buttons) {
            args->counter = 0;
        } else {
            if (args->counter < (args->time_ms / DINDT_PUSH_SCAN_DELAY)) {
                args->counter++;
                if (args->counter == (args->time_ms / DINDT_PUSH_SCAN_DELAY)) {
                    xEventGroupSetBits(args->event_group, DIDNT_PRESS_EVENT);
                    args->counter = 0;
                }
            }
        }
    }
}

/* === End of documentation ========================================================================================
 */
