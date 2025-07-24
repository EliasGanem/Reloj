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
#include "queue.h"
#include "semphr.h"

#include "shield_config.h"
#include "shield.h"
#include "clock.h"
#include <stdbool.h>
#include <stdlib.h>

#include "button_task.h"
#include "state_task.h"
#include "display_refresh_task.h"
#include "clock_tick_task.h"

/* === Macros definitions ====================================================================== */

#define BUTTON_TASK_PRIOTIRY (tskIDLE_PRIORITY + 1)

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
    DigitalOutputActivate(shield->buzzer);
    uint8_t invalid[6] = {9, 9, 9, 9, 9, 9};
    DisplayWriteBCD(shield->display, invalid, 6);

    clock_alarm_driver_p alarm_driver = &(struct clock_alarm_driver_s){
        .TurnOnAlarm = TurnOnAlarm,
        .TurnOffAlarm = TurnOffAlarm,
    };

    clock_p clock = ClockCreate(1000, alarm_driver, 300);

    EventGroupHandle_t buttons_event = xEventGroupCreate();
    EventGroupHandle_t other_event = xEventGroupCreate();
    //! Mutex para uso del display
    SemaphoreHandle_t display_mutex = xSemaphoreCreateMutex();
    SemaphoreHandle_t clock_mutex = xSemaphoreCreateMutex();
    BaseType_t result;

    if (buttons_event && display_mutex) {
        button_task_arg_p button_args = malloc(sizeof(*button_args));
        button_args->event_group = buttons_event;
        button_args->push_event = ACCEPT_EVENT;
        button_args->hold_event = HOLD_ACCEPT_EVENT;
        button_args->button = shield->accept;
        button_args->hold_time = 3000;
        result = xTaskCreate(ButtonTask, "Accept", BUTTON_TASK_STACK_SIZE, button_args, BUTTON_TASK_PRIOTIRY, NULL);
    }
    if (buttons_event != NULL) {
        button_task_arg_p button_args = malloc(sizeof(*button_args));
        button_args->event_group = buttons_event;
        button_args->push_event = CANCEL_EVENT;
        button_args->hold_event = HOLD_CANCEL_EVENT;
        button_args->button = shield->cancel;
        button_args->hold_time = 3000;
        result = xTaskCreate(ButtonTask, "Cancel", BUTTON_TASK_STACK_SIZE, button_args, BUTTON_TASK_PRIOTIRY, NULL);
    }
    if (buttons_event != NULL) {
        button_task_arg_p button_args = malloc(sizeof(*button_args));
        button_args->event_group = buttons_event;
        button_args->push_event = SET_TIME_EVENT;
        button_args->hold_event = HOLD_SET_TIME_EVENT;
        button_args->button = shield->set_time;
        button_args->hold_time = 3000;
        result = xTaskCreate(ButtonTask, "SetTime", BUTTON_TASK_STACK_SIZE, button_args, BUTTON_TASK_PRIOTIRY, NULL);
    }
    if (buttons_event != NULL) {
        button_task_arg_p button_args = malloc(sizeof(*button_args));
        button_args->event_group = buttons_event;
        button_args->push_event = SET_ALARM_EVENT;
        button_args->hold_event = HOLD_SET_ALARM_EVENT;
        button_args->button = shield->set_alarm;
        button_args->hold_time = 3000;
        result = xTaskCreate(ButtonTask, "SetAlarm", BUTTON_TASK_STACK_SIZE, button_args, BUTTON_TASK_PRIOTIRY, NULL);
    }
    if (buttons_event != NULL) {
        button_task_arg_p button_args = malloc(sizeof(*button_args));
        button_args->event_group = buttons_event;
        button_args->push_event = INCREMENT_EVENT;
        button_args->hold_event = HOLD_INCREMENT_EVENT;
        button_args->button = shield->incremet;
        button_args->hold_time = 3000;
        result = xTaskCreate(ButtonTask, "Increment", BUTTON_TASK_STACK_SIZE, button_args, BUTTON_TASK_PRIOTIRY, NULL);
    }
    if (buttons_event != NULL) {
        button_task_arg_p button_args = malloc(sizeof(*button_args));
        button_args->event_group = buttons_event;
        button_args->push_event = DECREMENT_EVENT;
        button_args->hold_event = HOLD_DECREMENT_EVENT;
        button_args->button = shield->decrement;
        button_args->hold_time = 3000;
        result = xTaskCreate(ButtonTask, "Decrement", BUTTON_TASK_STACK_SIZE, button_args, BUTTON_TASK_PRIOTIRY, NULL);
    }
    if (result == pdPASS) {
        state_task_arg_p state_args = malloc(sizeof(*state_args));
        state_args->display_mutex = display_mutex;
        state_args->buttons_event_group = buttons_event;
        state_args->buttons_event_group = other_event;
        // No le paso los eventos, estan en config
        state_args->buzzer = shield->buzzer;
        state_args->display = shield->display;
        state_args->clock = clock;
        result = xTaskCreate(StateTask, "States", STATE_TASK_STACK_SIZE, state_args, tskIDLE_PRIORITY + 2, NULL);
    }
    if (result == pdPASS) {
        display_refresh_task_arg_p display_args = malloc(sizeof(*display_args));
        display_args->display_mutex = display_mutex;
        display_args->display = shield->display;
        result = xTaskCreate(DisplayRefreshTask, "Refresh", DISPLAY_REFRESH_TASK_STACK_SIZE, display_args,
                             tskIDLE_PRIORITY + 9, NULL);
    }
    if (result == pdPASS) {
        clock_tick_task_arg_p clock_args = malloc(sizeof(*clock_args));
        clock_args->clock = clock;
        clock_args->clock_mutex = clock_mutex;
        clock_args->event_group = other_event;
        clock_args->second_event = SECOND_EVENT;
        result = xTaskCreate(ClockTickTask, "ClockTick", CLOCK_TICK_TASK_STACK_SIZE, clock_args, tskIDLE_PRIORITY + 10,
                             NULL);
    }
    if (result == pdPASS) {
        write_time_task_arg_p write_time_args = malloc(sizeof(*write_time_args));
        write_time_args->clock = clock;
        write_time_args->clock_mutex = clock_mutex;
        write_time_args->display = shield->display;
        write_time_args->event_group = other_event;
        write_time_args->second_event = SECOND_EVENT;
        write_time_args->write_flag = WRITE_FLAG;
        result = xTaskCreate(WriteTime, "WriteTime", WRITE_TIME_TASK_STACK_SIZE, write_time_args, tskIDLE_PRIORITY + 3,
                             NULL);
    }

    vTaskStartScheduler();

    while (1) {
    }
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
