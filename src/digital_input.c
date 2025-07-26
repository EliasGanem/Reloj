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

/** @file digital_input.c
 ** @brief Codigo fuente del modulo de entradas digitales para el proyecto reloj - Electrónica 4 2025
 **/

/* === Headers files inclusions ==================================================================================== */

#include "digital_input.h"
#include "config.h"
#include "chip.h"
#include <stdlib.h>

/* === Macros definitions ========================================================================================== */

#ifndef DIGITAL_INPUT_MAX_INSTANCE
#define DIGITAL_INPUT_MAX_INSTANCE 3
#endif

/* === Private data type declarations ============================================================================== */

//! Estructura que representa una entrada digital
struct digital_input_s {
    uint8_t port;   //!< Puerto al que pertenece la entrada digital
    uint32_t pin;   //!< Pin al que pertenece la entrada digital
    bool inverted;  //!< Indica si la entrada digital tiene logica invertida
    bool laststate; //!< Recuerda el estado anterior independiente de si es logica invertida o no
#ifndef USE_DYNAMIC_MEMORY
    bool used; //!< Indica si la entrada digital esta siendo usada. Solo es usado cuando NO se tiene memoria dinamcia
#endif
};

#ifndef USE_DYNAMIC_MEMORY
//! Array de entradas digitales, es usado en caso de NO tener memoria dinamica
static struct digital_input_s instances[DIGITAL_INPUT_MAX_INSTANCE] = {0};
#endif

/* === Private function declarations =============================================================================== */

#ifndef USE_DYNAMIC_MEMORY
/**
 * @brief Funcion para Crear un objeto entrada digital
 *
 * Esta funcion es usa en caso de no usar memoria dinamica
 *
 * @return digital_input_p Referencia a la entrada digital usada
 */
static digital_input_p CreateInstance(void);
#endif

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

#ifndef USE_DYNAMIC_MEMORY
static digital_input_p CreateInstance() {
    digital_input_p self = NULL;
    int i;

    for (i = 0; i < DIGITAL_INPUT_MAX_INSTANCE; i++) {
        if (!instances[i].used) {
            instances[i].used = true;
            self = &instances[i];
            break;
        }
    }

    return self;
}
#endif

digital_input_p DigitalInputCreate(uint8_t port, uint32_t pin, bool inverted) {
    digital_input_p self;

#ifdef USE_DYNAMIC_MEMORY
    self = malloc(sizeof(struct digital_input_s));
#else
    self = CreateInstance();
#endif

    if (self != NULL) {
        self->port = port;
        self->pin = pin;
        self->inverted = inverted;
        Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, self->port, self->pin, false);
        self->laststate = DigitalInputGetIsActive(self);
    }

    return self;
}

bool DigitalInputGetIsActive(digital_input_p self) {
    bool state = false; //!< estado real del pin
    if (Chip_GPIO_ReadPortBit(LPC_GPIO_PORT, self->port, self->pin) != 0) {
        state = true;
    }
    if (self->inverted) {
        state = !state;
    }
    return state;
}

digital_input_changes_t DigitalInputWasChanged(digital_input_p self) {
    digital_input_changes_t result = DIGITAL_INPUT_NO_CHANGE;

    bool state = DigitalInputGetIsActive(self);
    if (!self->laststate && state) {
        result = DIGITAL_INPUT_WAS_ACTIVATED;
    } else if (self->laststate && !state) {
        result = DIGITAL_INPUT_WAS_DEACTIVATED;
    }

    self->laststate = state;

    return result;
}

bool DigitalInputWasActivated(digital_input_p self) {
    return DIGITAL_INPUT_WAS_ACTIVATED == DigitalInputWasChanged(self);
}

bool DigitalInputWasDeactivated(digital_input_p self) {
    return DIGITAL_INPUT_WAS_DEACTIVATED == DigitalInputWasChanged(self);
}

/* === End of documentation ======================================================================================== */
