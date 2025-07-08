/* Copyright 2022, Laboratorio de Microprocesadores
 * Facultad de Ciencias Exactas y Tecnología
 * Universidad Nacional de Tucuman
 * http://www.microprocesadores.unt.edu.ar/
 * Copyright 2022, Esteban Volentini <evolentini@herrera.unt.edu.ar>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/** \brief Simple sample of use LPC HAL gpio functions
 **
 ** \addtogroup samples Sample projects
 ** \brief Sample projects to use as a starting point
 ** @{ */

/* === Headers files inclusions =============================================================== */

#include "shield.h"
#include "clock.h"
#include "chip.h"
#include <stdbool.h>

#include "edusia_config.h" // solo para usar los leds

/* === Macros definitions ====================================================================== */

#ifndef TIME_TO_HOLD_TO_CHANGE_STATE_MS
#define TIME_TO_HOLD_TO_CHANGE_STATE_MS 300
#endif

/* === Private data type declarations ========================================================== */

//! Representa los estados en lo que puede estar el reloj
typedef enum {
    valid_time,
    invalid_time,
    adjust_time_hours,
    adjust_time_minutes,
    adjust_alarm,
} states_e;

//! Struct que contiene los parametros de la funcion @ref CheckButtonHoldTime
typedef struct check_button_hold_s {
    const digital_input_p button; //!< botón
    uint32_t counter;             //!< contador propio del boton
    const uint32_t time_to_hold;  //!< tiempo que se desea que se controle el boton.
} * check_button_hold_p;

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

static void ConfigureSystick(void);

/**
 * @brief Función para realizar el cambio de estado de la MEF del reloj
 *
 * Se encarga de verificar si el estado siguiente se valido
 *
 * @param next_state
 * @return retorna el estado siguiente de la MEF, si el cambio de estado es posible.
 * Si el cambio es invalido devuelve el mismo estado en el que esta
 *
 */
static void ChangeState(shield_p ShieldCreate, states_e next_state);

/**
 * @brief Funcion para mejorar legibilidad de main
 * Se encarga de verificar que los botones se mantienen pulsados el tiempo necesario
 * @param check_values
 * @return retorna:
 *  \li 1 si se mantuvo presionado el botón el tiempo necesario
 *  \li 0 si no se lo mantuvo presionado lo suficiente
 */
static bool KeepedHoldButton(check_button_hold_p check_values);

void TurnOnAlarm(clock_p clock);  // No deberia ser publica?
void TurnOffAlarm(clock_p clock); // No deberia ser publica?

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

/**
 * @brief Funcion para evitar codigo repetido. Si se aprieta el boton cancelar en el ajuste de hora o alarma este hace
 * el cambio de estado correspondiente
 *
 * @param clock
 */
static void CanceledAdjust(shield_p shield, clock_p clock);
/* === Public variable definitions ============================================================= */

static clock_time_u new_time;
static states_e current_state = invalid_time;

/* === Private variable definitions ============================================================ */

//! Referencia al objeto reloj
static clock_p clock;

//! Contador de milisegundos, para tener un control de tiempo en main
static volatile uint32_t milliseconds = 0;

//! Led para probar que anda el SysTick
static digital_output_p led_1;
static digital_output_p led_2;
static digital_output_p led_3;
static digital_output_p led_b;

/* === Private function implementation ========================================================= */

static void ConfigureSystick(void) {
    SystemCoreClockUpdate();
    SysTick_Config((SystemCoreClock / 1000) - 1);

    // NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
}

static void ChangeState(shield_p shield, states_e next_state) {
    switch (next_state) {
    case invalid_time:
        current_state = invalid_time;
        DigitalOutputDeactivate(led_1);
        DigitalOutputDeactivate(led_2);
        DigitalOutputDeactivate(led_3);
        DigitalOutputActivate(led_b);
        DisplayBlinkingDigits(shield->display, 0, 3, 50);
        DisplayDot(shield->display, 2, true, 50);
        break;

    case valid_time:
        current_state = valid_time;
        DigitalOutputActivate(led_1);
        DigitalOutputDeactivate(led_2);
        DigitalOutputDeactivate(led_3);
        DisplayBlinkingDigits(shield->display, 0, 3, 0);
        DisplayDot(shield->display, 2, true, 500);
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

    case adjust_alarm:
        DigitalOutputActivate(led_3);
        DigitalOutputDeactivate(led_1);
        DigitalOutputDeactivate(led_2);
        break;

    default:
        DigitalOutputDeactivate(led_1);
        DigitalOutputDeactivate(led_2);
        DigitalOutputDeactivate(led_3);
        break;
    }
}

static bool KeepedHoldButton(check_button_hold_p check_values) {
    bool result = 0;
    if (DigitalInputGetIsActive(check_values->button) && check_values->counter < check_values->time_to_hold) {
        check_values->counter++;
        if (check_values->counter == check_values->time_to_hold) {
            check_values->counter = 0;
            result = 1;
        }
    } else {
        check_values->counter = 0;
        result = 0;
    }
    return result;
}

void TurnOnAlarm(clock_p clock) {
    (void)clock;
}

