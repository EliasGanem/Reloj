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
#include "edusia_config.h"
#include "chip.h"
#include <stdbool.h>

/* === Macros definitions ====================================================================== */

/* === Private data type declarations ========================================================== */

//! Representa los estados en lo que puede estar el reloj
typedef enum states_e {
    show_time,
    ajust_time,
    ajust_alarm,
} * states_p;

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

static void ConfigureSystick(void);

// static void ChangeState(states_p current_state);

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

//! Contador de milisegundos, para tener un control de tiempo en main
static volatile uint32_t milliseconds = 0;

//! Led para probar que anda el SysTick
static digital_output_p led_1;
static digital_output_p led_2;
static digital_output_p led_3;

/* === Private function implementation ========================================================= */

static void ConfigureSystick(void) {
    SystemCoreClockUpdate();
    SysTick_Config((SystemCoreClock / 1000) - 1);

    // NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
}

/* === Public function implementation ========================================================= */

int main(void) {
    // static states_p current_state = &(enum states_e){show_time};
    uint32_t aux_1s = 0;
    uint32_t aux_1ms = 0;
    uint32_t aux_10ms = 0;

    uint8_t value[4] = {1, 2, 3, 4};

    shield_p shield = ShieldCreate();
    led_1 = DigitalOutputCreate(LED_1_GPIO, LED_1_BIT);
    led_2 = DigitalOutputCreate(LED_2_GPIO, LED_2_BIT);
    led_3 = DigitalOutputCreate(LED_3_GPIO, LED_3_BIT);

    ConfigureSystick();

    while (1) {
        DisplayWriteBCD(shield->display, value, sizeof(value));
        if (milliseconds == 86400000) {
            milliseconds = milliseconds - aux_1s; // para mantener el 1[s] que es mas importante que el 1[ms]
        }

        if ((milliseconds - aux_1ms) == 1) {
            DisplayRefresh(shield->display);
            aux_1ms = milliseconds;
        }

        if ((milliseconds - aux_10ms) == 10) {
            aux_10ms = milliseconds;
            if (DigitalInputGetIsActive(shield->accept)) {
                DigitalOutputActivate(led_3);
            } else if (!DigitalInputGetIsActive(shield->accept)) {
                DigitalOutputDeactivate(led_3);
            }
        }

        if ((milliseconds - aux_1s) == 1000) {
            DigitalOutputToggle(led_2);
            aux_1s = milliseconds;
        }
    }

    // DisplayBlinkingDigits(shield->display, 2, 3, 25);
}

void SysTick_Handler(void) {
    static int count = 1;
    if (count == 1000) {
        DigitalOutputToggle(led_1);
        count = 1;
    }
    count++;

    milliseconds++;
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
