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

/** @file state_task.c
 ** @brief Definiciones de la biblioteca para la gestión de estados - Electrónica 4 2025
 **/

/* === Headers files inclusions ==================================================================================== */

#include "FreeRTOS.h"
#include "state_task.h"

#include "shield_config.h"
#include "clock.h"
#include "config.h"

/* === Macros definitions ========================================================================================== */

#define StateTaskDelay

/* === Private data type declarations ============================================================================== */

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

/* === Public function definitions ================================================================================= */

void StateTask(void * pointer) {
    state_task_arg_p args = pointer;
    EventBits_t events;
    clock_time_u accept = {
        .time = {.hours = {0, 3}, .minutes = {0, 2}, .seconds = {0, 1}},
    };
    clock_time_u cancel = {
        .time = {.hours = {0, 3}, .minutes = {0, 2}, .seconds = {0, 2}},
    };
    clock_time_u set_time = {
        .time = {.hours = {0, 3}, .minutes = {0, 2}, .seconds = {0, 3}},
    };
    clock_time_u set_alarm = {
        .time = {.hours = {0, 3}, .minutes = {0, 2}, .seconds = {0, 4}},
    };
    int state = 0;

    while (1) {
        xSemaphoreTake(args->display_mutex, portMAX_DELAY);
        events = xEventGroupWaitBits(args->buttons_event_group, BUTTONS_EVENT, pdTRUE, pdFALSE, portMAX_DELAY);
        switch (events) {
        case ACCEPT_EVENT:
            state = INVALID_TIME_STATE;
            // xQueueSend(args->state_queue, &state, portMAX_DELAY);
            DigitalOutputDeactivate(args->buzzer);
            DisplayWriteBCD(args->display, accept.bcd, DISPLAY_MAX_DIGITS);
            break;
        case CANCEL_EVENT:
            DisplayWriteBCD(args->display, cancel.bcd, DISPLAY_MAX_DIGITS);
            break;
        case SET_TIME_EVENT:
            DisplayWriteBCD(args->display, set_time.bcd, DISPLAY_MAX_DIGITS);
            break;
        case SET_ALARM_EVENT:
            DisplayWriteBCD(args->display, set_alarm.bcd, DISPLAY_MAX_DIGITS);
            break;
        default:
            break;
        }
        xSemaphoreGive(args->display_mutex);
    }
}

/* === End of documentation ======================================================================================== */