void TurnOffAlarm(clock_p clock) {
    (void)clock;
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

static void CanceledAdjust(shield_p shield, clock_p clock) {
    if (ClockGetTime(clock, &new_time)) {
        ChangeState(shield, valid_time);
    } else {
        ChangeState(shield, invalid_time);
    }
}
/* === Public function implementation ========================================================= */

int main(void) {

    uint32_t aux_1s = 0;
    uint32_t aux_30s = 0;
    uint32_t aux_1ms = 0;
    uint32_t aux_10ms = 0;

    uint8_t minutes_limit[2] = {0, 6};
    uint8_t hours_limit[2] = {4, 2};

    shield_p shield = ShieldCreate();

    // preguntar: no lo puedo hacer static xq un argumento se crea en t de ejecucion
    volatile check_button_hold_p set_time = &(struct check_button_hold_s){
        .button = shield->set_time,
        .counter = 0,
        .time_to_hold = TIME_TO_HOLD_TO_CHANGE_STATE_MS,
    };
    volatile check_button_hold_p set_alarm = &(struct check_button_hold_s){
        .button = shield->set_alarm,
        .counter = 0,
        .time_to_hold = TIME_TO_HOLD_TO_CHANGE_STATE_MS,
    };

    led_1 = DigitalOutputCreate(LED_1_GPIO, LED_1_BIT);
    led_2 = DigitalOutputCreate(LED_2_GPIO, LED_2_BIT);
    led_3 = DigitalOutputCreate(LED_3_GPIO, LED_3_BIT);
    led_b = DigitalOutputCreate(LED_B_GPIO, LED_B_BIT);
    DigitalOutputDeactivate(led_1);
    DigitalOutputDeactivate(led_2);
    DigitalOutputDeactivate(led_3);
    DigitalOutputDeactivate(led_b);

    clock_alarm_driver_p alarm_driver = &(struct clock_alarm_driver_s){
        .TurnOnAlarm = TurnOnAlarm,
        .TurnOffAlarm = TurnOffAlarm,
    };

    clock = ClockCreate(1000, alarm_driver, 300);
    clock_time_u current_time;

    ConfigureSystick();
    ChangeState(shield, invalid_time);

    while (1) {
        if ((milliseconds - aux_1ms) == 1) {
            aux_1ms = milliseconds;
            DisplayRefresh(shield->display);

            if (current_state == valid_time || current_state == invalid_time) {
                ClockGetTime(clock, &current_time);
                DisplayWriteBCD(shield->display, &current_time.bcd[2], sizeof(current_time.bcd)); //&current_time.bcd[2]
            }
        }

        if ((milliseconds - aux_10ms) == 10) {
            aux_10ms = milliseconds;

            switch (current_state) {
            case invalid_time:
                if (KeepedHoldButton(set_time)) {
                    ChangeState(shield, adjust_time_minutes);
                    ClockGetTime(clock, &new_time);
                }
                break;
            case valid_time:
                if (KeepedHoldButton(set_time)) {
                    ChangeState(shield, adjust_time_minutes);
                }
                if (KeepedHoldButton(set_alarm)) {
                    ChangeState(shield, adjust_alarm);
                }
                break;

            case adjust_time_minutes:
                if (DigitalInputWasActivated(shield->incremet)) {
                    IncrementControl(&new_time.bcd[2], minutes_limit, 2);
                    aux_30s = 0;
                } else if (DigitalInputWasActivated(shield->decrement)) {
                    DecrementControl(&new_time.bcd[2], minutes_limit, 2);
                    aux_30s = 0;
                } else if (DigitalInputWasActivated(shield->accept)) {
                    ChangeState(shield, adjust_time_hours);
                    aux_30s = 0;
                } else if (DigitalInputWasActivated(shield->cancel)) {
                    CanceledAdjust(shield, clock);
                    aux_30s = 0;
                }

                DisplayWriteBCD(shield->display, &new_time.bcd[2], sizeof(new_time.bcd));
                aux_30s++;
                if (aux_30s == 3000) {
                    CanceledAdjust(shield, clock);
                    aux_30s = 0;
                }
                break;

            case adjust_time_hours:
                if (DigitalInputWasActivated(shield->incremet)) {
                    IncrementControl(&new_time.bcd[4], hours_limit, 2);
                    aux_30s = 0;
                } else if (DigitalInputWasActivated(shield->decrement)) {
                    DecrementControl(&new_time.bcd[4], hours_limit, 2);
                    aux_30s = 0;
                } else if (DigitalInputWasActivated(shield->cancel)) {
                    CanceledAdjust(shield, clock);
                    aux_30s = 0;
                } else if (DigitalInputWasActivated(shield->accept)) {
                    if (ClockSetTime(clock, &new_time)) {
                        ChangeState(shield, valid_time);
                    } else {
                        ChangeState(shield, invalid_time);
                    }
                    aux_30s = 0;
                }
                DisplayWriteBCD(shield->display, &new_time.bcd[2], sizeof(new_time.bcd));
                aux_30s++;
                if (aux_30s == 3000) {
                    CanceledAdjust(shield, clock);
                    aux_30s = 0;
                }
                break;

            case adjust_alarm:
                if (DigitalInputGetIsActive(shield->accept) || DigitalInputGetIsActive(shield->cancel)) {
                    current_state = valid_time;
                }
                ChangeState(shield, current_state);
                break;

            default:
                break;
            }
        }

        if ((milliseconds - aux_1s) == 1000) {
            aux_1s = milliseconds;
        }
    }
}

void SysTick_Handler(void) {
    ClockNewTick(clock);
    milliseconds++;
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
