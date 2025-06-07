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

/**
 * @brief Función que apaga todos los dígitos de una pantalla.
 *
 * Este es un puntero a una función encargada de apagar todos los digitos de una pantalla de displays de 7 segmentos.
 *
 * @param void no recibe nada
 * @return no devuelve nada
 *
 */
typedef void (*turn_off_digits_p)(void);

/**
 * @brief Función que enciende los segmentos deseados de una pantalla.
 *
 * Este es un puntero a una función encargada de encender todos los segmentos que se le indica, inluyendo el punto.
 *
 * @param uint_8 recibe un byte donde el MSB correponde al punto, MSB-1 correponde al segmento F y el LSB corresponde al
 * LSB
 * @return no devuelve nada
 *
 */
typedef void (*update_segments_p)(uint8_t new_segments);

/**
 * @brief Función que enciende un digito de una pantalla.
 *
 * Este es un puntero a una función que se encarga de encender un digito de una pantalla.
 *
 * @param uint8_t indica que digito se dese prender, el digito correspondiente a la unidad es el 0.
 * @return no devuelve nada
 */
typedef void (*turn_on_digit_p)(uint8_t digit);

//! Interface Controlador para el Display
typedef struct display_controller_s {
    turn_off_digits_p TurnOffDigits;  //!< puntero a la función encargada de apagar los dígitos
    update_segments_p UpdateSegments; //!< puntero a la función encargada de prender los segmentos
    turn_on_digit_p TurnOnDigit;      //!< puntero a la función encargada de prender un dígito
} const * display_controller_p;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

/**
 * @brief Función que crea una referencia para el objeto display
 *
 * Se le debe indicar la cantidad de display de 7 segmentos que tiene la pantalla. Si la cantidad indicada es mayor a la
 * posible se utiliza la maxima posible.
 *
 * @param number_of_digits cantidad de dígitos
 * @return display_p referencia al objeto creado
 */
display_p DisplayCreate(uint8_t number_of_digits, display_controller_p driver);

/**
 * @brief Función que escribe números en el Display
 *
 * Se le indica que display usar y el array de u_int_8 con los números que se desea mostrar, no modifica el punto.
 *
 * @param display referencia al display que se va a usar
 * @param bcd_to_show puntero al array que contiene los números a mostar
 * @param size tamaño del array
 */
void DisplayWriteBCD(display_p display, uint8_t * bcd_to_show, uint8_t size);

/**
 * @brief Función que muestra el dígito i, en su proxima llamada muestra el dígito i+1. Se actualiza automatimente
 * y muestra lo que contiene la memoria de video correspondiente al digito i.
 *
 * @param display referencia a la pantalla
 */
void DisplayRefresh(display_p display);

/**
 * @brief Función para configurar un parpadero en los dígitos desde @ref from hasta @ref to.
 *
 * El primer display es el número 0.
 * Si la variable @ref number_of_calls es cero entonces se apaga el parpadeo en los dígitos desde from hasta to.
 * En caso de que @ref to sea mayor que la cantidad de dígitos parapadearán los dígitos desde @ref from hasta la
 * cantidad máxima de digitios. En caso de que from > to, o que from > cantidad de display la función retorna -1.
 *
 * @param display referencia al display
 * @param from indica desde qué dígito se desea que parapadeen los display
 * @param to indica hasta qué dígito se desea que parapadeen los display
 * @param number_of_calls varaible que indica la cantidad de veces que se desea que esten apagados/prendidos los
 * dígitos cuando se llama a la función de refresco
 * @return int
 */
int DisplayBlinkingDigits(display_p display, uint8_t from, uint8_t to, uint16_t number_of_calls);

/**
 * @brief Función para controlar independientemente cada punto del display
 *
 * Se le indica la pantalla, el dígito al que pertenece el punto que se desea controlar.
 * Permite prender, apagar y hacer parpaderar el punto. Para el parpadeo se debe indicar la cantidad de llamadas a la
 * función que se desee que esté en apagado/prendido. Si @ref number_of_calls es cero entonces no parpadea, es caso de
 * estar prendido. El valor de @ref number_of_calls puede ser cualquiera cuando se apaga el punto.
 *
 * @param display display que se conrtola
 * @param digit número del dígito al cual pertenece el punto a controlar
 * @param turn_on indica si se quiere que el punto este prendido
 * @param number_of_calls si es cero indica que no parpadea, si es otro valor da la cantidad de veces que se desea que
 * al llamar a DisplayRefresh el punto este apagado
 * @return devuelve -1 si el numero del dígito es mayor a la cantidad de display disponibles
 */
int DisplayDot(display_p display, uint8_t digit, bool turn_on, uint16_t number_of_calls);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_H_ */
