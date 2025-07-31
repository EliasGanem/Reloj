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

/** @file state.c
 ** @brief Codigo del control de estados - Electrónica 4 2025
 **/

/* === Headers files inclusions ==================================================================================== */

#include "state.h"

/* === Macros definitions ========================================================================================== */

/* === Private data type declarations ============================================================================== */

/* === Private function declarations =============================================================================== */

/**
 * @brief Función para incrementar los digitos de un númeroo en formato array cada vez que es llamada y realiza el
 * control de si se alcanzó el limite establecido.
 *
 * El el LS digit es el 0 y el MS digit es el tamaño del array-1.
 *
 * @param array array a modificarl
 * @param limits_array array con los limites maximos del número
 * @param size tamaño de los array
 */
static void IncrementControl(uint8_t * array, uint8_t * array_limits, int size);

/**
 * @brief Función para decrementar los digitos de un númeroo en formato array cada vez que es llamada y realiza el
 * control de si se alcanzó el limite establecido.
 *
 * El el LS digit es el 0 y el MS digit es el tamaño del array-1.
 *
 * @param array array a modificarl
 * @param limits_array array con los limites maximos del número
 * @param size tamaño de los array
 */
static void DecrementControl(uint8_t * array, uint8_t * array_limits, int size);

/**
 * @brief Funcion para mejorar legibilidad de main
 * Se encarga de verificar que los botones se mantienen pulsados el tiempo necesario
 * @param check_values
 * @return retorna:
 *  \li 1 si se mantuvo presionado el botón el tiempo necesario
 *  \li 0 si no se lo mantuvo presionado lo suficiente
 */
static bool KeepedHoldButton(check_button_hold_p check_values);

/**
 * @brief Función para evitar codigo repetido. Si se aprieta el boton cancelar en el ajuste de hora o alarma este hace
 * el cambio de estado correspondiente
 *
 * @param clock
 */
static void CanceledAdjustTime(shield_p shield, clock_p clock);

/**
 * @brief Función para evitar codigo repetido, se encarga de sumar 1 al contador de 30 segundos y ver si ya pasaron 30s
 *
 * @return retorna:
 *  \li 1 si pasaron 30s
 *  \li 0 si no pasaron
 */
static bool Passed30s(void);

/* === Private variable definitions ================================================================================ */

uint8_t minutes_limit[2] = {9, 5};
uint8_t hours_limit[2] = {3, 2};

//! Variable Auxiliar utilizada para guardar la hora que se elige al configurar la alarma o la hora
static clock_time_u new_time;

//! Varaible para saber cuando pasaron 30 segundos sin apretar un botón
static uint32_t aux_30s = 0;

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

static void IncrementControl(uint8_t * array, uint8_t * array_limits, int size) {
    bool increment = false;

    aux_30s = 0;

    for (int i = 0; i < size; i++) {
        if (array[i] != array_limits[i]) {
            increment = true;
        }
    }

    if (increment) {
        for (int i = 0; i < size; i++) {
            if (array[i] < 9) {
                array[i]++;
                for (int j = 0; j < i; j++) {
                    array[j] = 0;
                }
                i = size;
            }
        }
    } else {
        for (int i = 0; i < size; i++) {
            array[i] = 0;
        }
    }
}

static void DecrementControl(uint8_t * array, uint8_t * array_limits, int size) {
    bool decrement = false;

    aux_30s = 0;

    for (int i = 0; i < size; i++) {
        if (array[i] != 0) {
            decrement = true;
        }
    }

    if (decrement) {
        for (int i = 0; i < size; i++) {
            if (array[i] > 0) {
                array[i]--;
                for (int j = 0; j < i; j++) {
                    array[j] = 9;
                }
                i = size;
            }
        }
    } else {
        for (int i = 0; i < size; i++) {
            array[i] = array_limits[i];
        }
    }
}

