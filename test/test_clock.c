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
- Ver que al superar 24hs, vuelva a cero
- Ajustar la hora con valores invalidos y ver que los rechaza.
- Fijar la hora de la alarma y consultarla.
- Controlar que al inicio la alamar no este puesta
- Controlar que la hora que se setea en la alarma sea valida
- Fijar la alarma y avanzar el reloj para que suene.
- Fijar la alarma, deshabilitarla y avanzar el reloj para no suene.
- Hacer sonar la alarma y posponerla.
- Ver que la alarma vuelva a sonar X tiempo definido despues de ser pospuesta
- Ver que el timpo X que se define para que se posponga la alarma sea valido

- Hacer sonar la alarma y cancelarla hasta el otro dia..
- Ver que la alarma se apaga cuando pasan X minutos
- Ver que la alarma se apaga cuando  la posponen X minutos despues de ser pospuesta
- Probar que devulve ClockGetTime cuando le pido la hora
- Verificar cuando no puede crear el reloj
- Probar reloj con una frecuencia distinta
- Probar al crear el reloj la alarma no este pospuesta, activada ni sonando
- OBSERVACION nunca actualizo el struct de tiempo ni su bcd current_time
- Creo que hay un problema en snooze_counter == self->seconds_snoozed (linea 196) cuando no se pospone la alarma creo
que puede darse esta condicion
- Ver que pasa si se pospone 24hs
 *
 */

/* === Headers files inclusions ==================================================================================== */

#include "unity.h"

#include "clock.h"
#include <stdbool.h>

/* === Macros definitions ========================================================================================== */

#define CLOCK_TICKS_PER_SECONDS 5
#define CLOCK_SECONDS_OF_SNOOZE 300
#define TEST_ASSERT_TIME(hours_ten, hours_unit, minutes_ten, minutes_unit, seconds_ten, seconds_unit, desired_time)    \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(seconds_unit, desired_time.bcd[0], "Diference in the unit of seconds");            \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(seconds_ten, desired_time.bcd[1], "Diference in the ten of seconds");              \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(minutes_unit, desired_time.bcd[2], "Diference in the unit of minutes");            \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(minutes_ten, desired_time.bcd[3], "Diference in the ten of minutes");              \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(hours_unit, desired_time.bcd[4], "Diference in the unit of hours");                \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(hours_ten, desired_time.bcd[5], "Diference in the ten of hours");

/* === Private data type declarations ============================================================================== */

/* === Private function declarations =============================================================================== */

//! Funcion para simular el encendido de la alarma
static void TurnOnAlarm(clock_p clock);

//! Funcion para simular el apagado de la alarma
static void TurnOffAlarm(clock_p clock);

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

static clock_p clock;
static bool alarm_is_ringing = false;
static const struct clock_alarm_driver_s alarm_driver = {
    .TurnOnAlarm = TurnOnAlarm,
    .TurnOffAlarm = TurnOffAlarm,
};

/* === Private function definitions ================================================================================ */

void setUp(void) {
    clock = ClockCreate(CLOCK_TICKS_PER_SECONDS, &alarm_driver, CLOCK_SECONDS_OF_SNOOZE);
    alarm_is_ringing = false;
}

static void SimulateSeconds(clock_p clock, int seconds) {
    for (int i = 0; i < CLOCK_TICKS_PER_SECONDS * seconds; i++) {
        ClockNewTick(clock);
    }
}

static void TurnOnAlarm(clock_p clock) {
    alarm_is_ringing = true;
    (void)clock;
}

static void TurnOffAlarm(clock_p clock) {
    alarm_is_ringing = false;
    (void)clock;
}

/* === Public function definitions ===+============================================================================= */

// 1-Al inicializar el reloj está en 00:00 y con hora invalida.
void test_init_with_invalid_time(void) {
    clock_time_u current_time = {
        .bcd = {1, 2, 3, 4, 5, 6},
    };

    clock_p local_clock = ClockCreate(CLOCK_TICKS_PER_SECONDS, &alarm_driver, CLOCK_SECONDS_OF_SNOOZE);

    TEST_ASSERT_FALSE(ClockGetTime(local_clock, &current_time));
    TEST_ASSERT_EACH_EQUAL_UINT8(0, current_time.bcd, 6);
}

// 2-Al ajustar la hora el reloj queda en hora y es válida.
void test_set_up_and_adjust_with_valid_time(void) {
    static const clock_time_u new_time = {
        .time = {.hours = {4, 1}, .minutes = {5, 3}, .seconds = {2, 1}},
    };

    TEST_ASSERT_TRUE(ClockSetTime(clock, &new_time));

    clock_time_u current_time = {0};
    TEST_ASSERT_TRUE_MESSAGE(ClockGetTime(clock, &current_time), "Clock has invalid time");
    TEST_ASSERT_TIME(1, 4, 3, 5, 1, 2, current_time);
}

