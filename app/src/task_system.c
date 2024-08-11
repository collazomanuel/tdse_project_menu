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
 * @file   : task_system.c
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
#include "task_screen_attribute.h"
#include "task_screen_interface.h"
#include "task_system_attribute.h"
#include "task_system_interface.h"
#include "menu.h"

/********************** macros and definitions *******************************/
#define G_TASK_SYSTEM_CNT_INI	0u

#define DELAY_INI	0u
#define DELAY_MED	500u
#define DELAY_MAX	5000u

#define SYS_XX_DEL_MIN	DELAY_INI
#define SYS_XX_DEL_MED	DELAY_MED
#define SYS_XX_DEL_MAX	DELAY_MAX

/********************** internal data declaration ****************************/
task_system_dta_t task_system_dta =

	{SYS_XX_DEL_MIN, ST_SYS_XX_IDLE, EV_SYS_XX_IDLE, false};

#define SYSTEM_DTA_QTY	(sizeof(task_system_dta)/sizeof(task_system_dta_t))

menu_t main_menu;
menu_t* current_menu = &main_menu;

/********************** internal functions declaration ***********************/
task_screen_dta_t build_screen_dta_from_menu(menu_t* menu, bool context_switch);

/********************** internal data definition *****************************/
const char *p_task_system 		= "Task System (System Modeling)";
const char *p_task_system_ 		= "Non-Blocking & Update By Time Code";

/********************** external data declaration ****************************/
uint32_t g_task_system_cnt;
volatile uint32_t g_task_system_tick;

/********************** internal functions definition ************************/
task_screen_dta_t build_screen_dta_from_menu(menu_t* menu, bool context_switch)
{
	int i;
	task_screen_dta_t task_screen;
	for (i = 0; i < LCD_DISPLAY_HEIGHT; i++)
	{
		task_screen.lines[i] = menu_get_item_label(current_menu, i);
	}
	task_screen.selected = menu_get_current_item_index(current_menu);
	task_screen.context_switch = context_switch;
	return task_screen;
}

/********************** external functions definition ************************/
void task_system_init(void *parameters)
{
	task_system_dta_t 	*p_task_system_dta;
	task_system_st_t	state;
	task_system_ev_t	event;
	bool b_event;

	/* Print out: Task Initialized */
	LOGGER_LOG("  %s is running - %s\r\n", GET_NAME(task_system_init), p_task_system);
	LOGGER_LOG("  %s is a %s\r\n", GET_NAME(task_system), p_task_system_);

	g_task_system_cnt = G_TASK_SYSTEM_CNT_INI;

	/* Print out: Task execution counter */
	LOGGER_LOG("   %s = %d\r\n", GET_NAME(g_task_system_cnt), (int)g_task_system_cnt);

	init_queue_event_task_system();

	/* Update Task System Data Pointer */
	p_task_system_dta = &task_system_dta;

	/* Print out: Task execution FSM */
	state = p_task_system_dta->state;
	LOGGER_LOG("   %s = %d", GET_NAME(state), (int)state);

	event = p_task_system_dta->event;
	LOGGER_LOG("   %s = %d", GET_NAME(event), (int)event);

	b_event = p_task_system_dta->flag;
	LOGGER_LOG("   %s = %d\r\n", GET_NAME(b_event), (int)b_event);

	g_task_system_tick = DELAY_INI;

	menu_initialize_default(current_menu);
	
	task_screen_dta_t init_task_screen = build_screen_dta_from_menu(current_menu, true);

	put_event_task_screen(init_task_screen);
}

void task_system_update(void *parameters)
{
	task_system_dta_t *p_task_system_dta;
	bool b_time_update_required = false;

	/* Update Task System Counter */
	g_task_system_cnt++;

	/* Protect shared resource (g_task_system_tick) */
	__asm("CPSID i");	/* disable interrupts*/
    if (DELAY_INI < g_task_system_tick)
    {
    	g_task_system_tick--;
    	b_time_update_required = true;
    }
    __asm("CPSIE i");	/* enable interrupts*/

    while (b_time_update_required)
    {
		/* Protect shared resource (g_task_system_tick) */
		__asm("CPSID i");	/* disable interrupts*/
		if (DELAY_INI < g_task_system_tick)
		{
			g_task_system_tick--;
			b_time_update_required = true;
		}
		else
		{
			b_time_update_required = false;
		}
		__asm("CPSIE i");	/* enable interrupts*/

    	/* Update Task System Data Pointer */
		p_task_system_dta = &task_system_dta;

		if (true == any_event_task_system())
		{
			p_task_system_dta->flag = true;
			p_task_system_dta->event = get_event_task_system();
		}

		if (true == p_task_system_dta->flag)
		{
			bool context_switch = false;
			switch ((task_button_sig_t) p_task_system_dta->event)
			{
				case SIG_BTN_S1_DOWN: menu_go_to_parent(&current_menu); context_switch = true;	break;
				case SIG_BTN_S2_DOWN: menu_move_up(current_menu);		context_switch = false;	break;
				case SIG_BTN_S3_DOWN: menu_move_down(current_menu);		context_switch = false;	break;
				case SIG_BTN_S4_DOWN: menu_select(&current_menu);	    context_switch = true;	break;
				default:																		break;
			}
			task_screen_dta_t new_task_screen = build_screen_dta_from_menu(current_menu, context_switch);
			put_event_task_screen(new_task_screen);
		}

		p_task_system_dta->flag = false;

		switch (p_task_system_dta->state)
		{
			case ST_SYS_XX_IDLE:

				if ((true == p_task_system_dta->flag) && (EV_SYS_XX_ACTIVE == p_task_system_dta->event))
				{
					p_task_system_dta->flag = false;
					//put_event_task_actuator(EV_LED_XX_BLINK, ID_LED_A);
					p_task_system_dta->state = ST_SYS_XX_ACTIVE;
				}

				break;

			case ST_SYS_XX_ACTIVE:

				if ((true == p_task_system_dta->flag) && (EV_SYS_XX_IDLE == p_task_system_dta->event))
				{
					p_task_system_dta->flag = false;
					//put_event_task_actuator(EV_LED_XX_NOT_BLINK, ID_LED_A);
					p_task_system_dta->state = ST_SYS_XX_IDLE;
				}

				break;

			default:

				break;
		}
	}
}

/********************** end of file ******************************************/
