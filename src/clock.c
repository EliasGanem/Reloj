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

/** @file clock.c
 ** @brief Código fuente de la biblioteca reloj - Electrónica 4 2025
 **/

/* === Headers files inclusions ==================================================================================== */

#include "clock.h"
#include <stddef.h>
#include <string.h>

/* === Macros definitions ========================================================================================== */

/* === Private data type declarations ============================================================================== */

struct clock_s {
    clock_time_u current_time;         //!< hora actual
    clock_time_u current_alarm;        //!< hora de la alarma
    uint32_t current_alarm_in_seconds; //!< hora de la alarma en segundos desde las 00:00:00
    bool valid;                        //!< indica si la hora del reloj es valida
    bool alarm_set;                    //!< indica si alarma se configuro alguna vez
    bool alarm_is_ringing;             //!< indica si la alarma esta sonado
    bool alarm_is_activated;           //!< indica si la alarma esta desactivada
    bool snooze_alarm;                 //!< indica si se pospuso la alarma
    uint32_t seconds_counter;          //!< cantidad de segundos desde las 00:00:00
    uint32_t seconds_snoozed;          //!< cantidad de segundos que se pospone la alarma
    uint16_t ticks_per_second;         //!< cantidad de llamadas a @ref ClockNewTick que equivalen a un segundo
    uint16_t ticks_counter;            //!< canntidad de veces que se llamó a @ref ClockNewTick
    clock_alarm_driver_p alarm_driver; //! punteros a función para controlar la alarma
};

/* === Private function declarations =============================================================================== */

/**
 * @brief Funcion que verifica si la hora es valida, la hora está en un array
 *
 * Su objetivo es facilitar la lectura de @ref ClockSetTime().
 *
 * @param time tiempo
 * @return retorna true si el la hora es valida.
 */
static bool ValidTime(const clock_time_u * time);

/**
 * @brief Funcion que convierte un uint8_t en bcd
 *
 * Es resultado respeta siguiente formato: las unidades están en el índice 0, las decenas en el ínidce 1, ..
 *
 * @param bcd arreglo de uint8_t en el que almacena el resultado.
 */
static void Uint8ToBCD(uint8_t, uint8_t * bcd);

/**
 * @brief Función para convertir Segundos a hora,minutos y segundos en formato BCD
 *
 * @param seconds segundos a convertir en tiempo
 * @param time array de 6 elementos donde se guarda el tiempo en dormato bcd
 */
static void SecondsToTime(uint32_t seconds, uint8_t * time);

/**
 * @brief Funcion para convertir tiempo expresado en un array a segundos,  donde el LSB es la unidad de segundos y el
 * MSB es la decena de horas.
 *
 * No verifica la validez de argumentos, pues es para mejorar legibilidad de código
 *
 * @param bcd_time puntero del array a converit en segundos
 * @return resultado de segundos que equivalen al tiempo
 */
static uint32_t BcdTimeToSeconds(const uint8_t * bcd_time);

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

static bool ValidTime(const clock_time_u * time) {
    bool result = true;

    if ((time->bcd[5] >= 2) && (time->bcd[4] > 4)) {
        result = false;
    } else if ((time->bcd[3] >= 6) && (time->bcd[2] > 0)) {
        result = false;
    } else if ((time->bcd[1] >= 6) && (time->bcd[0] > 0)) {
        result = false;
    }

    return result;
}

static void Uint8ToBCD(uint8_t integer, uint8_t * bcd) {
    for (int i = 0; i < 8; i++) {
        bcd[i] = integer % 10;
        integer = integer / 10;
    }
}

static void SecondsToTime(uint32_t seconds, uint8_t * time) {
    uint8_t minutes, hours = 0;
    uint8_t bcd[8] = {0};

    hours = (seconds / 3600);
    Uint8ToBCD(hours, bcd);
    time[4] = bcd[0];
    time[5] = bcd[1];

    minutes = (seconds / 60) % 60;
    Uint8ToBCD(minutes, bcd);
    time[2] = bcd[0];
    time[3] = bcd[1];

    seconds = seconds % 60;
    Uint8ToBCD(seconds, bcd);
    time[0] = bcd[0];
    time[1] = bcd[1];
}

