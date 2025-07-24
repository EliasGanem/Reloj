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

/** @file state_task.c
 ** @brief Definiciones de la biblioteca para la gestión de estados - Electrónica 4 2025
 **/

/* === Headers files inclusions ==================================================================================== */

#include "FreeRTOS.h"
#include "state_task.h"

#include "shield_config.h"
#include "config.h"
#include "clock.h"

/* === Macros definitions ========================================================================================== */

#define StateTaskDelay

/* === Private data type declarations ============================================================================== */

/* === Private function declarations =============================================================================== */

static void ChangeState(state_task_arg_p args, states_e new_state);

/**
 * @brief Funcion para incrementar los digitos de un númeroo en formato array cada vez que es llamada y realiza el
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
 * @brief Funcion para decrementar los digitos de un númeroo en formato array cada vez que es llamada y realiza el
 * control de si se alcanzó el limite establecido.
 *
 * El el LS digit es el 0 y el MS digit es el tamaño del array-1.
 *
 * @param array array a modificarl
 * @param limits_array array con los limites maximos del número
 * @param size tamaño de los array
 */
static void DecrementControl(uint8_t * array, uint8_t * array_limits, int size);

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

static void ChangeState(state_task_arg_p args, states_e new_state) {
    args->state = new_state;
    switch (new_state) {
    case invalid_time:
        xEventGroupSetBits(args->other_event_group, WRITE_FLAG);
        DisplayBlinkingDigits(args->display, 0, 3, 50);
        DisplayDot(args->display, 0, false, 0);
        DisplayDot(args->display, 1, false, 0);
        DisplayDot(args->display, 2, true, 50);
        DisplayDot(args->display, 3, false, 0);
        break;

    case valid_time:
        xEventGroupSetBits(args->other_event_group, WRITE_FLAG);
        DisplayBlinkingDigits(args->display, 0, 3, 0);
        if (ClockIsAlarmRinging(args->clock)) {
            DisplayDot(args->display, 0, true, 0);
        } else {
            DisplayDot(args->display, 0, false, 0);
        }
        DisplayDot(args->display, 1, false, 0);
        DisplayDot(args->display, 2, true, 500);
        if (ClockIsAlarmActivated(args->clock)) {
            DisplayDot(args->display, 3, true, 0);
        } else {
            DisplayDot(args->display, 3, false, 0);
        }
        break;

    case adjust_time_minutes:
        xEventGroupClearBits(args->other_event_group, WRITE_FLAG);
        DisplayBlinkingDigits(args->display, 0, 1, 50);
        DisplayDot(args->display, 2, true, 0);
        break;

    case adjust_time_hours:
        xEventGroupClearBits(args->other_event_group, WRITE_FLAG);
        DisplayBlinkingDigits(args->display, 2, 3, 50);
        break;

    case adjust_alarm_minutes:
        xEventGroupClearBits(args->other_event_group, WRITE_FLAG);
        DisplayBlinkingDigits(args->display, 0, 1, 50);
        DisplayDot(args->display, 0, true, 100);
        DisplayDot(args->display, 1, true, 100);
        DisplayDot(args->display, 2, true, 100);
        DisplayDot(args->display, 3, true, 100);
        break;

    case adjust_alarm_hours:
        xEventGroupClearBits(args->other_event_group, WRITE_FLAG);
        DisplayBlinkingDigits(args->display, 2, 3, 50);
        DisplayDot(args->display, 0, true, 100);
        DisplayDot(args->display, 1, true, 100);
        DisplayDot(args->display, 2, true, 100);
        DisplayDot(args->display, 3, true, 100);
        break;

    default:
        break;
    }
}

