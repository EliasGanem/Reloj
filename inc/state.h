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

#ifndef STATE_H_
#define STATE_H_

/** @file state.h
 ** @brief Declaraciones para el control de estados - Electrónica 4 2025
 **/

/* === Headers files inclusions ==================================================================================== */

#include "shield.h"
#include "clock.h"

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

/* === Public data type declarations =============================================================================== */

//! Representa los estados en lo que puede estar el reloj
typedef enum {
    valid_time,
    invalid_time,
    adjust_time_hours,
    adjust_time_minutes,
    adjust_alarm_hours,
    adjust_alarm_minutes,
} states_e;

//! Struct que contiene los parametros de la función KeepedHoldButton(check_button_hold_p)
typedef struct check_button_hold_s {
    const digital_input_p button; //!< botón
    uint32_t counter;             //!< contador propio del boton
    const uint32_t time_to_hold;  //!< tiempo que se desea que se controle el boton.
} * check_button_hold_p;

/* === Public variable declarations ================================================================================ */

//! Variable que tiene el estado actual del reloj
extern states_e current_state;

/* === Public function declarations ================================================================================ */

/**
 * @brief Función que se encarga de cambiar el estado de la MEF del reloj
 *
 * @param shield referencia al objeto placa
 * @param clock referencia al objero reloj
 * @param set_time parametros para KeepedHoldButton(check_button_hold_p)
 * @param set_alarm parametros para KeepedHoldButton(check_button_hold_p)
 */
void State(shield_p shield, clock_p clock, check_button_hold_p set_time, check_button_hold_p set_alarm);

/**
 * @brief Función para modificar el comportamiento del display segun el estado de la MEF del reloj, para facilitar
 * legibilidad del codigo.
 *
 * @param shield referencia al objeto
 * @param clock referencia al objeto reloj
 * @param next_state proximo estado
 *
 */
void ChangeState(shield_p shield, clock_p clock, states_e next_state);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* STATE_H_ */
