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

/** @file clock_tick_task.c
 ** @brief Definiciones de la biblioteca que se encarga de hacer el tick de reloj y escribir la hora actual -
 *Electrónica 4 2025
 **/

/* === Headers files inclusions ==================================================================================== */

#include "FreeRTOS.h"
#include "task.h"

#include "config.h"
#include "clock_tick_task.h"

/* === Macros definitions ========================================================================================== */

#define ButtonScanDelay 15

/* === Private data type declarations ============================================================================== */

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

/* === Public function definitions ================================================================================= */

void ClockTickTask(void * pointer) {
    clock_tick_task_arg_p args = pointer;
    TickType_t last_value;
    args->ms_counter = 0;

    while (1) {
        last_value = xTaskGetTickCount();

        xSemaphoreTake(args->clock_mutex, 0);
        ClockNewTick(args->clock);
        xSemaphoreGive(args->clock_mutex);

        // Genero un evento cada 0,5s
        if (args->ms_counter < 500) {
            args->ms_counter++;
        } else {
            args->ms_counter = 0;
            // DigitalOutputToggle(args->led);
            xEventGroupSetBits(args->event_group, args->second_event);
        }

        vTaskDelayUntil(&last_value, pdMS_TO_TICKS(1))
    }
}

/* === End of documentation ======================================================================================== */
