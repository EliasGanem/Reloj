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

#ifndef CLOCK_H_
#define CLOCK_H_

/** @file clock.h
 ** @brief Declaraciones del modulo de reloj - Electrónica 4 2025
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

//! Tipo de dato con la referencia a un reloj
typedef struct clock_s * clock_p;

//! Union para representar la hora en formato BCD y como Array
typedef union {
    struct {
        uint8_t seconds[2];
        uint8_t minutes[2];
        uint8_t hours[2];
    } time;

    uint8_t bcd[6];
} clock_time_u;

/**
 * @brief Puntero a una función que enciende la alarma correspondiente al reloj.
 *
 * @param clock referencia al reloj del que se habla.
 */
typedef void (*turn_on_alarm_p)(clock_p clock);

/**
 * @brief Puntero a una función que apaga la alarma correspondiente al reloj.
 *
 * @param clock referencia al reloj del que se habla.
 */
typedef void (*turn_off_alarm_p)(clock_p clock);

typedef struct clock_alarm_driver_s {
    turn_off_alarm_p TurnOffAlarm;
    turn_on_alarm_p TurnOnAlarm;
} const * clock_alarm_driver_p;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

/**
 * @brief Función para crear un reloj
 *
 * Solo se puede crear un único reloj
 *
 * @param ticks_per_second es la cantidad de veces que se llama a @ref ClockNewTick que equivalena  un segundo.
 * @param alarm_driver puntero a una función que enciende una alarma
 * @param seconds_snoozed cantidad de segundos que el reloj pospondrá la alarma
 * @return retorna NULL si la cantidad de segundos que se puede posponer es invalida
 */
clock_p ClockCreate(uint16_t ticks_per_second, clock_alarm_driver_p alarm_driver, uint32_t seconds_snoozed);

/**
 * @brief Función para obtener la hora actual.
 *
 * Devuelve la hora actual por referencia con la variable @p current_time y además devuelve un entero para indicar si
 * es la hora es valida.
 *
 * @param clock referencia al reloj
 * @param current_time variable en la que devuelve la hora actual
 * @return devuelve:
 *  \li 1 si el reloj se puso en hora valida
 *  \li 0 si hay que poner en hora el reloj
 */
int ClockGetTime(clock_p clock, clock_time_u * current_time);

/**
 * @brief Función para configurar la hora en el reloj
 *
 * @param clock referencia del reloj
 * @param new_time valor ed la nueva hora del reloj
 * @return devuelve 1 si ajusto la hora, si devuelve 0 es porque la hora es invalida
 */
int ClockSetTime(clock_p clock, const clock_time_u * new_time);

void ClockNewTick(clock_p clock);

/**
 * @brief Función para poner la alarma
 *
 * La hora a la que se pone la alarma debe ser valida, en caso de no serlo se deja el horario anterior. Si nunca se puso
 * un horario para la alarma y la hora enviada es invalida entonces queda por defecto en 00:00:00
 *
 * @param clock referencia al reloj
 * @param new_alarm hora
 * @return devuelve
 *  \li 1 si se definio un hoario para la alarma
 *  \li 0 en caso de que la hora para la alarma sea invalida
 */
int ClockSetAlarm(clock_p clock, const clock_time_u * new_alarm);

/**
 * @brief Función para obtener el horario en el que la alarma suena
 *
 * @param clock referencia del reloj
 * @param current_alarm variable pasada por referencia, en esta se devuelve la hora actual
 * @return devuelde:
 *  \li 0 en caso de que nunca se le dio un horario a la alarma
 *  \li 1 si se puso la alarma
 */
int ClockGetAlarm(clock_p clock, clock_time_u * current_alarm);

/**
 * @brief Función para saber si la alarma del reloj esta sonando
 *
 * @param clock referencia al reloj
 * @return devuelve un entero cuyo valor es:
 *  \li 1 si la alarma esta encendida
 *  \li 0 si la alarma esta apagada
 */
int ClockIsAlarmRinging(clock_p clock);

/**
 * @brief Función para activar y desactivar la alarma.
 *
 * @param clock referenia al reloj
 * @param state si es:
 *  \li true activa la alarma
 *  \li falso desactiva la alarma
 */
void ClockSetAlarmState(clock_p clock, bool state);

/**
 * @brief Función para saber si la alarma esta activa
 *
 * @param clock
 * @return int
 */
int ClockIsAlarmActivated(clock_p clock);

/**
 * @brief Función para posponer la alarma
 *
 * @param clock referencia del reloj
 */
void ClockSnoozeAlarm(clock_p clock);

/**
 * @brief Función para apagar la alarma, esta sonorá al dia siguiente
 *
 * @param clock referencia al reloj
 */
void ClockTurnOffAlarm(clock_p clock);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* CLOCK_H_ */