// 3-Después de n ciclos de reloj la hora avanza un segundo
void test_clock_advance_one_second(void) {

    ClockSetTime(clock, &(clock_time_u){0});
    SimulateSeconds(clock, 1);

    clock_time_u current_time = {0};
    TEST_ASSERT_TRUE_MESSAGE(ClockGetTime(clock, &current_time), "Clock has invalid time");
    TEST_ASSERT_TIME(0, 0, 0, 0, 0, 1, current_time);
}

// 4-Después de n ciclos de reloj la hora avanza 10 segundos
void test_clock_advance_ten_seconds(void) {

    ClockSetTime(clock, &(clock_time_u){0});
    SimulateSeconds(clock, 10);

    clock_time_u current_time = {0};
    TEST_ASSERT_TRUE_MESSAGE(ClockGetTime(clock, &current_time), "Clock has invalid time");
    TEST_ASSERT_TIME(0, 0, 0, 0, 1, 0, current_time);
}

// 5-Después de n ciclos de reloj la hora avanza 55 segundos
void test_clock_advance_fiftyfive_seconds(void) {

    ClockSetTime(clock, &(clock_time_u){0});
    SimulateSeconds(clock, 55);

    clock_time_u current_time = {0};
    TEST_ASSERT_TRUE_MESSAGE(ClockGetTime(clock, &current_time), "Clock has invalid time");
    TEST_ASSERT_TIME(0, 0, 0, 0, 5, 5, current_time);
}

// 6-Después de n ciclos de reloj la hora avanza un minuto
void test_clock_advance_one_minute(void) {

    ClockSetTime(clock, &(clock_time_u){0});
    SimulateSeconds(clock, 60);

    clock_time_u current_time = {0};
    TEST_ASSERT_TRUE_MESSAGE(ClockGetTime(clock, &current_time), "Clock has invalid time");
    TEST_ASSERT_TIME(0, 0, 0, 1, 0, 0, current_time);
}

// 7-Después de n ciclos de reloj la hora avanza 10 minutos
void test_clock_advance_ten_minutes(void) {

    ClockSetTime(clock, &(clock_time_u){0});
    SimulateSeconds(clock, 600);

    clock_time_u current_time = {0};
    TEST_ASSERT_TRUE_MESSAGE(ClockGetTime(clock, &current_time), "Clock has invalid time");
    TEST_ASSERT_TIME(0, 0, 1, 0, 0, 0, current_time);
}

// 8-Después de n ciclos de reloj la hora avanza 55 minutos
void test_clock_advance_fiftyfive_minutes(void) {

    ClockSetTime(clock, &(clock_time_u){0});
    SimulateSeconds(clock, 3300);

    clock_time_u current_time = {0};
    TEST_ASSERT_TRUE_MESSAGE(ClockGetTime(clock, &current_time), "Clock has invalid time");
    TEST_ASSERT_TIME(0, 0, 5, 5, 0, 0, current_time);
}

// 9-Después de n ciclos de reloj la hora avanza una hora
void test_clock_advance_one_hour(void) {

    ClockSetTime(clock, &(clock_time_u){0});
    SimulateSeconds(clock, 3600);

    clock_time_u current_time = {0};
    TEST_ASSERT_TRUE_MESSAGE(ClockGetTime(clock, &current_time), "Clock has invalid time");
    TEST_ASSERT_TIME(0, 1, 0, 0, 0, 0, current_time);
}

// 10-Después de n ciclos de reloj la hora avanza 10 horas
void test_clock_advance_ten_hours(void) {

    ClockSetTime(clock, &(clock_time_u){0});
    SimulateSeconds(clock, 36000);

    clock_time_u current_time = {0};
    TEST_ASSERT_TRUE_MESSAGE(ClockGetTime(clock, &current_time), "Clock has invalid time");
    TEST_ASSERT_TIME(1, 0, 0, 0, 0, 0, current_time);
}

// 11-Al superar 24hs vuelve a 00:00:00
void test_clock_advance_one_day(void) {

    ClockSetTime(clock, &(clock_time_u){0});
    SimulateSeconds(clock, 86400);

    clock_time_u current_time = {0};
    TEST_ASSERT_TRUE_MESSAGE(ClockGetTime(clock, &current_time), "Clock has invalid time");
    TEST_ASSERT_TIME(0, 0, 0, 0, 0, 0, current_time);
}

