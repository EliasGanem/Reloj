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

#ifndef DIGITAL_OUTPUT_H_
#define DIGITAL_OUTPUT_H_

/** @file digital_output.h
 ** @brief Declaraciones del modulo para la gestión de salidas digitales - Electrónica 4 2025
 **/

/* === Headers files inclusions ==================================================================================== */

#include <stdint.h>

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

/* === Public data type declarations =============================================================================== */

//! Refencia al objeto salida digital
typedef struct digital_output_s * digital_output_p;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

/**
 * @brief Funcion para crear una Salida Digital
 *
 * Si pudo asignar un espacio de memoria devuelve una referencia a la salida digital creada ademas desactiva esta salida
 * digital, en caso contrario devuelve NULL.
 *
 * @param port Puerto de la salida digital creada
 * @param pin Pin de la salida digital creada
 * @return digital_output_p Referencia  la salida digital creada
 */
digital_output_p DigitalOutputCreate(uint8_t port, uint32_t pin);

/**
 * @brief Funcion para activar para decirle a una salida digital que se acative
 *
 * @param self Referencia a la salida digital
 */
void DigitalOutputActivate(digital_output_p self);

/**
 * @brief Funcion para desactivar para decirle a una salida digital que se desacative
 *
 * @param self Referencia a la salida digital
 */
void DigitalOutputDeactivate(digital_output_p self);

/**
 * @brief Funcion para decirle a una salida digital que cambie de estado
 *
 * @param self Referencia de la salida digital
 */
void DigitalOutputToggle(digital_output_p self);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* DIGITAL_OUTPUT_H_ */
