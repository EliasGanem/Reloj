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

/** @file shield.c
 *
 * Se debe incluir el archivo .h de la placa, con la configuracion de los pines
 *
 ** @brief Codigo fuente del modulo de poncho para el proyecto reloj - Electrónica 4 2025
 **/

/* === Headers files inclusions ==================================================================================== */

#include "shield.h"
#include "edusia_config.h"
#include "poncho_config.h"
#include "chip.h"
#include <stddef.h>
#include <stdlib.h>

/* === Macros definitions ========================================================================================== */

/* === Private data type declarations ============================================================================== */

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

shield_p ShieldCreate(void) {
    struct shield_s * self = NULL;

    self = malloc(sizeof(struct shield_s));

    if (self != NULL) {
        Chip_SCU_PinMuxSet(LED_R_PORT, LED_R_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | LED_R_FUNC);
        self->led_red = DigitalOutputCreate(LED_R_GPIO, LED_R_BIT);

        Chip_SCU_PinMuxSet(LED_G_PORT, LED_G_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | LED_G_FUNC);
        self->led_green = DigitalOutputCreate(LED_G_GPIO, LED_G_BIT);

        Chip_SCU_PinMuxSet(LED_B_PORT, LED_B_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | LED_B_FUNC);
        self->led_blue = DigitalOutputCreate(LED_B_GPIO, LED_B_BIT);

        /******************/
        Chip_SCU_PinMuxSet(LED_1_PORT, LED_1_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | LED_1_FUNC);
        self->led_1 = DigitalOutputCreate(LED_1_GPIO, LED_1_BIT);

        Chip_SCU_PinMuxSet(LED_2_PORT, LED_2_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | LED_2_FUNC);
        self->led_2 = DigitalOutputCreate(LED_2_GPIO, LED_2_BIT);

        Chip_SCU_PinMuxSet(LED_3_PORT, LED_3_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | LED_3_FUNC);
        self->led_3 = DigitalOutputCreate(LED_3_GPIO, LED_3_BIT);

        /******************/
        Chip_SCU_PinMuxSet(TEC_1_PORT, TEC_1_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_PULLUP | TEC_1_FUNC);
        self->button_1 = DigitalInputCreate(TEC_1_GPIO, TEC_1_BIT, true);

        Chip_SCU_PinMuxSet(TEC_2_PORT, TEC_2_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_PULLUP | TEC_2_FUNC);
        self->button_2 = DigitalInputCreate(TEC_2_GPIO, TEC_2_BIT, true);

        Chip_SCU_PinMuxSet(TEC_3_PORT, TEC_3_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_PULLUP | TEC_3_FUNC);
        self->button_3 = DigitalInputCreate(TEC_3_GPIO, TEC_3_BIT, true);

        Chip_SCU_PinMuxSet(TEC_4_PORT, TEC_4_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_PULLUP | TEC_4_FUNC);
        self->button_4 = DigitalInputCreate(TEC_4_GPIO, TEC_4_BIT, true);

        Chip_SCU_PinMuxSet(KEY_ACCEPT_PORT, KEY_ACCEPT_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_PULLUP | KEY_ACCEPT_FUNC);
        self->accept = DigitalInputCreate(KEY_ACCEPT_GPIO, KEY_ACCEPT_BIT, false);

        Chip_SCU_PinMuxSet(KEY_CANCEL_PORT, KEY_CANCEL_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_PULLUP | KEY_CANCEL_FUNC);
        self->cancel = DigitalInputCreate(KEY_CANCEL_GPIO, KEY_CANCEL_BIT, false);

        Chip_SCU_PinMuxSet(KEY_F1_PORT, KEY_F1_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_PULLUP | KEY_F1_FUNC);
        self->f1 = DigitalInputCreate(KEY_F1_GPIO, KEY_F1_BIT, false);

        Chip_SCU_PinMuxSet(KEY_F2_PORT, KEY_F2_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_PULLUP | KEY_F2_FUNC);
        self->f2 = DigitalInputCreate(KEY_F2_GPIO, KEY_F2_BIT, false);

        Chip_SCU_PinMuxSet(KEY_F3_PORT, KEY_F3_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_PULLUP | KEY_F3_FUNC);
        self->f3 = DigitalInputCreate(KEY_F3_GPIO, KEY_F3_BIT, false);

        Chip_SCU_PinMuxSet(KEY_F4_PORT, KEY_F4_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_PULLUP | KEY_F4_FUNC);
        self->f4 = DigitalInputCreate(KEY_F4_GPIO, KEY_F4_BIT, false);
    }

    return self;
}

/* === End of documentation ======================================================================================== */