// 12-Ajustar la hora con valor invalido de horas y ver que no los guarda.
void test_set_up_with_invalid_hour(void) {
    ClockSetTime(clock, &(clock_time_u){0});

    static const clock_time_u new_time = {
        .time = {.hours = {5, 2}, .minutes = {0, 0}, .seconds = {0, 0}},
    };
    TEST_ASSERT_EQUAL_INT(0, ClockSetTime(clock, &new_time));

    clock_time_u current_time = {0};
    TEST_ASSERT_TRUE_MESSAGE(ClockGetTime(clock, &current_time), "Clock has invalid time");
    TEST_ASSERT_TIME(0, 0, 0, 0, 0, 0, current_time);
}

// 13-Ajustar la hora con valor invalido de minutos y ver que no los guarda.
void test_set_up_with_invalid_minutes(void) {
    ClockSetTime(clock, &(clock_time_u){0});

    static const clock_time_u new_time = {
        .time = {.hours = {0, 0}, .minutes = {1, 6}, .seconds = {0, 0}},
    };
    TEST_ASSERT_EQUAL_INT(0, ClockSetTime(clock, &new_time));

    clock_time_u current_time = {0};
    TEST_ASSERT_TRUE_MESSAGE(ClockGetTime(clock, &current_time), "Clock has invalid time");
    TEST_ASSERT_TIME(0, 0, 0, 0, 0, 0, current_time);
}

// 14-Ajustar la hora con valor invalido de segundos y ver que no los guarda.
void test_set_up_with_invalid_seconds(void) {
    ClockSetTime(clock, &(clock_time_u){0});

    static const clock_time_u new_time = {
        .time = {.hours = {0, 0}, .minutes = {0, 0}, .seconds = {1, 6}},
    };
    TEST_ASSERT_EQUAL_INT(0, ClockSetTime(clock, &new_time));

    clock_time_u current_time = {0};
    TEST_ASSERT_TRUE_MESSAGE(ClockGetTime(clock, &current_time), "Clock has invalid time");
    TEST_ASSERT_TIME(0, 0, 0, 0, 0, 0, current_time);
}

// 15-Fijar la hora de la alarma y consultarla
void test_set_alarm() {

    static const clock_time_u alarm = {
        .time = {.hours = {3, 1}, .minutes = {9, 5}, .seconds = {3, 4}},
    };
    TEST_ASSERT_EQUAL_INT(1, ClockSetAlarm(clock, &alarm));

    clock_time_u alarm_time = {0};
    TEST_ASSERT_EQUAL_INT(1, ClockGetAlarm(clock, &alarm_time));
    TEST_ASSERT_TIME(1, 3, 5, 9, 4, 3, alarm_time);
}

// 16-Controlar que al inicio la alamar no este puesta
void test_is_alarm_set() {
    clock_p local_clock = ClockCreate(CLOCK_TICKS_PER_SECONDS, &alarm_driver, CLOCK_SECONDS_OF_SNOOZE);

    clock_time_u alarm_time = {0};
    TEST_ASSERT_EQUAL_INT(0, ClockGetAlarm(local_clock, &alarm_time));
}

// 17-Controlar que la hora que se setea en la alarma sea valida por la hora
void test_set_alarm_with_valid_hours() {
    static const clock_time_u invalid_alarm = {
        .time = {.hours = {5, 2}, .minutes = {0, 0}, .seconds = {0, 0}},
    };
    TEST_ASSERT_EQUAL_INT(0, ClockSetAlarm(clock, &invalid_alarm));
}

// 18-Controlar que la hora que se setea en la alarma sea valida por los minutos
void test_set_alarm_with_valid_minutes() {
    static const clock_time_u invalid_alarm = {
        .time = {.hours = {0, 0}, .minutes = {5, 6}, .seconds = {0, 0}},
    };
    TEST_ASSERT_EQUAL_INT(0, ClockSetAlarm(clock, &invalid_alarm));
}

// 19-Controlar que la hora que se setea en la alarma sea valida por los segundos
void test_set_alarm_with_valid_seconds() {
    static const clock_time_u invalid_alarm = {
        .time = {.hours = {0, 0}, .minutes = {0, 0}, .seconds = {8, 9}},
    };
    TEST_ASSERT_EQUAL_INT(0, ClockSetAlarm(clock, &invalid_alarm));
}

// 20-Fijar la alarma y avanzar el reloj para que suene.
void test_that_the_alarm_turns_on_when_it_should() {

    ClockSetTime(clock, &(clock_time_u){0});
    alarm_is_ringing = false;

    static const clock_time_u valid_alarm = {
        .time = {.hours = {0, 0}, .minutes = {0, 0}, .seconds = {0, 1}},
    };
    ClockSetAlarm(clock, &valid_alarm);

    TEST_ASSERT_EQUAL_INT(0, ClockIsAlarmRinging(clock));
    TEST_ASSERT_FALSE(alarm_is_ringing);

    SimulateSeconds(clock, 10);

    TEST_ASSERT_EQUAL_INT(1, ClockIsAlarmRinging(clock)); // Esta bien esto?
    TEST_ASSERT_TRUE(alarm_is_ringing);

    alarm_is_ringing = false;
}

