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

/** @file show.c
 ** @brief Definiciones de la biblioteca para determinar que se muesta - Electrónica 4 2025
 **/

/* === Headers files inclusions ==================================================================================== */

#include "FreeRTOS.h"

#include "display_refresh_task.h"
#include "config.h"

/* === Macros definitions ========================================================================================== */

#define ButtonScanDelay 15

/* === Private data type declarations ============================================================================== */

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

/* === Public function definitions ================================================================================= */

void DisplayRefreshTask(void * pointer) {
    display_refresh_task_arg_p args = pointer;
    TickType_t last_value = xTaskGetTickCount();

    while (1) {
        xSemaphoreTake(args->display_mutex, portMAX_DELAY);
        DisplayRefresh(args->display);
        xSemaphoreGive(args->display_mutex);
        vTaskDelayUntil(&last_value, pdMS_TO_TICKS(1));
    }
}

void WriteTime(void * pointer) {
    write_time_task_arg_p args = pointer;
    EventBits_t events;

    while (1) {
        events = xEventGroupWaitBits(args->event_group, SECOND_EVENT, pdTRUE, pdFALSE, portMAX_DELAY);
        // if de si se peude escribir el tiempo osea si esta en valido o en invaliddo
        if (events & WRITE_FLAG) {
            xSemaphoreTake(args->clock_mutex, portMAX_DELAY);
            ClockGetTime(args->clock, &args->current_time);
            xSemaphoreGive(args->clock_mutex);

            // xSemaphoreTake(args->display_mutex, portMAX_DELAY);
            DisplayWriteBCD(args->display, &args->current_time.bcd[2], DISPLAY_MAX_DIGITS);
            // xSemaphoreGive(args->display_mutex);
        }
    }
}

/* === End of documentation ======================================================================================== */