static void IncrementControl(uint8_t * array, uint8_t * array_limits, int size) {
    bool increment = false;

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

static void CanceledAdjustTime(state_task_arg_p args) {
    if (ClockGetTime(args->clock, &args->new_time)) {
        ChangeState(args, valid_time);
    } else {
        ChangeState(args, invalid_time);
    }
}

/* === Public function definitions ================================================================================= */

void StateTask(void * pointer) {
    state_task_arg_p args = pointer;
    EventBits_t events;
    ChangeState(args, invalid_time);
    uint8_t minutes_limit[2] = {0, 6};
    uint8_t hours_limit[2] = {4, 2};

    while (1) {
        //! espero un evento ¿Que passa si llegan 2 eventos? ademas events no es propia de la tarea hay reentrancia
        events = xEventGroupWaitBits(args->buttons_event_group, BUTTONS_EVENT, pdTRUE, pdFALSE, portMAX_DELAY);

        // if (events & DIDNT_PRESS_EVENT) {
        //     if (ClockGetTime(args->clock, &args->new_time)) {
        //         ChangeState(args, valid_time);
        //     } else {
        //         ChangeState(args, invalid_time);
        //     }
        // }

        switch (args->state) {
        case invalid_time:
            if (events & HOLD_SET_TIME_EVENT) {
                ChangeState(args, adjust_time_minutes);
                ClockGetTime(args->clock, &args->new_time);
            }
            break;
            //  case valid_time:
            //      if (KeepedHoldButton(set_time)) {
            //          ChangeState(shield, adjust_time_minutes);
            //          ClockGetTime(clock, &new_time);
            //      } else if (KeepedHoldButton(set_alarm)) {
            //          ChangeState(shield, adjust_alarm_minutes);
            //          ClockGetAlarm(clock, &new_time);
            //          new_time.bcd[0] = 0; // Para que los segundos no afecten la alarma
            //          new_time.bcd[1] = 0; // Para que los segundos no afecten la alarma
            //      } else if (ClockIsAlarmRinging(clock)) {
            //          if (DigitalInputWasActivated(shield->cancel)) {
            //              ClockTurnOffAlarm(clock);
            //              ChangeState(shield, valid_time);
            //          } else if (DigitalInputWasActivated(shield->accept)) {
            //              ClockSnoozeAlarm(clock);
            //              ChangeState(shield, valid_time);
            //          }
            //          ChangeState(shield, valid_time); // para activar el punto del 1er digito
            //      } else if (ClockGetAlarm(clock, &new_time)) {
            //          if (!ClockIsAlarmSnoozed(clock)) {
            //              if (DigitalInputWasActivated(shield->accept)) {
            //                  ClockSetAlarmState(clock, true);
            //                  ChangeState(shield, valid_time);
            //              } else if (DigitalInputWasActivated(shield->cancel)) {
            //                  ClockSetAlarmState(clock, false);
            //                  ChangeState(shield, valid_time);
            //              }
            //          }
            //      }

            //     break;

        case adjust_time_minutes:
            if (events & INCREMENT_EVENT) {
                IncrementControl(&args->new_time.bcd[2], minutes_limit, 2);
            } else if (events & DECREMENT_EVENT) {
                DecrementControl(&args->new_time.bcd[2], minutes_limit, 2);
            } else if (events & ACCEPT_EVENT) {
                ChangeState(args, adjust_time_hours);
            } else if (events & CANCEL_EVENT) {
                CanceledAdjustTime(args);
            }

            DisplayWriteBCD(args->display, &args->new_time.bcd[2], sizeof(args->new_time.bcd));

            // if (Passed30s()) {
            //     CanceledAdjustTime(shield, clock);
            // }
            break;

        case adjust_time_hours:
            if (events & INCREMENT_EVENT) {
                IncrementControl(&args->new_time.bcd[4], hours_limit, 2);
            } else if (events & DECREMENT_EVENT) {
                DecrementControl(&args->new_time.bcd[4], hours_limit, 2);
            } else if (events & CANCEL_EVENT) {
                CanceledAdjustTime(args);
            } else if (events & ACCEPT_EVENT) {
                if (ClockSetTime(args->clock, &args->new_time)) {
                    ChangeState(args, valid_time);
                } else {
                    ChangeState(args, invalid_time);
                }
            }
            DisplayWriteBCD(args->display, &args->new_time.bcd[2], sizeof(args->new_time.bcd));

            // if (Passed30s()) {
            //     CanceledAdjustTime(shield, clock);
            // }
            break;

            // case adjust_alarm_minutes:
            //     if (DigitalInputWasActivated(shield->incremet)) {
            //         IncrementControl(&new_time.bcd[2], minutes_limit, 2);
            //     } else if (DigitalInputWasActivated(shield->decrement)) {
            //         DecrementControl(&new_time.bcd[2], minutes_limit, 2);
            //     } else if (DigitalInputWasActivated(shield->accept)) {
            //         ChangeState(shield, adjust_alarm_hours);
            //     } else if (DigitalInputWasActivated(shield->cancel)) {
            //         ChangeState(shield, valid_time);
            //     }

            //     DisplayWriteBCD(shield->display, &new_time.bcd[2], sizeof(new_time.bcd));

            //     if (Passed30s()) {
            //         ChangeState(shield, valid_time);
            //     }
            //     break;

            // case adjust_alarm_hours:
            //     if (DigitalInputWasActivated(shield->incremet)) {
            //         IncrementControl(&new_time.bcd[4], hours_limit, 2);
            //     } else if (DigitalInputWasActivated(shield->decrement)) {
            //         DecrementControl(&new_time.bcd[4], hours_limit, 2);
            //     } else if (DigitalInputWasActivated(shield->cancel)) {
            //         ChangeState(shield, valid_time);
            //     } else if (DigitalInputWasActivated(shield->accept)) {
            //         ClockSetAlarm(clock, &new_time);
            //         ChangeState(shield, valid_time);
            //     }
            //     DisplayWriteBCD(shield->display, &new_time.bcd[2], sizeof(new_time.bcd));

            //     if (Passed30s()) {
            //         ChangeState(shield, valid_time);
            //     }
            //     break;

            // default:
            //     break;
        }
    }
}

/* === End of documentation ======================================================================================== */
