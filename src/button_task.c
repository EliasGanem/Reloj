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

/** @file button_task.c
 ** @brief Definiciones de la biblioteca para la gestión de los botones - Electrónica 4 2025
 **/

/* === Headers files inclusions ==================================================================================== */

#include "button_task.h"

/* === Macros definitions ========================================================================================== */

#define ButtonScanDelay 20

/* === Private data type declarations ============================================================================== */

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

/* === Public function definitions ================================================================================= */

void ButtonTask(void * pointer) {
    button_task_arg_p args = pointer;

    while (1) {
        if (DigitalInputGetIsActive(args->button)) {
            xEventGroupSetBits(args->event_group, args->event_bit);
        }
        vTaskDelay(pdMS_TO_TICKS(ButtonScanDelay));
    }
}

/* === End of documentation ======================================================================================== */