static bool KeepedHoldButton(check_button_hold_p check_values) {
    bool result = 0;
    if (DigitalInputGetIsActive(check_values->button) && check_values->counter < (check_values->time_to_hold / 15)) {
        check_values->counter++;
        if (check_values->counter == (check_values->time_to_hold / 15)) { // time to hold / 15ms
            check_values->counter = 0;
            result = 1;
        }
    } else {
        check_values->counter = 0;
        result = 0;
    }
    return result;
}

static void CanceledAdjustTime(shield_p shield, clock_p clock) {
    if (ClockGetTime(clock, &new_time)) {
        ChangeState(shield, clock, valid_time);
    } else {
        ChangeState(shield, clock, invalid_time);
    }
    aux_30s = 0;
}

static bool Passed30s(void) {
    bool result = false;

    aux_30s++;
    if (aux_30s == 2000) { // 30000ms / 15ms
        result = true;
    }

    return result;
}

void ChangeState(shield_p shield, clock_p clock, states_e next_state) {
    aux_30s = 0;
    switch (next_state) {
    case invalid_time:
        current_state = invalid_time;
        DisplayBlinkingDigits(shield->display, 0, 3, 50);
        DisplayDot(shield->display, 0, false, 0);
        DisplayDot(shield->display, 1, false, 0);
        DisplayDot(shield->display, 2, true, 50);
        DisplayDot(shield->display, 3, false, 0);
        break;

    case valid_time:
        current_state = valid_time;
        DisplayBlinkingDigits(shield->display, 0, 3, 0);
        if (ClockIsAlarmRinging(clock)) {
            DisplayDot(shield->display, 0, true, 0);
        } else {
            DisplayDot(shield->display, 0, false, 0);
        }
        DisplayDot(shield->display, 1, false, 0);
        DisplayDot(shield->display, 2, true, 500);
        if (ClockIsAlarmActivated(clock)) {
            DisplayDot(shield->display, 3, true, 0);
        } else {
            DisplayDot(shield->display, 3, false, 0);
        }
        break;

    case adjust_time_minutes:
        current_state = adjust_time_minutes;
        DisplayBlinkingDigits(shield->display, 0, 1, 50);
        DisplayDot(shield->display, 2, true, 0);
        break;

    case adjust_time_hours:
        current_state = adjust_time_hours;
        DisplayBlinkingDigits(shield->display, 2, 3, 50);
        break;

    case adjust_alarm_minutes:
        current_state = adjust_alarm_minutes;
        DisplayBlinkingDigits(shield->display, 0, 1, 50);
        DisplayDot(shield->display, 0, true, 100);
        DisplayDot(shield->display, 1, true, 100);
        DisplayDot(shield->display, 2, true, 100);
        DisplayDot(shield->display, 3, true, 100);
        break;

    case adjust_alarm_hours:
        current_state = adjust_alarm_hours;
        DisplayBlinkingDigits(shield->display, 2, 3, 50);
        DisplayDot(shield->display, 0, true, 100);
        DisplayDot(shield->display, 1, true, 100);
        DisplayDot(shield->display, 2, true, 100);
        DisplayDot(shield->display, 3, true, 100);
        break;

    default:
        break;
    }
}

