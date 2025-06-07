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

#ifndef DISPLAY_MAX_INSTANCE
#define DISPLAY_MAX_INSTANCE 1
#endif

#ifndef DISPLAY_MAX_DIGITS
#define DISPLAY_MAX_DIGITS 2
#endif

/* === Private data type declarations ============================================================================== */

//! Estructura que representa a una pantalla de displays de 7 segmentos
struct display_s {
    display_controller_p driver; //!< puntero a controladores del display
    struct {
        uint8_t from;   //!< dígito desde el que se hacen parpadear los segmentos
        uint8_t to;     //!< ultimo dígito que se hacen parpadear los segmentos
        uint16_t calls; //!< cantidad de llamadas a @ref DisplayRefresh() para los segmentos
        uint16_t count; //!< cuenta la cantidad de llamadas a @ref DisplayRefresh() para los segmentos
        uint16_t dots_calls[DISPLAY_MAX_DIGITS]; //!< cantidad de llamadas a @ref DisplayRefresh() para los puntos
        uint16_t
            dots_count[DISPLAY_MAX_DIGITS]; //!< cuenta la cantidad de llamadas @ref DisplayRefresh() para los puntos
    } blinking[1];
    uint8_t digits;                           //!< cantidad de displays de 7 segmentos de la pantalla
    uint8_t video_memory[DISPLAY_MAX_DIGITS]; //!< array utilizado para memorizar los segmentos prendidos de cada //!<
    //!< display
    uint8_t current_digit;
#ifndef USE_DYNAMIC_MEMORY
    bool used; //!< indica si el struc esta siendo usado en caso de no usar memoria dinamica
#endif
};

#ifndef USE_DYNAMIC_MEMORY
//! Array que contiene los display creados si no se utuliza memoria dinámica.
static struct display_s instances[DISPLAY_MAX_INSTANCE] = {0};
#endif

//! Array con los segmentos que se deben prender según el número en bcd.
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

#ifndef USE_DYNAMIC_MEMORY
/**
 * @brief Función para crear un display si no se usa memoria dinámica
 *
 * @return shield_p devuelve una referencia al display creado
 */
static display_p CreateInstance();
#endif

/**
 * @brief Función para saber si hay que apagar los segmentos dentro de DisplayRefresh
 *
 * @param self referencia del display con el que se trabaja
 * @return int devuelve 1 si hay que apagar los segmentos
 */
static int TurnOffSegments(display_p self);

/**
 * @brief Función para saber si hay que apagar el punto dentro de DisplayRefresh
 *
 * @param self referencia del display con el que se trabaja
 * @return int devuelve 1 si hay que apagar el punto
 */
static int TurnOffDots(display_p self);

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

#ifndef USE_DYNAMIC_MEMORY
static display_p CreateInstance() {
    display_p self = NULL;
    int i;

    for (i = 0; i < SHIELD_MAX_INSTANCE; i++) {
        if (!instances[i].used) {
            instances[i].used = true;
            self = &instances[i];
            break;
        }
    }

    return self;
}
#endif

int TurnOffSegments(display_p self) {
    int result = 0;

    if (self->blinking->count < (self->blinking->calls / 2)) {
        if (self->current_digit >= self->blinking->from) {
            if (self->current_digit <= self->blinking->to) {
                result = 1;
            }
        }
    }

    return result;
}

static int TurnOffDots(display_p self) {
    int result = 0;
    if (self->blinking->dots_calls[self->current_digit]) {
        if (self->blinking->dots_count[self->current_digit] < self->blinking->dots_calls[self->current_digit] / 2) {
            result = 1;
        }
    }
    return result;
}

/* === Public function definitions ================================================================================= */

display_p DisplayCreate(uint8_t number_of_digits, display_controller_p driver) {
    display_p self = NULL;

#ifdef USE_DYNAMIC_MEMORY
    self = malloc(sizeof(struct display_s));
#else
    self = CreateInstance();
#endif

    if (number_of_digits > DISPLAY_MAX_DIGITS) {
        number_of_digits = DISPLAY_MAX_DIGITS;
    }

    if (self) {
        self->digits = number_of_digits;
        self->driver = driver;
        self->current_digit = 0;
        self->blinking->calls = 0;
        self->blinking->count = 0;
        for (uint8_t i = 0; i < self->digits; i++) {
            self->blinking->dots_calls[i] = 0;
            self->blinking->dots_count[i] = 0;
        }
    }

    return self;
}

void DisplayWriteBCD(display_p self, uint8_t * value, uint8_t size) {
    memset(self->video_memory, 0, sizeof(self->video_memory));

    if (size > self->digits) {
        size = self->digits;
    }

    for (uint8_t i = 0; i < size; i++) {
        self->video_memory[i] = self->video_memory[i] | NUMBERS[value[i]];
    }
}

void DisplayRefresh(display_p self) {
    uint8_t segments;

    self->driver->TurnOffDigits();

    self->current_digit = (self->current_digit + 1) % self->digits;
    segments = self->video_memory[self->current_digit];

    if (self->blinking->calls) {
        if (self->current_digit == 0) {
            self->blinking->count = (self->blinking->count + 1) % self->blinking->calls;
        }
        if (TurnOffSegments(self)) {
            segments = (~SEGMENTS_MASK) & segments;
        }
    }

    // Actualiza el contador de todos los puntos que tienen que parpadear
    if (self->current_digit == 0) {
        for (uint8_t i = 0; i < self->digits; i++) {
            if (self->blinking->dots_calls[i]) {
                self->blinking->dots_count[i] = (self->blinking->dots_count[i] + 1) % self->blinking->dots_calls[i];
            }
        }
    }

    if (TurnOffDots(self)) {
        segments = (SEGMENTS_MASK)&segments;
    }

    self->driver->UpdateSegments(segments);
    self->driver->TurnOnDigit(self->current_digit);
}

int DisplayBlinkingDigits(display_p self, uint8_t from, uint8_t to, uint16_t number_calls) {
    int result = 0;

    if (from > to || from >= self->digits || !self) {
        result = -1;
    } else {
        self->blinking->from = from;
        self->blinking->to = to;
        self->blinking->calls = number_calls * 2;
        self->blinking->count = 0;
    }

    return result;
}

int DisplayDot(display_p self, uint8_t digit, bool on, uint16_t number_of_calls) {
    int result = 0;
    if (!self || digit >= self->digits) {
        result = -1;
    } else {
        if (on) {
            self->video_memory[digit] = SEGMENT_DOT_MASK | self->video_memory[digit];
        } else {
            self->video_memory[digit] = (~SEGMENT_DOT_MASK) & self->video_memory[digit];
        }

        self->blinking->dots_calls[digit] = 2 * number_of_calls;
    }

    return result;
}

/* === End of documentation ======================================================================================== */
