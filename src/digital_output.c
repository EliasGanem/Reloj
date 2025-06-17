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

/** @file digital_output.c
 ** @brief Código fuente del modulo de salidas digitales para el proyecto reloj - Electrónica 4 2025
 **/

/* === Headers files inclusions ==================================================================================== */

#include "digital_output.h"
#include "config.h"
#include "chip.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/* === Macros definitions ========================================================================================== */

#ifndef DIGITAL_OUTPUT_MAX_INSTANCE
#define DIGITAL_OUTPUT_MAX_INSTANCE 4
#endif

/* === Private data type declarations ============================================================================== */

//! Estructura que representa una salida digital
struct digital_output_s {
    uint8_t port; //!< Puerto al que pertenece la salida digital
    uint32_t pin; //!< Pin al que pertenece la salida digital
#ifndef USE_DYNAMIC_MEMORY
    bool used; //!< Indica si la salida digital esta siendo usada. Solo es usado cuando NO se tiene memoria dinamcia
#endif
};

/* === Private function declarations =============================================================================== */

#ifndef USE_DYNAMIC_MEMORY
/**
 * @brief Funcion para crear instancia de alumno en caso de NO usar memoria dinamica
 *
 * Esta funcion hace el control de si hay espacio de memoria disponible para crear una nueva salida digital
 *
 * @return digital_output_p
 */
static digital_output_p CreateInstance();
#endif

/* === Private variable definitions ================================================================================ */

#ifndef USE_DYNAMIC_MEMORY
//! Array de salidas digitales, es usado en caso de NO tener memoria dinamica
static struct digital_output_s instances[DIGITAL_OUTPUT_MAX_INSTANCE] = {0};
#endif

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

#ifndef USE_DYNAMIC_MEMORY
static digital_output_p CreateInstance() {
    digital_output_p self = NULL;
    int i;

    for (i = 0; i < DIGITAL_OUTPUT_MAX_INSTANCE; i++) {
        if (!instances[i].used) {
            instances[i].used = true;
            self = &instances[i];
            break;
        }
    }

    return self;
}
#endif

digital_output_p DigitalOutputCreate(uint8_t port, uint32_t pin) {
    digital_output_p self;

#ifdef USE_DYNAMIC_MEMORY
    self = malloc(sizeof(struct digital_output_s));
#else
    self = CreateInstance();
#endif

    if (self != NULL) {
        self->port = port;
        self->pin = pin;
        Chip_GPIO_SetPinState(LPC_GPIO_PORT, self->port, self->pin, false);
        Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, self->port, self->pin, true);
    }

    return self;
}

void DigitalOutputActivate(digital_output_p self) {
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, self->port, self->pin, true);
}

void DigitalOutputDeactivate(digital_output_p self) {
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, self->port, self->pin, false);
}

void DigitalOutputToggle(digital_output_p self) {
    Chip_GPIO_SetPinToggle(LPC_GPIO_PORT, self->port, self->pin);
}

/* === End of documentation ======================================================================================== */
