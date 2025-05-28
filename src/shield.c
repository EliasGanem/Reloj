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

#ifndef SHIELD_MAX_INSTANCE
#define SHIELD_MAX_INSTANCE 2
#endif

/* === Private data type declarations ============================================================================== */

#ifndef USE_DYNAMIC_MEMORY
static struct shield_s instances[SHIELD_MAX_INSTANCE] = {0}; //!< Array con los objetos pocho
#endif

/* === Private function declarations =============================================================================== */

#ifndef USE_DYNAMIC_MEMORY
/**
 * @brief Funcion para crear un poncho si no se usa memoria dinamica
 *
 * @return shield_p devuelve una referencia al poncho
 */
static shield_p CreateInstance();
#endif

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

#ifndef USE_DYNAMIC_MEMORY
static shield_p CreateInstance() {
    shield_p self = NULL;
    int i;

    for (i = 0; i < SHIELD_MAX_INSTANCE; i++) {
        if (!instances[i].used) {
            instances[i].used = true;
            self = &instances[i];
            break;
        }
    }

    return self;
}
#endif

shield_p ShieldCreate(void) {
    struct shield_s * self = NULL;

#ifdef SHIELD_MAX_INSTANCE
    self = malloc(sizeof(struct shield_s));
#else
    self = CreateInstance();
#endif
    if (self != NULL) {
        Chip_SCU_PinMuxSet(BUZZER_PORT, BUZZER_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | BUZZER_FUNC);
        self->buzzer = DigitalOutputCreate(BUZZER_GPIO, BUZZER_BIT);

        /******************/

        Chip_SCU_PinMuxSet(KEY_ACCEPT_PORT, KEY_ACCEPT_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_PULLUP | KEY_ACCEPT_FUNC);
        self->accept = DigitalInputCreate(KEY_ACCEPT_GPIO, KEY_ACCEPT_BIT, false);

        Chip_SCU_PinMuxSet(KEY_CANCEL_PORT, KEY_CANCEL_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_PULLUP | KEY_CANCEL_FUNC);
        self->cancel = DigitalInputCreate(KEY_CANCEL_GPIO, KEY_CANCEL_BIT, false);

        Chip_SCU_PinMuxSet(KEY_F1_PORT, KEY_F1_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_PULLUP | KEY_F1_FUNC);
        self->set_time = DigitalInputCreate(KEY_F1_GPIO, KEY_F1_BIT, false);

        Chip_SCU_PinMuxSet(KEY_F2_PORT, KEY_F2_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_PULLUP | KEY_F2_FUNC);
        self->set_alarm = DigitalInputCreate(KEY_F2_GPIO, KEY_F2_BIT, false);

        Chip_SCU_PinMuxSet(KEY_F3_PORT, KEY_F3_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_PULLUP | KEY_F3_FUNC);
        self->decrement = DigitalInputCreate(KEY_F3_GPIO, KEY_F3_BIT, false);

        Chip_SCU_PinMuxSet(KEY_F4_PORT, KEY_F4_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_PULLUP | KEY_F4_FUNC);
        self->incremet = DigitalInputCreate(KEY_F4_GPIO, KEY_F4_BIT, false);
    }

    return self;
}

/* === End of documentation ======================================================================================== */
