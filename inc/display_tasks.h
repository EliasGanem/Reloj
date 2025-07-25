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

#ifndef DISPLAY_TASKS_H_
#define DISPLAY_TASKS_H_

/** @file display_tasks.h
 ** @brief Declaraciones de la biblioteca para la tarea del refresco de pantalla - Electrónica 4 2025
 **/

/* === Headers files inclusions ==================================================================================== */

#include "semphr.h"
#include "event_groups.h"

#include "display.h"
#include "clock.h"

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

#define DISPLAY_REFRESH_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE)
#define WRITE_TIME_TASK_STACK_SIZE      (configMINIMAL_STACK_SIZE)

/* === Public data type declarations =============================================================================== */

//! Estructura que contine los argumentos de la tarea @ref DisplayRefreshTask()
typedef struct display_refresh_task_arg_s {
    SemaphoreHandle_t display_mutex; //!< mutex para el display
    display_p display;               //!< referencia al objeto display
} * display_refresh_task_arg_p;

//! Estructura que contine los argumentos de la tarea @ref WriteTimeTask()
typedef struct write_time_task_arg_s {
    SemaphoreHandle_t display_mutex; //!< mutex para el uso de la memoria de video del display
    SemaphoreHandle_t clock_mutex;   //!< mutex para el uso de cualquier varaible del reloj
    display_p display;               //!< referencia al objeto display
    EventGroupHandle_t event_group;  //!< grupo de eventos de los botones
    clock_p clock;                   //!< referencia al objeto reloj
    clock_time_u current_time;       //< variable para que cada tarea almacene una varible de hora
} * write_time_task_arg_p;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

/**
 * @brief Tarea que se encarga de refrescar el display
 *
 * @param arguments puntero a @ref display_refresh_task_arg_s
 */
void DisplayRefreshTask(void * arguments);

/**
 * @brief Tarea que actualiza la hora cada un cierto tiempo, siempre que este en los estados correspondientes
 * (invlaido y valido)
 *
 * @param arguments puntero a @ref write_time_task_arg_s
 */
void WriteTimeTask(void * arguments);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_TASKS_H_ */
