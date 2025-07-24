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

#ifndef DIGITAL_INPUT_H_
#define DIGITAL_INPUT_H_

/** @file digital_input.h
 ** @brief Declaraciones del modulo para la gestión de entradas digitales - Electrónica 4 2025
 **/

/* === Headers files inclusions ==================================================================================== */

#include <stdint.h>
#include <stdbool.h>

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

/* === Public data type declarations =============================================================================== */

//! Referencia al objeto entrada digital
typedef struct digital_input_s * digital_input_p;

//! Tipo de dato de cambios en el estado de una entrada digital
typedef enum digital_input_changes_e {
    DIGITAL_INPUT_WAS_ACTIVATED = 1,
    DIGITAL_INPUT_NO_CHANGE = 0,
    DIGITAL_INPUT_WAS_DEACTIVATED = -1,
} digital_input_changes_t;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

/**
 * @brief Funcion para crear una entrada digital
 *
 * @param port Puerto de de la entrada digital
 * @param pin Pin de la entrada digital
 * @param inverted Indica si la logica de la entrada digital es invertida
 * @return digital_input_p Referencia de la entrada digital
 */
digital_input_p DigitalInputCreate(uint8_t port, uint32_t pin, bool inverted);

/**
 * @brief Funcion para preguntarle a la entrada digital si está activa
 *
 * Se encarga de la logica negada, devuelve uno si se considera un estado activo. Ademas lee por primera vez la entrada
 * para usarla como el ultimo estado.
 * Actualiza el ultimo estado.
 *
 * @param input Referencia a la entrada digital
 * @return true Si la entrada esta activa
 * @return false Si la entrada esta inactiva
 */
bool DigitalInputGetIsActive(digital_input_p input);

/**
 * @brief Funcion para preguntarle a la entrada digital si su estado anterior era activado
 *
 * Ademas actualiza el ultimo estado
 *
 * @param input Referencia a la entrada digital
 * @return true Si el estado anterior de la entrada digital era activado
 * @return false Si el estado anterior de la entrada digital era desactivado
 */
bool DigitalInputWasActivated(digital_input_p input);

/**
 * @brief Funcion para preguntarle a la entrada digital si su estado anterior era desactivado
 *
 * @param input Referencia a la entrada digital
 * @return true Si el estado anterior de la entrada digital era desactivado
 * @return false Si el estado anterior de la entrada digital era activado
 */
bool DigitalInputWasDeactivated(digital_input_p input);

/**
 * @brief Funcion para preguntarle a la entrada digital si cambió
 *
 * Devuelve el estado anterior del pulsador
 *
 * @param input
 * @return digital_input_changes_t
 * \li 1 si el estado anterior era activado
 * \li 0 si no cambio
 * \li -1 si el estado anterior era desactivado
 */
digital_input_changes_t DigitalInputWasChanged(digital_input_p input);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* DIGITAL_INPUT_H_ */
