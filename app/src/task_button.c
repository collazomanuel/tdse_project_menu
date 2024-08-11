/*
 * Copyright (c) 2023 Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * @file   : task_button.c
 * @date   : Set 26, 2023
 * @author : Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>
 * @version	v1.0.0
 */

/********************** inclusions *******************************************/
/* Project includes. */
#include "main.h"

/* Demo includes. */
#include "logger.h"
#include "dwt.h"

/* Application & Tasks includes. */
#include "board.h"
#include "app.h"
#include "task_button_attribute.h"
#include "task_system_attribute.h"
#include "task_system_interface.h"

/********************** macros and definitions *******************************/
#define G_task_button_CNT_INIT	0u

#define DELAY_INI	0u
#define DELAY_MAX	50u

#define BTN_XX_DEL_MAX		DELAY_MAX
#define BTN_XX_DEL_MIN		DELAY_INI

/********************** internal data declaration ****************************/
const task_button_cfg_t task_button_cfg_list[] =

	{{ID_BTN_S1, BTN_S1_PORT, BTN_S1_PIN, BTN_S1_PRESSED,
	  BTN_XX_DEL_MAX, SIG_BTN_S1_UP, SIG_BTN_S1_DOWN},

	 {ID_BTN_S2, BTN_S2_PORT, BTN_S2_PIN, BTN_S2_PRESSED,
	  BTN_XX_DEL_MAX, SIG_BTN_S2_UP, SIG_BTN_S2_DOWN},

	 {ID_BTN_S3, BTN_S3_PORT, BTN_S3_PIN, BTN_S3_PRESSED,
	  BTN_XX_DEL_MAX, SIG_BTN_S3_UP, SIG_BTN_S3_DOWN},

	 {ID_BTN_S4, BTN_S4_PORT, BTN_S4_PIN, BTN_S4_PRESSED,
	  BTN_XX_DEL_MAX, SIG_BTN_S4_UP, SIG_BTN_S4_DOWN}};

#define BUTTON_CFG_QTY	(sizeof(task_button_cfg_list)/sizeof(task_button_cfg_t))

task_button_dta_t task_button_dta_list[] =

	{{BTN_XX_DEL_MIN, ST_BTN_XX_UP, EV_BTN_XX_UP},

	 {BTN_XX_DEL_MIN, ST_BTN_XX_UP, EV_BTN_XX_UP},

	 {BTN_XX_DEL_MIN, ST_BTN_XX_UP, EV_BTN_XX_UP},

     {BTN_XX_DEL_MIN, ST_BTN_XX_UP, EV_BTN_XX_UP}};

#define BUTTON_DTA_QTY	(sizeof(task_button_dta_list)/sizeof(task_button_dta_t))

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/
const char *p_task_button 		= "Task Sensor (Sensor Modeling)";
const char *p_task_button_ 		= "Non-Blocking & Update By Time Code";

/********************** external data declaration ****************************/
uint32_t g_task_button_cnt;
volatile uint32_t g_task_button_tick;

/********************** external functions definition ************************/
void task_button_init(void *parameters)
{
	uint32_t index;
	task_button_dta_t *p_task_button_dta;
	task_button_st_t state;
	task_button_ev_t event;

	/* Print out: Task Initialized */
	LOGGER_LOG("  %s is running - %s\r\n", GET_NAME(task_button_init), p_task_button);
	LOGGER_LOG("  %s is a %s\r\n", GET_NAME(task_button), p_task_button_);

	g_task_button_cnt = G_task_button_CNT_INIT;

	/* Print out: Task execution counter */
	LOGGER_LOG("   %s = %d\r\n", GET_NAME(g_task_button_cnt), (int)g_task_button_cnt);

	for (index = 0; BUTTON_DTA_QTY > index; index++)
	{
		/* Update Task Sensor Data Pointer */
		p_task_button_dta = &task_button_dta_list[index];

		/* Print out: Index & Task execution FSM */
		LOGGER_LOG("   %s = %d", GET_NAME(index), (int)index);

		state = p_task_button_dta->state;
		LOGGER_LOG("   %s = %d", GET_NAME(state), (int)state);

		event = p_task_button_dta->event;
		LOGGER_LOG("   %s = %d\r\n", GET_NAME(event), (int)event);
	}
	g_task_button_tick = DELAY_INI;
}

void task_button_update(void *parameters)
{
	uint32_t index;
	const task_button_cfg_t *p_task_button_cfg;
	task_button_dta_t *p_task_button_dta;
	bool b_time_update_required = false;

	/* Update Task Sensor Counter */
	g_task_button_cnt++;

	/* Protect shared resource (g_task_button_tick) */
	__asm("CPSID i");	/* disable interrupts*/
    if (DELAY_INI < g_task_button_tick)
    {
    	g_task_button_tick--;
    	b_time_update_required = true;
    }
    __asm("CPSIE i");	/* enable interrupts*/

    while (b_time_update_required)
    {
		/* Protect shared resource (g_task_button_tick) */
		__asm("CPSID i");	/* disable interrupts*/
		if (DELAY_INI < g_task_button_tick)
		{
			g_task_button_tick--;
			b_time_update_required = true;
		}
		else
		{
			b_time_update_required = false;
		}
		__asm("CPSIE i");	/* enable interrupts*/

    	for (index = 0; BUTTON_DTA_QTY > index; index++)
		{
    		/* Update Task Sensor Configuration & Data Pointer */
			p_task_button_cfg = &task_button_cfg_list[index];
			p_task_button_dta = &task_button_dta_list[index];

			if (p_task_button_cfg->pressed == HAL_GPIO_ReadPin(p_task_button_cfg->gpio_port, p_task_button_cfg->pin))
			{
				p_task_button_dta->event =	EV_BTN_XX_DOWN;
			}
			else
			{
				p_task_button_dta->event =	EV_BTN_XX_UP;
			}

			switch (p_task_button_dta->state)
			{
				case ST_BTN_XX_UP:

					if (EV_BTN_XX_DOWN == p_task_button_dta->event)
					{
						p_task_button_dta->tick = p_task_button_cfg->tick_max;
						p_task_button_dta->state = ST_BTN_XX_FALLING;
					}

					break;

				case ST_BTN_XX_FALLING:

					p_task_button_dta->tick--;
					if (BTN_XX_DEL_MIN == p_task_button_dta->tick)
					{
						if (EV_BTN_XX_DOWN == p_task_button_dta->event)
						{
							put_event_task_system(p_task_button_cfg->signal_down);
							p_task_button_dta->state = ST_BTN_XX_DOWN;
						}
						else
						{
							p_task_button_dta->state = ST_BTN_XX_UP;
						}
					}

					break;

				case ST_BTN_XX_DOWN:

					if (EV_BTN_XX_UP == p_task_button_dta->event)
					{
						p_task_button_dta->tick = p_task_button_cfg->tick_max;
						p_task_button_dta->state = ST_BTN_XX_RISING;
					}

					break;

				case ST_BTN_XX_RISING:

					p_task_button_dta->tick--;
					if (BTN_XX_DEL_MIN == p_task_button_dta->tick)
					{
						if (EV_BTN_XX_UP == p_task_button_dta->event)
						{
							//put_event_task_system(p_task_button_cfg->signal_up);
							p_task_button_dta->state = ST_BTN_XX_UP;
						}
						else
						{
							p_task_button_dta->state = ST_BTN_XX_DOWN;
						}
					}

					break;

				default:

					break;
			}
		}
    }
}

/********************** end of file ******************************************/
