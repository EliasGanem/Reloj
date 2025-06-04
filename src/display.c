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

/** @file display.c
 *
 *
 ** @brief Implementacion del modulo para la gestion de una pantalla con n displays de 7 segmentos multiplexada para el
 *proyecto reloj - Electrónica 4 2025
 **/

/* === Headers files inclusions ==================================================================================== */

#include <stdlib.h>
#include <string.h>
#include "display.h"
#include "config.h"
#include "shield_config.h"

/* === Macros definitions ========================================================================================== */

#ifndef DISPLAY_MAX_DIGITS
#define DISPLAY_MAX_DIGITS 2
#endif

/* === Private data type declarations ============================================================================== */

//! Estructura que representa a una pantalla de displays de 7 segmentos
struct display_s {
    struct display_controller_s * driver;
    uint8_t digits;                           //!< cantidad de displays de 7 segmentos de la pantalla
    uint8_t video_memory[DISPLAY_MAX_DIGITS]; //!< array utilizado para memorizar los segmentos prendidos de cada //!<
                                              //!< display
    uint8_t current_digit;
};

static const uint8_t NUMBERS[10] = {
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_E | SEGMENT_F,             // 0
    SEGMENT_B | SEGMENT_C,                                                             // 1
    SEGMENT_A | SEGMENT_B | SEGMENT_D | SEGMENT_E | SEGMENT_G,                         // 2
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_G,                         // 3
    SEGMENT_B | SEGMENT_C | SEGMENT_F | SEGMENT_G,                                     // 4
    SEGMENT_A | SEGMENT_C | SEGMENT_D | SEGMENT_F | SEGMENT_G,                         // 5
    SEGMENT_A | SEGMENT_C | SEGMENT_D | SEGMENT_E | SEGMENT_F | SEGMENT_G,             // 6
    SEGMENT_A | SEGMENT_B | SEGMENT_C,                                                 // 7
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_E | SEGMENT_F | SEGMENT_G, // 8
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_F | SEGMENT_G,             // 9
};
/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

/* === Public function definitions ================================================================================= */

display_p DisplayCreate(uint8_t number_digits, display_controller_p driver) {
    display_p self = malloc(sizeof(struct display_s));

    if (number_digits > DISPLAY_MAX_DIGITS) {
        number_digits = DISPLAY_MAX_DIGITS;
    }

    if (self) {
        self->digits = number_digits;
        self->driver = driver;
        self->current_digit = 0;
    }

    return self;
}

void DisplayWriteBCD(display_p self, uint8_t * value, uint8_t size) {
    memset(self->video_memory, 0, sizeof(self->video_memory));

    if (size > self->digits) {
        size = self->digits;
    }

    for (uint8_t i = 0; i < size; i++) {
        self->video_memory[i] = NUMBERS[value[i]];
    }
}

void DisplayRefresh(display_p self) {
    self->driver->TurnOffDigits();

    self->current_digit = (self->current_digit + 1) % self->digits;

    self->driver->UpdateSegments(self->video_memory[self->current_digit]);
    self->driver->TurnOnDigit(self->current_digit);
}

/* === End of documentation ======================================================================================== */
