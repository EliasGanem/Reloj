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
static digital_output_p led_1;
/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */
static void ConfigureSystick(void);
/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

static void ConfigureSystick(void) {
    SystemCoreClockUpdate();
    SysTick_Config((SystemCoreClock / 1000) - 1);

    // NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
}

/* === Public function implementation ========================================================= */

int main(void) {
    // uint8_t value[] = {3, 2, 1, 0};

    // shield_p shield = ShieldCreate();
    led_1 = DigitalOutputCreate(LED_1_GPIO, LED_1_BIT);

    ConfigureSystick();

    // DisplayWriteBCD(shield->display, value, sizeof(value));
    // DisplayBlinkingDigits(shield->display, 2, 3, 25);
    // DisplayDot(shield->display, 0, true, 20);
    // DisplayDot(shield->display, 1, true, 20);
    // DisplayDot(shield->display, 2, true, 40);
    // DisplayDot(shield->display, 3, true, 40);
}

void SysTick_Handler(void) {
    static int count = 1;
    if (count == 1000) {
        DigitalOutputToggle(led_1);
        count = 1;
    }
    count++;
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