static uint32_t BcdTimeToSeconds(const uint8_t * time) {
    uint32_t seconds;

    seconds = time[0];
    seconds += 10 * time[1];
    seconds += 60 * time[2];
    seconds += 10 * 60 * time[3];
    seconds += 60 * 60 * time[4];
    seconds += 10 * 60 * 60 * time[5];

    return seconds;
}

/* === Public function definitions ================================================================================= */

clock_p ClockCreate(uint16_t ticks_per_second, clock_alarm_driver_p alarm_driver, uint32_t seconds_snoozed) {
    static struct clock_s self[1];
    clock_p result = self;

    if (seconds_snoozed > 86400) {
        result = NULL;
    } else if (alarm_driver == NULL) {
        result = NULL;
    } else if (alarm_driver->TurnOffAlarm == NULL || alarm_driver->TurnOnAlarm == NULL) {
        result = NULL;
    } else {
        memset(self, 0, sizeof(struct clock_s));
        self->valid = false;
        self->alarm_set = false;
        self->alarm_is_ringing = false;
        self->alarm_is_activated = false;
        self->snooze_alarm = false;
        self->seconds_snoozed = seconds_snoozed;
        self->ticks_per_second = ticks_per_second;
        self->alarm_driver = alarm_driver;
    }

    return result;
}

int ClockGetTime(clock_p self, clock_time_u * result) {
    int aux = 0;

    SecondsToTime(self->seconds_counter, self->current_time.bcd);
    memcpy(result, &self->current_time, sizeof(clock_time_u));

    if (self->valid) {
        aux = 1;
    }

    return aux;
}

int ClockSetTime(clock_p self, const clock_time_u * new_time) {
    int result = 1;

    if (!ValidTime(new_time)) {
        result = 0;
    } else {
        self->valid = true;
        memcpy(&self->current_time, new_time, sizeof(clock_time_u));
        self->seconds_counter = BcdTimeToSeconds(new_time->bcd);
    }

    return result;
}

void ClockNewTick(clock_p self) {
    static uint32_t snooze_counter = 0;

    self->ticks_counter++;

    if (self->ticks_counter == self->ticks_per_second) {
        self->ticks_counter = 0;
        self->seconds_counter++;
        if (self->snooze_alarm) {
            snooze_counter++;
        }
    }

    if (self->seconds_counter == 86400) {
        self->seconds_counter = 0;
    }

    if (snooze_counter == self->seconds_snoozed) {
        snooze_counter = 0;
        self->snooze_alarm = false;
        self->alarm_is_ringing = true;
        self->alarm_driver->TurnOnAlarm();
    }

    // Activa alarma
    if ((self->seconds_counter == self->current_alarm_in_seconds) && self->alarm_is_activated) {
        self->alarm_is_ringing = true;
        self->alarm_driver->TurnOnAlarm();
    }
}

int ClockSetAlarm(clock_p self, const clock_time_u * new_alarm) {
    int result = 1;

    if (!ValidTime(new_alarm)) {
        result = 0;
    } else {
        self->alarm_set = true;
        self->alarm_is_activated = true;

        memcpy(&self->current_alarm, new_alarm, sizeof(clock_time_u));
        self->current_alarm_in_seconds = BcdTimeToSeconds(self->current_alarm.bcd);
    }

    return result;
}

int ClockGetAlarm(clock_p self, clock_time_u * current_alarm) {

    memcpy(current_alarm, &self->current_alarm, sizeof(clock_time_u));

    return self->alarm_set;
}

int ClockIsAlarmRinging(clock_p self) {
    int result = 0;

    if (self->alarm_is_ringing) {
        result = 1;
    }

    return result;
}

void ClockSetAlarmState(clock_p self, bool activate) {
    self->alarm_is_activated = activate;
}

int ClockIsAlarmActivated(clock_p self) {
    int result = 0;

    if (self->alarm_is_activated) {
        result = 1;
    }

    return result;
}

void ClockSnoozeAlarm(clock_p self) {
    self->snooze_alarm = true;
    ClockTurnOffAlarm(self);
}

void ClockTurnOffAlarm(clock_p self) {
    self->alarm_is_ringing = false;
    self->alarm_driver->TurnOffAlarm();
}

uint32_t ClockGetTimeInSeconds(clock_p self) {
    return self->seconds_counter;
}

int ClockIsAlarmSnoozed(clock_p self) {
    int result = 0;
    if (self->snooze_alarm) {
        result = 1;
    }
    return result;
}
/* === End of documentation ======================================================================================== */
