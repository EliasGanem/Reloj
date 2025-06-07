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

/** @file test_clock.c
 ** @brief Código para testeo de biblioteca reloj - Electrónica 4 2025
 **/

/**
 * Pruebas a realizar
- Al inicializar el reloj está en 00:00 y con hora invalida.
- Al ajustar la hora el reloj queda en hora y es válida.
- Después de n ciclos de reloj la hora avanza un segundo, diez segundos, un minutos, diez minutos, una hora, diez horas
y un día completo.
- Fijar la hora de la alarma y consultarla.
- Fijar la alarma y avanzar el reloj para que suene.
- Fijar la alarma, deshabilitarla y avanzar el reloj para no suene.
- Hacer sonar la alarma y posponerla.
- Hacer sonar la alarma y cancelarla hasta el otro dia..
- Probar que devulve ClockGetTime cuando le pido la hora
- Ajustar la hora con valores invalidso y ver que los rechaza.
- Verificar cuando no puede crear el reloj
- Probar reloj con una frecuencia distinta
- Ver que al superar 24hs, vuelva a cero
 *
 */

/* === Headers files inclusions ==================================================================================== */

#include "unity.h"

#include "clock.h"

/* === Macros definitions ========================================================================================== */

#define CLOCK_TICKS_PER_SECONDS 5
#define TEST_ASSERT_TIME(hours_ten, hours_unit, minutes_ten, minutes_unit, seconds_ten, seconds_unit)                  \
    clock_time_u current_time = {0};                                                                                   \
    TEST_ASSERT_TRUE_MESSAGE(ClockGetTime(clock, &current_time), "Clock has invalid time");                            \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(seconds_unit, current_time.bcd[0], "Diference in the unit of seconds");            \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(seconds_ten, current_time.bcd[1], "Diference in the ten of seconds");              \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(minutes_unit, current_time.bcd[2], "Diference in the unit of minutes");            \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(minutes_ten, current_time.bcd[3], "Diference in the ten of minutes");              \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(hours_unit, current_time.bcd[4], "Diference in the unit of hours");                \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(hours_ten, current_time.bcd[5], "Diference in the ten of hours");

/* === Private data type declarations ============================================================================== */

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

clock_p clock;

/* === Private function definitions ================================================================================ */

void setUp(void) {
    clock = ClockCreate(CLOCK_TICKS_PER_SECONDS);
}

static void SimulateSeconds(clock_p clock, uint8_t seconds) {
    for (uint8_t i = 0; i < CLOCK_TICKS_PER_SECONDS * seconds; i++) {
        ClockNewTick(clock);
    }
}

/* === Public function definitions ===+============================================================================= */

// Al inicializar el reloj está en 00:00 y con hora invalida.
void test_init_with_invalid_time(void) {
    clock_time_u current_time = {
        .bcd = {1, 2, 3, 4, 5, 6},
    };

    clock_p local_clock = ClockCreate(CLOCK_TICKS_PER_SECONDS);

    TEST_ASSERT_FALSE(ClockGetTime(local_clock, &current_time));
    TEST_ASSERT_EACH_EQUAL_UINT8(0, current_time.bcd, 6);
}

// Al ajustar la hora el reloj queda en hora y es válida.
void test_set_up_and_adjust_with_valid_time(void) {
    static const clock_time_u new_time = {
        .time = {.hours = {4, 1}, .minutes = {5, 3}, .seconds = {2, 1}},
    };

    TEST_ASSERT_TRUE(ClockSetTime(clock, &new_time));
    TEST_ASSERT_TIME(1, 4, 3, 5, 1, 2);
}

// Después de n ciclos de reloj la hora avanza un segundo
void test_clock_advance_one_second(void) {

    ClockSetTime(clock, &(clock_time_u){0});
    SimulateSeconds(clock, 1);

    TEST_ASSERT_TIME(0, 0, 0, 0, 0, 1);
}

// Después de n ciclos de reloj la hora avanza 10 segundos
void test_clock_advance_ten_seconds(void) {

    ClockSetTime(clock, &(clock_time_u){0});
    SimulateSeconds(clock, 10);

    TEST_ASSERT_TIME(0, 0, 0, 0, 1, 0);
}

/* === End of documentation ======================================================================================== */
