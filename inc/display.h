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

#ifndef DISPLAY_H_
#define DISPLAY_H_

/** @file display.h
 *
 ** @brief Declaraciones del modulo display - Electrónica 4 2025
 **/

/* === Headers files inclusions ==================================================================================== */

#include <stdint.h>
#include <stdbool.h>

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

#define SEGMENT_A   (1 << 0)
#define SEGMENT_B   (1 << 1)
#define SEGMENT_C   (1 << 2)
#define SEGMENT_D   (1 << 3)
#define SEGMENT_E   (1 << 4)
#define SEGMENT_F   (1 << 5)
#define SEGMENT_G   (1 << 6)
#define SEGMENT_DOT (1 << 7)

/* === Public data type declarations =============================================================================== */

//! Tipo de dato que  tiene la referencia a una pantalla
typedef struct display_s * display_p;

//! Puntero a una función que apaga los digitos
typedef void (*turn_off_digits_p)(void);

//! Puntero a una función que prende los segmentos que se le indica, incluye el punto. Se le envía un byte cuyo LSB
//! corresponde al segmento a, el siguiente bit es el b. El MSB es el punto, el anterior corresponde al segmento g.
typedef void (*update_segments_p)(uint8_t new_segments);

//! Puntero a una función que pendre un digito
typedef void (*turn_on_digit_p)(uint8_t digit);

//! Interface Controlador para el Display
typedef struct display_controller_s {
    turn_off_digits_p TurnOffDigits;  //!< puntero a la funcion encargada de apagar los digitos
    update_segments_p UpdateSegments; //!< puntero a la funcion encargada de actualizar los segmentos
    turn_on_digit_p TurnOnDigit;      //!< puntero a la funcion encargada de prender un digito
} const * const display_controller_p;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

/**
 * @brief Funcion que crea una referencia para el objeto display
 *
 * Se le debe indicar la cantidad de display de 7 segmentos que tiene la pantalla. Si la cantidad indicada es mayor a la
 * posible se utiliza la maxima posible.
 *
 * @param number_of_digits cantidad de digitos
 * @return display_p referencia al objeto creado
 */
display_p DisplayCreate(uint8_t number_of_digits, display_controller_p driver);

/**
 * @brief Funcion que escribe numeros en el Display
 *
 * Se le indica que display usar y el array de u_int_8 con los numeros que se desea mostrar
 *
 * @param display referencia al display que se va a usar
 * @param value_to_show puntero al array que contiene los numeros a mostar
 * @param size tamañó del array
 */
void DisplayWriteBCD(display_p display, uint8_t * value_to_show, uint8_t size);

/**
 * @brief Funcion que muestra el digito i, en su proxima llamada muestra el dijito i+1. Se actualiza automatimente
 * y muestra lo que contiene la memoria de video de la pantalla.
 *
 * @param display referencia a la pantalla
 */
void DisplayRefresh(display_p display);

/**
 * @brief Funcion para configurar un parpadero en los digitos desde from hasta to.
 *
 * El primer display es el numero 0.
 * Si la variable "number_call_of" es cero entonces se apaga el parpadeo en los digitos desde from hasta to. En caso de
 * que to sea mayor que la cantidad de digitos parapadearán los digitos desde from hasta el cantidad de digitios En caso
 * de que from > to, o que from > cantidad de display la funcion retorna -1
 *
 * @param display
 * @param from
 * @param to
 * @param number_calls_off varaible que indica la cantidad de veces que se desea que esten apagados los digitos cuando
 * se llama a la función de refresco
 * @return int
 */
int DisplayBlinkingDigits(display_p display, uint8_t from, uint8_t to, uint16_t number_calls_off);

/**
 * @brief Funcion para controlar cada punto del display
 *
 * Se le indica el displya, el digito al que pertenece el punto que se desea hacer que parpadee y la cantidad de
 * llamadas a la funcion que debe estar en apagado
 *
 * @param display display que se conrtola
 * @param digit número del digito al cual pertenece el punto a controlar
 * @param turn_on indica si se quiere que el punto este prendido
 * @param number_call_off si es cero indica que no parpadea, si es otro valor da la cantidad de veces que se desea que
 * al llamar a DisplayRefresh el punto este apagado
 * @return devuelve -1 si el numero del digito es mayor a la cantidad de display disponibles
 */
int DisplayDot(display_p display, uint8_t digit, bool turn_on, uint16_t number_calls_off);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_H_ */
