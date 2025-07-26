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
#include "state.h"
#include <stdbool.h>

#include "edusia_config.h" // solo para usar los leds

/* === Macros definitions ====================================================================== */

#ifndef TIME_TO_HOLD_TO_CHANGE_STATE_MS
#define TIME_TO_HOLD_TO_CHANGE_STATE_MS 300
#endif

/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

static void ConfigureSystick(void);

/**
 * @brief Función perteneciente a la Interface utilizada por el reloj, esta se encarga de prender la alarma
 *
 */
void TurnOnAlarm(void);

/**
 * @brief Función perteneciente a la Interface utilizada por el reloj, esta se encarga de apagar la alarma
 *
 */
void TurnOffAlarm(void);

/* === Public variable definitions ============================================================= */

//! Variable que tiene el estado actual del reloj
states_e current_state = invalid_time;

/* === Private variable definitions ============================================================ */

//! Referencia al poncho
static struct shield_s * shield;

//! Referencia al objeto reloj
static clock_p clock;

//! Contador de milisegundos, para tener un control de tiempo en main
static volatile uint32_t milliseconds = 0;

/* === Private function implementation ========================================================= */

static void ConfigureSystick(void) {
    SystemCoreClockUpdate();
    SysTick_Config((SystemCoreClock / 1000) - 1);

    // NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
}

void TurnOnAlarm(void) {
    DigitalOutputDeactivate(shield->buzzer); // Es activo en bajo
}

void TurnOffAlarm(void) {
    DigitalOutputActivate(shield->buzzer);
}

/* === Public function implementation ========================================================= */

int main(void) {

    uint32_t aux_15ms = 0;

    shield = ShieldCreate();
    DigitalOutputActivate(shield->buzzer);

    check_button_hold_p set_time = &(struct check_button_hold_s){
        .button = shield->set_time,
        .counter = 0,
        .time_to_hold = TIME_TO_HOLD_TO_CHANGE_STATE_MS,
    };
    check_button_hold_p set_alarm = &(struct check_button_hold_s){
        .button = shield->set_alarm,
        .counter = 0,
        .time_to_hold = TIME_TO_HOLD_TO_CHANGE_STATE_MS,
    };

    clock_alarm_driver_p alarm_driver = &(struct clock_alarm_driver_s){
        .TurnOnAlarm = TurnOnAlarm,
        .TurnOffAlarm = TurnOffAlarm,
    };

    clock = ClockCreate(1000, alarm_driver, 300);

    ConfigureSystick();
    ChangeState(shield, clock, invalid_time);

    while (1) {

        if ((milliseconds - aux_15ms) == 15) {
            aux_15ms = milliseconds;
            State(shield, clock, set_time, set_alarm);
        }
    }
}

void SysTick_Handler(void) {
    clock_time_u current_time;

    ClockNewTick(clock);
    milliseconds++;

    DisplayRefresh(shield->display);

    if (current_state == valid_time || current_state == invalid_time) {
        ClockGetTime(clock, &current_time);
        DisplayWriteBCD(shield->display, &current_time.bcd[2], sizeof(current_time.bcd)); //&current_time.bcd[2]
    }
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
