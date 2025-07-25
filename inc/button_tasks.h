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

#ifndef BUTTON_TASKS_H_
#define BUTTON_TASKS_H_

/** @file button_tasks.h
 ** @brief Declaraciones de la biblioteca para la gestión de los botones - Electrónica 4 2025
 **/

/* === Headers files inclusions ==================================================================================== */

#include "FreeRTOS.h"
#include "event_groups.h"
#include "digital_input.h"

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

#define BUTTON_TASK_STACK_SIZE     (configMINIMAL_STACK_SIZE)
#define DIDNT_PUSH_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE)

/* === Public data type declarations =============================================================================== */

//! Estructura que contine los argumentos de la tarea @ref ButtonTask()
typedef struct button_task_arg_s {
    digital_input_p button;         //!< pulsador que se escanea
    EventGroupHandle_t event_group; //!< grupo de eventos al que avisa
    int push_event;                 //!< bit de evento que indica que se presionó el botón
    int hold_event;                 //!< bit de evento que indica que se mantuvo presionado el botón
    int hold_time;                  //!< timpo que hay que mantener apretado el boton en ms
    int time_counter;               //!< contador propio de cada tarea
} * button_task_arg_p;

//! Estructura que contine los argumentos de la tarea @ref DidntPushTask()
typedef struct didnt_prush_task_arg_s {
    EventGroupHandle_t event_group; //!< grupo de eventos al que avisa
    int buttons; //!< conjunto de todos los bits de evento correspondiente a los pulsadores que se desea que avisen
                 //!< cuando se pulsaron
    int time_ms; //!< tiempo que se desea que se espere para avisar que no se presionó nada
    int counter; //!< contador usado para llevar el tiempo, propio de la tarea
} * didnt_prush_task_arg_p;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

/**
 * @brief Tarea encargada de avisar que se presionó un botón, o que se lo mantuvo presionado
 *
 * @param arguments puntero a @ref button_task_arg_s
 */
void ButtonTask(void * arguments);

/**
 * @brief Tarea encargada de avisar que ningun boton se presionó
 *
 * @param arguments puntero a @ref didnt_prush_task_arg_s
 */
void DidntPushTask(void * arguments);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* BUTTON_TASKS_H_ */
