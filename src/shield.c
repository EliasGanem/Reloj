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

#include "board.h"

#include "shield.h"
#include "edusia_config.h"
#include "shield_config.h"
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

/**
 * @brief Funcion para configurar los pines de entrada de la placa recién creada
 *
 * @param self // referencia de la placa que se crea
 */
static void InputInit(struct shield_s * self);

/**
 * @brief Función para configurar los pines de salida de la placa recién creada
 *
 * @param self // referencia de la placa que se crea
 */
static void OutputInit(struct shield_s * self);

/**
 * @brief Funcion que configura los pines de habilitacion de los digitos y los inicializa apagados
 *
 */
static void DigitisInit(void);

/**
 * @brief Funcion que configura los pines de los segmentos de cada display y los inicializa apagados
 *
 */
static void SegmentsInit(void);

/**
 * @brief Función que implementa el apagado de todos los dígitos
 *
 */
static void TurnOffDigits(void);

/**
 * @brief Función que prende el digito que se le indica
 *
 * @param digit número de digito que se desea prender
 */
static void TurnOnDigit(uint8_t digit);

/**
 * @brief Función que apaga todo los segmentos y luego prende los segmentos que se le indican.
 *
 * @param segments
 */
static void UpdateSegments(uint8_t segments);

/* === Private variable definitions ================================================================================ */

//! Interfase para control del Display
static const struct display_controller_s display_driver = {
    .TurnOffDigits = TurnOffDigits,
    .TurnOnDigit = TurnOnDigit,
    .UpdateSegments = UpdateSegments,
};

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

static void InputInit(struct shield_s * self) {
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

static void OutputInit(struct shield_s * self) {
    Chip_SCU_PinMuxSet(RGB_RED_PORT, RGB_RED_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | RGB_RED_FUNC);
    self->buzzer = DigitalOutputCreate(RGB_RED_GPIO, RGB_RED_BIT);
}

static void DigitisInit(void) {
    Chip_GPIO_ClearValue(LPC_GPIO_PORT, DIGITS_GPIO, DIGITS_MASK);

    Chip_SCU_PinMuxSet(DIGIT_0_PORT, DIGIT_0_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | DIGIT_0_FUNC);
    Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, DIGIT_0_GPIO, DIGIT_0_BIT, true);

    Chip_SCU_PinMuxSet(DIGIT_1_PORT, DIGIT_1_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | DIGIT_1_FUNC);
    Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, DIGIT_1_GPIO, DIGIT_1_BIT, true);

    Chip_SCU_PinMuxSet(DIGIT_2_PORT, DIGIT_2_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | DIGIT_2_FUNC);
    Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, DIGIT_2_GPIO, DIGIT_2_BIT, true);

    Chip_SCU_PinMuxSet(DIGIT_3_PORT, DIGIT_3_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | DIGIT_3_FUNC);
    Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, DIGIT_3_GPIO, DIGIT_3_BIT, true);
}

static void SegmentsInit(void) {
    Chip_GPIO_ClearValue(LPC_GPIO_PORT, SEGMENTS_GPIO, SEGMENTS_MASK);
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, SEGMENT_DOT_GPIO, SEGMENT_DOT_BIT, false);

    Chip_SCU_PinMuxSet(SEGMENT_A_PORT, SEGMENT_A_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SEGMENT_A_FUNC);
    Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, SEGMENT_A_GPIO, SEGMENT_A_BIT, true);

    Chip_SCU_PinMuxSet(SEGMENT_B_PORT, SEGMENT_B_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SEGMENT_B_FUNC);
    Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, SEGMENT_B_GPIO, SEGMENT_B_BIT, true);

    Chip_SCU_PinMuxSet(SEGMENT_C_PORT, SEGMENT_C_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SEGMENT_C_FUNC);
    Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, SEGMENT_C_GPIO, SEGMENT_C_BIT, true);

    Chip_SCU_PinMuxSet(SEGMENT_D_PORT, SEGMENT_D_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SEGMENT_D_FUNC);
    Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, SEGMENT_D_GPIO, SEGMENT_D_BIT, true);

    Chip_SCU_PinMuxSet(SEGMENT_E_PORT, SEGMENT_E_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SEGMENT_E_FUNC);
    Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, SEGMENT_E_GPIO, SEGMENT_E_BIT, true);

    Chip_SCU_PinMuxSet(SEGMENT_F_PORT, SEGMENT_F_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SEGMENT_F_FUNC);
    Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, SEGMENT_F_GPIO, SEGMENT_F_BIT, true);

    Chip_SCU_PinMuxSet(SEGMENT_G_PORT, SEGMENT_G_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SEGMENT_G_FUNC);
    Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, SEGMENT_G_GPIO, SEGMENT_G_BIT, true);

    Chip_SCU_PinMuxSet(SEGMENT_DOT_PORT, SEGMENT_DOT_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SEGMENT_DOT_FUNC);
    Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, SEGMENT_DOT_GPIO, SEGMENT_DOT_BIT, true);
}

static void TurnOffDigits(void) {
    Chip_GPIO_ClearValue(LPC_GPIO_PORT, DIGITS_GPIO, DIGITS_MASK);
}

static void TurnOnDigit(uint8_t digit) {
    Chip_GPIO_SetValue(LPC_GPIO_PORT, DIGITS_GPIO, (1 << (digit)) & DIGITS_MASK); // (3 - digit)
}

static void UpdateSegments(uint8_t segments) {
    Chip_GPIO_ClearValue(LPC_GPIO_PORT, SEGMENTS_GPIO, SEGMENTS_MASK);
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, SEGMENT_DOT_GPIO, SEGMENT_DOT_BIT, false);

    Chip_GPIO_SetValue(LPC_GPIO_PORT, SEGMENTS_GPIO, (segments & SEGMENTS_MASK));
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, SEGMENT_DOT_GPIO, SEGMENT_DOT_BIT, (segments & SEGMENT_DOT_MASK));
}

/* === Public function definition ================================================================================== */

shield_p ShieldCreate(void) {
    struct shield_s * self = NULL;

    BoardSetup();
    BoardSetup();

#ifdef SHIELD_MAX_INSTANCE
    self = malloc(sizeof(struct shield_s));
#else
    self = CreateInstance();
#endif

    if (self != NULL) {
        DigitisInit();
        SegmentsInit();

        InputInit(self);
        OutputInit(self);

        self->display = DisplayCreate(4, &display_driver);
    }

    return self;
}

/* === End of documentation ======================================================================================== */
