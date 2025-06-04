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

#ifndef SHIELD_H_
#define SHIELD_H_

/** @file shield.h
 ** @brief Declaraciones del modulo shield - Electrónica 4 2025
 **/

/* === Headers files inclusions ==================================================================================== */

#include <stdint.h>
#include <stdbool.h>
#include "digital_input.h"
#include "digital_output.h"
#include "display.h"
#include "config.h"

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

/* === Public data type declarations =============================================================================== */

//! Estructura que representa el poncho
typedef struct shield_s {
    digital_input_p accept;    //! boton de acceptar
    digital_input_p cancel;    //! boton de cancelar
    digital_input_p set_time;  //! boton para cambiar la hora
    digital_input_p set_alarm; //! boton para cambiar la alarma
    digital_input_p incremet;  //! boton para incrementar
    digital_input_p decrement; //! boton para decrementar
    digital_output_p buzzer;   //! indicador de alarma
    display_p display;         //! display de 7 segmentos para mostrar la hora
#ifndef USE_DYNAMIC_MEMORY
    bool used; //!< indica si el struc esta siendo usado en caso de no usar memoria dinamica
#endif
} const * const shield_p;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */
/**
 * @brief Funcion para crear un pocnho
 *
 * @return shield_p referencia al poncho
 */
shield_p ShieldCreate(void);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* SHIELD_H_ */