// 21-Fijar la alarma, deshabilitarla
void test_that_alarm_can_be_deactivated() {

    ClockSetTime(clock, &(clock_time_u){0});

    static const clock_time_u valid_alarm = {
        .time = {.hours = {0, 0}, .minutes = {0, 0}, .seconds = {0, 1}},
    };
    ClockSetAlarm(clock, &valid_alarm);
    TEST_ASSERT_EQUAL_INT(1, ClockIsAlarmActivated(clock));

    ClockDeactivateAlarm(clock);

    TEST_ASSERT_EQUAL_INT(0, ClockIsAlarmActivated(clock));
}

// 22-Ver que cuando la alarma esté seteada y deshabilitada no suene
void test_when_the_alarm_is_deactivate_it_does_not_sound() {

    ClockSetTime(clock, &(clock_time_u){0});

    static const clock_time_u valid_alarm = {
        .time = {.hours = {0, 0}, .minutes = {0, 0}, .seconds = {1, 1}},
    };
    ClockSetAlarm(clock, &valid_alarm);

    ClockDeactivateAlarm(clock);

    SimulateSeconds(clock, 11);
    TEST_ASSERT_EQUAL_INT(0, ClockIsAlarmRinging(clock));
    TEST_ASSERT_FALSE(alarm_is_ringing);
}

// 23-Hacer sonar la alarma y posponerla.
void test_when_the_alarm_sounds_it_can_be_snoozed_and_turned_off() {

    ClockSetTime(clock, &(clock_time_u){0});

    static const clock_time_u valid_alarm = {
        .time = {.hours = {0, 0}, .minutes = {0, 3}, .seconds = {0, 0}},
    };
    ClockSetAlarm(clock, &valid_alarm);

    SimulateSeconds(clock, 1800);
    TEST_ASSERT_EQUAL_INT(1, ClockIsAlarmRinging(clock));
    TEST_ASSERT_TRUE(alarm_is_ringing);

    SimulateSeconds(clock, 180);
    ClockSnoozeAlarm(clock);
    TEST_ASSERT_EQUAL_INT(0, ClockIsAlarmRinging(clock));
    TEST_ASSERT_FALSE(alarm_is_ringing);
}

// 24-Ver que la alarma suena X tiempo después de ser pospuesta
void test_alarm_sounds_after_be_snoozed() {

    ClockSetTime(clock, &(clock_time_u){0});

    static const clock_time_u valid_alarm = {
        .time = {.hours = {0, 0}, .minutes = {0, 3}, .seconds = {0, 0}},
    };
    ClockSetAlarm(clock, &valid_alarm);

    SimulateSeconds(clock, 1800); // pasaron 30 minutos
    SimulateSeconds(clock, 180);  // pasaron 3 minutos
    ClockSnoozeAlarm(clock);
    SimulateSeconds(clock, 300); // pasaron 3 minutos
    TEST_ASSERT_EQUAL_INT(1, ClockIsAlarmRinging(clock));
    TEST_ASSERT_TRUE(alarm_is_ringing);
}

// 25-Ver que el timpo X que se define para que se posponga la alarma sea valido
void test_that_the_snooze_time_is_valid() {

    clock_p local_clock = ClockCreate(CLOCK_TICKS_PER_SECONDS, &alarm_driver, 86401);
    TEST_ASSERT_NULL(local_clock);
}

// 26-Hacer sonar la alarma y apagarla hasta el otro dia..
void test_the_alarm_sounds_the_next_day_when_its_turned_off() {

    ClockSetTime(clock, &(clock_time_u){0});

    static const clock_time_u valid_alarm = {
        .time = {.hours = {0, 0}, .minutes = {0, 3}, .seconds = {0, 0}},
    };
    ClockSetAlarm(clock, &valid_alarm); // alarma a las 00:30:00

    SimulateSeconds(clock, 1860); // pasaron 31 minutos

    // Apaga la alarma
    ClockTurnOffAlarm(clock);
    SimulateSeconds(clock, 240); // pasaron 4 minutos
    TEST_ASSERT_EQUAL_INT(0, ClockIsAlarmRinging(clock));
    TEST_ASSERT_FALSE(alarm_is_ringing);

    SimulateSeconds(clock, 86100); // pasaron 24hs desde que sono la alarma
    SimulateSeconds(clock, 60);    // pasó 1 minuto

    TEST_ASSERT_EQUAL_INT(1, ClockIsAlarmRinging(clock));
    TEST_ASSERT_TRUE(alarm_is_ringing);
}

/* === End of documentation ======================================================================================== */
