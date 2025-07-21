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

#include "FreeRTOS.h"
#include "task.h"
// #include "queue.h"

#include "button_task.h"
#include "shield_config.h"

#include "shield.h"
#include "clock.h"
#include <stdbool.h>
#include <stdlib.h>

#include "edusia_config.h" // solo para usar los leds

/* === Macros definitions ====================================================================== */

#define ACCEPT_BUTTON BUTTONS_EVENT_BUTTON_0;
#define CANCEL_BUTTON BUTTONS_EVENT_BUTTON_1;

/* === Private data type declarations ========================================================== */

typedef struct led_task_arg_s {
    EventGroupHandle_t event_group;
    int event_bit;
    digital_output_p led;
} * led_task_arg_p;

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/**
 * @brief Funcion perteneciente a la Interface utilizada por el reloj, esta se encarga de prender la alarma
 *
 */
void TurnOnAlarm(void);

/**
 * @brief Funcion perteneciente a la Interface utilizada por el reloj, esta se encarga de apagar la alarma
 *
 */
void TurnOffAlarm(void);

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

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

struct shield_s * shield;

/* === Private function implementation ========================================================= */

void TurnOnAlarm(void) {
    DigitalOutputDeactivate(shield->buzzer); // Es activo en bajo
}

void TurnOffAlarm(void) {
    DigitalOutputActivate(shield->buzzer);
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

void TurnOnLedTask(void * pointer) {
    led_task_arg_p args = pointer;
    EventBits_t events;

    while (true) {
        //  cuando pasa el evento limpia el bit correspondiente, y espera hasta que el evento se dé
        events = xEventGroupWaitBits(args->event_group, args->event_bit, true, false, portMAX_DELAY);
        if (events & args->event_bit) {
            DigitalOutputToggle(args->led);
            vTaskDelay(pdMS_TO_TICKS(500));
        }
    }
}

/* === Public function implementation ========================================================= */

int main(void) {

    // uint32_t aux_15ms = 0;
    // uint32_t aux_1s = 0;

    // uint8_t minutes_limit[2] = {0, 6};
    // uint8_t hours_limit[2] = {4, 2};

    shield = ShieldCreate();
    // DigitalOutputActivate(shield->buzzer);

    // clock_alarm_driver_p alarm_driver = &(struct clock_alarm_driver_s){
    //     .TurnOnAlarm = TurnOnAlarm,
    //     .TurnOffAlarm = TurnOffAlarm,
    // };

    // clock_p clock = ClockCreate(1000, alarm_driver, 300);

    EventGroupHandle_t buttons_event = xEventGroupCreate();
    BaseType_t result;

    if (buttons_event != NULL) {
        button_task_arg_p button_args = malloc(sizeof(*button_args));
        button_args->event_group = buttons_event;
        button_args->event_bit = ACCEPT_BUTTON;
        button_args->button = shield->accept;
        result = xTaskCreate(ButtonTask, "Accept", configMINIMAL_STACK_SIZE, button_args, tskIDLE_PRIORITY + 1, NULL);
    }
    if (result == pdPASS) {
        led_task_arg_p led_args = malloc(sizeof(*led_args));
        led_args->event_group = buttons_event;
        led_args->event_bit = ACCEPT_BUTTON;
        led_args->led = shield->buzzer;
        xTaskCreate(TurnOnLedTask, "Led", configMINIMAL_STACK_SIZE, led_args, tskIDLE_PRIORITY + 1, NULL);
    }

    vTaskStartScheduler();

    while (1) {
    }
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