void State(shield_p shield, clock_p clock, check_button_hold_p set_time, check_button_hold_p set_alarm) {
    switch (current_state) {
    case invalid_time:
        if (KeepedHoldButton(set_time)) {
            ChangeState(shield, clock, adjust_time_minutes);
            ClockGetTime(clock, &new_time);
        }
        break;
    case valid_time:
        if (KeepedHoldButton(set_time)) {
            ChangeState(shield, clock, adjust_time_minutes);
            ClockGetTime(clock, &new_time);
        } else if (KeepedHoldButton(set_alarm)) {
            ChangeState(shield, clock, adjust_alarm_minutes);
            ClockGetAlarm(clock, &new_time);
            new_time.bcd[0] = 0; // Para que los segundos no afecten la alarma
            new_time.bcd[1] = 0; // Para que los segundos no afecten la alarma
        } else if (ClockIsAlarmRinging(clock)) {
            if (DigitalInputWasActivated(shield->cancel)) {
                ClockTurnOffAlarm(clock);
                ChangeState(shield, clock, valid_time);
            } else if (DigitalInputWasActivated(shield->accept)) {
                ClockSnoozeAlarm(clock);
                ChangeState(shield, clock, valid_time);
            }
            ChangeState(shield, clock, valid_time); // para activar el punto del 1er digito
        } else if (ClockGetAlarm(clock, &new_time)) {
            if (!ClockIsAlarmSnoozed(clock)) {
                if (DigitalInputWasActivated(shield->accept)) {
                    ClockSetAlarmState(clock, true);
                    ChangeState(shield, clock, valid_time);
                } else if (DigitalInputWasActivated(shield->cancel)) {
                    ClockSetAlarmState(clock, false);
                    ChangeState(shield, clock, valid_time);
                }
            }
        }

        break;

    case adjust_time_minutes:
        if (DigitalInputWasActivated(shield->incremet)) {
            IncrementControl(&new_time.bcd[2], minutes_limit, 2);
        } else if (DigitalInputWasActivated(shield->decrement)) {
            DecrementControl(&new_time.bcd[2], minutes_limit, 2);
        } else if (DigitalInputWasActivated(shield->accept)) {
            ChangeState(shield, clock, adjust_time_hours);
        } else if (DigitalInputWasActivated(shield->cancel)) {
            CanceledAdjustTime(shield, clock);
        }

        DisplayWriteBCD(shield->display, &new_time.bcd[2], sizeof(new_time.bcd));

        if (Passed30s()) {
            CanceledAdjustTime(shield, clock);
        }
        break;

    case adjust_time_hours:
        if (DigitalInputWasActivated(shield->incremet)) {
            IncrementControl(&new_time.bcd[4], hours_limit, 2);
        } else if (DigitalInputWasActivated(shield->decrement)) {
            DecrementControl(&new_time.bcd[4], hours_limit, 2);
        } else if (DigitalInputWasActivated(shield->cancel)) {
            CanceledAdjustTime(shield, clock);
        } else if (DigitalInputWasActivated(shield->accept)) {
            if (ClockSetTime(clock, &new_time)) {
                ChangeState(shield, clock, valid_time);
            } else {
                ChangeState(shield, clock, invalid_time);
            }
            aux_30s = 0;
        }
        DisplayWriteBCD(shield->display, &new_time.bcd[2], sizeof(new_time.bcd));

        if (Passed30s()) {
            CanceledAdjustTime(shield, clock);
        }
        break;

    case adjust_alarm_minutes:
        if (DigitalInputWasActivated(shield->incremet)) {
            IncrementControl(&new_time.bcd[2], minutes_limit, 2);
        } else if (DigitalInputWasActivated(shield->decrement)) {
            DecrementControl(&new_time.bcd[2], minutes_limit, 2);
        } else if (DigitalInputWasActivated(shield->accept)) {
            ChangeState(shield, clock, adjust_alarm_hours);
        } else if (DigitalInputWasActivated(shield->cancel)) {
            ChangeState(shield, clock, valid_time);
        }

        DisplayWriteBCD(shield->display, &new_time.bcd[2], sizeof(new_time.bcd));

        if (Passed30s()) {
            ChangeState(shield, clock, valid_time);
        }
        break;

    case adjust_alarm_hours:
        if (DigitalInputWasActivated(shield->incremet)) {
            IncrementControl(&new_time.bcd[4], hours_limit, 2);
        } else if (DigitalInputWasActivated(shield->decrement)) {
            DecrementControl(&new_time.bcd[4], hours_limit, 2);
        } else if (DigitalInputWasActivated(shield->cancel)) {
            ChangeState(shield, clock, valid_time);
        } else if (DigitalInputWasActivated(shield->accept)) {
            ClockSetAlarm(clock, &new_time);
            ChangeState(shield, clock, valid_time);
        }
        DisplayWriteBCD(shield->display, &new_time.bcd[2], sizeof(new_time.bcd));

        if (Passed30s()) {
            ChangeState(shield, clock, valid_time);
        }
        break;

    default:
        break;
    }
}

/* === End of documentation ======================================================================================== */
