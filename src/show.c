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
#include "show.h"
#include "config.h"

/* === Macros definitions ========================================================================================== */

#define ButtonScanDelay 15

/* === Private data type declarations ============================================================================== */

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

/* === Public function definitions ================================================================================= */

void DisplayRefreshTask(void * display) {
    TickType_t last_value = xTaskGetTickCount();

    while (1) {
        DisplayRefresh(display);
        vTaskDelayUntil(&last_value, pdMS_TO_TICKS(1));
    }
}

void ChangeStateTask(void * pointer) {
    change_state_task_arg_p args = pointer;
    int state = 0;

    while (1) {
        xQueuePeek(args->state_queue, &state, portMAX_DELAY);
        xSemaphoreTake(args->display_mutex, portMAX_DELAY);
        switch (state) {
        case INVALID_TIME_STATE:
            DisplayBlinkingDigits(args->display, 0, 3, 50);
            DisplayDot(args->display, 0, false, 0);
            DisplayDot(args->display, 1, false, 0);
            DisplayDot(args->display, 2, true, 50);
            DisplayDot(args->display, 3, false, 0);
            break;
        case VALID_TIME_STATE:
            DisplayBlinkingDigits(args->display, 0, 3, 0);
            break;
        case ADJUST_TIME_MINUNTES_STATE:
            DisplayBlinkingDigits(args->display, 0, 1, 50);
            DisplayDot(args->display, 2, true, 0);
            break;
        case ADJUST_TIME_HOURS_STATE:
            DisplayBlinkingDigits(args->display, 2, 3, 50);
            break;
        case ADJUST_ALARM_MINUNTES_STATE:
            DisplayBlinkingDigits(args->display, 0, 1, 50);
            DisplayDot(args->display, 0, true, 100);
            DisplayDot(args->display, 1, true, 100);
            DisplayDot(args->display, 2, true, 100);
            DisplayDot(args->display, 3, true, 100);
            break;
        case ADJUST_ALARM_HOURS_STATE:
            DisplayBlinkingDigits(args->display, 2, 3, 50);
            DisplayDot(args->display, 0, true, 100);
            DisplayDot(args->display, 1, true, 100);
            DisplayDot(args->display, 2, true, 100);
            DisplayDot(args->display, 3, true, 100);
            break;
        default:
            break;
        }
        xSemaphoreGive(args->display_mutex);
    }
}

/* === End of documentation ======================================================================================== */
