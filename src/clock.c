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

/** @file clock.c
 ** @brief Código fuente de la biblioteca reloj - Electrónica 4 2025
 **/

/* === Headers files inclusions ==================================================================================== */

#include "clock.h"
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

/* === Macros definitions ========================================================================================== */

/* === Private data type declarations ============================================================================== */

struct clock_s {
    clock_time_u current_time;
    bool valid;
    uint32_t seconds_counter;
    uint16_t ticks_per_second;
    uint8_t ticks_counter;
};

/* === Private function declarations =============================================================================== */

/**
 * @brief Funcion que verifica si la hora es valida
 *
 * Su objetivo es facilitar la lectura de @ref ClockSetTime().
 *
 * @param time tiempo
 * @return retorna true si el la hora es valida.
 */
static bool ClockValidTime(const clock_time_u * time);

/**
 * @brief Funcion que convierte un uint8_t en bcd
 *
 * Es resultado respeta siguiente formato: las unidades están en el índice 0, las decenas en el ínidce 1, ..
 *
 * @param bcd arreglo de uint8_t en el que almacena el resultado.
 */
static void Uint8ToBCD(uint8_t, uint8_t * bcd);

/**
 * @brief Función para convertir Segundos a hora,minutos y segundos en formato BCD
 *
 * @param clock referencia del reloj
 */
static void ClockSecondsToTime(clock_p clock);

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

static bool ClockValidTime(const clock_time_u * time) {
    bool result = true;

    if ((time->bcd[5] >= 2) && (time->bcd[4] > 4)) {
        result = false;
    } else if ((time->bcd[3] >= 6) && (time->bcd[2] > 0)) {
        result = false;
    } else if ((time->bcd[1] >= 6) && (time->bcd[0] > 0)) {
        result = false;
    }

    return result;
}

static void Uint8ToBCD(uint8_t integer, uint8_t * bcd) {
    for (int i = 0; i < 8; i++) {
        bcd[i] = integer % 10;
        integer = integer / 10;
    }
}

static void ClockSecondsToTime(clock_p self) {
    uint8_t seconds, minutes, hours = 0;
    uint8_t bcd[8] = {0};

    seconds = self->seconds_counter % 60;
    Uint8ToBCD(seconds, bcd);
    self->current_time.bcd[0] = bcd[0];
    self->current_time.bcd[1] = bcd[1];

    minutes = (self->seconds_counter / 60) % 60;
    Uint8ToBCD(minutes, bcd);
    self->current_time.bcd[2] = bcd[0];
    self->current_time.bcd[3] = bcd[1];

    hours = (self->seconds_counter / 3600);
    Uint8ToBCD(hours, bcd);
    self->current_time.bcd[4] = bcd[0];
    self->current_time.bcd[5] = bcd[1];
}

/* === Public function definitions ================================================================================= */

clock_p ClockCreate(uint16_t ticks_per_second) {
    static struct clock_s self[1];

    memset(self, 0, sizeof(struct clock_s));
    self->valid = false;
    self->ticks_per_second = ticks_per_second;

    return self;
}

int ClockGetTime(clock_p self, clock_time_u * current_time) {

    memcpy(current_time, &self->current_time, sizeof(clock_time_u));

    return self->valid;
}

int ClockSetTime(clock_p self, const clock_time_u * new_time) {
    int result = 1;

    if (!ClockValidTime(new_time)) {
        result = 0;
    } else {
        self->valid = true;
        memcpy(&self->current_time, new_time, sizeof(clock_time_u));
    }

    return result;
}

void ClockNewTick(clock_p self) {
    self->ticks_counter++;

    if (self->ticks_counter == self->ticks_per_second) {
        self->ticks_counter = 0;
        self->seconds_counter++;
    }

    if (self->seconds_counter == 86400) {
        self->seconds_counter = 0;
    }

    ClockSecondsToTime(self);
}

/* === End of documentation ======================================================================================== */
