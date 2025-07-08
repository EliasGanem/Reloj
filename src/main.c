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
    show_time,
    invalid_time,
    adjust_time,
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
 * @param current_state
 * @param next_state
 * @return retorna el estado siguiente de la MEF, si el cambio de estado es posible.
 * Si el cambio es invalido devuelve el mismo estado en el que esta
 *
 */
// static states_e ChangeState(states_e current_state, states_e next_state);

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

/* === Public variable definitions ============================================================= */

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

/* === Public function implementation ========================================================= */

int main(void) {
    states_e current_state = invalid_time;

    uint32_t aux_1s = 0;
    uint32_t aux_1ms = 0;
    uint32_t aux_10ms = 0;

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

    DisplayBlinkingDigits(shield->display, 0, 3, 50); // La 1ra vez que enciende Parpadea
    DisplayDot(shield->display, 2, true, 50);

    //  Al inicio veo cuanto tiempo pasó y al final hago lo que corresponde al estado
    while (1) {
        if ((milliseconds - aux_1ms) == 1) {
            aux_1ms = milliseconds;
            DisplayRefresh(shield->display);

            if (current_state == show_time || current_state == invalid_time) {
                ClockGetTime(clock, &current_time);
                DisplayWriteBCD(shield->display, current_time.bcd, sizeof(current_time.bcd)); //&current_time.bcd[2]
            }
        }

        if ((milliseconds - aux_10ms) == 10) {
            aux_10ms = milliseconds;

            switch (current_state) {
            case invalid_time:
                if (KeepedHoldButton(set_time)) {
                    current_state = adjust_time;
                }
                break;
            case show_time:
                if (KeepedHoldButton(set_time)) {
                    current_state = adjust_time;
                }
                if (KeepedHoldButton(set_alarm)) {
                    current_state = adjust_alarm;
                }

                break;
            case adjust_time:
                if (DigitalInputGetIsActive(shield->accept) || DigitalInputGetIsActive(shield->cancel)) {
                    if (ClockGetTime(clock, &current_time)) {
                        current_state = show_time;
                    } else {
                        current_state = invalid_time;
                    }
                }

                break;
            case adjust_alarm:
                if (DigitalInputGetIsActive(shield->accept) || DigitalInputGetIsActive(shield->cancel)) {
                    current_state = show_time;
                }

                break;

            default:
                break;
            }
        }

        if ((milliseconds - aux_1s) == 1000) {
            aux_1s = milliseconds;
        }

        switch (current_state) {
        case invalid_time:
            DigitalOutputDeactivate(led_1);
            DigitalOutputDeactivate(led_2);
            DigitalOutputDeactivate(led_3);
            DigitalOutputActivate(led_b);
            break;

        case show_time:
            DigitalOutputActivate(led_1);
            DigitalOutputDeactivate(led_2);
            DigitalOutputDeactivate(led_3);
            break;

        case adjust_time:
            DigitalOutputActivate(led_2);
            DigitalOutputDeactivate(led_1);
            DigitalOutputDeactivate(led_3);
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
}

void SysTick_Handler(void) {
    ClockNewTick(clock);
    milliseconds++;
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
