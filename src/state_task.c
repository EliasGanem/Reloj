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
    int state = 0;

    while (1) {
        events = xEventGroupWaitBits(args->buttons_event_group, BUTTONS_EVENT, pdTRUE, pdFALSE, portMAX_DELAY);
        switch (events) {
        case ACCEPT_EVENT:
            state = INVALID_TIME_STATE;
            break;
        case CANCEL_EVENT:
            state = VALID_TIME_STATE;
            break;
        default:
            break;
        }
        // aquí hay un problema haciendo que espere indefinidamente, si la tarea de ShowState no saca el elemento de la
        // cola lo suficientemente rapido
        xQueueSend(args->state_queue, &state, portMAX_DELAY);
    }
}

/* === End of documentation ======================================================================================== */
