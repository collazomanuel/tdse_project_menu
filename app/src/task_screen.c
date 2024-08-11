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
 * @file   : task_screen.c
 * @date   : Set 26, 2023
 * @author : Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>
 * @version	v1.0.0
 */

/********************** inclusions *******************************************/
#include <string.h>

/* Project includes. */
#include "main.h"

/* Demo includes. */
#include "logger.h"
#include "dwt.h"

/* Application & Tasks includes. */
#include "board.h"
#include "app.h"
#include "task_screen_attribute.h"
#include "task_screen_interface.h"
#include "display.h"

/********************** macros and definitions *******************************/
#define G_TASK_ACT_CNT_INIT	0u

#define DELAY_INI	0u

#define SYSTEM_TIME_INCREMENT_MS   5
#define DISPLAY_REFRESH_TIME_MS 50000
#define FIRST_COLUMN_NUMBER 0

/********************** internal data declaration ****************************/
task_screen_dta_t task_screen_dta = {{"Default 1", "Default 2", "Default 3", "Default 4"}, 0, false};

/********************** internal functions declaration ***********************/
static void format_display_line(const char* input, char* output, bool is_selected);
void change_display(const char* line_1, const char* line_2, const char* line_3, const char* line_4, int current_item_index);
void update_selected(int current_item_index);

/********************** internal data definition *****************************/
const char *p_task_screen 		= "Task Screen (Screen Modeling)";
const char *p_task_screen_ 		= "Non-Blocking & Update By Time Code";

/********************** external data declaration ****************************/
uint32_t g_task_screen_cnt;
volatile uint32_t g_task_screen_tick;

/********************** internal functions definition ************************/
static void format_display_line(const char* input, char* output, bool is_selected) {
	int input_len = strlen(input);
    if (input_len > 0) {
        strncpy(output, "[ ] ", LCD_DISPLAY_WIDTH);
        if (is_selected) {
            output[1] = 'x';
        }
        strncpy(output + 4, input, LCD_DISPLAY_WIDTH - 4);
        int output_len = strlen(output);
        if (output_len < LCD_DISPLAY_WIDTH) {
            for (int i = output_len; i < LCD_DISPLAY_WIDTH; i++) {
                output[i] = ' ';
            }
        }
    } else {
        for (int i = 0; i < LCD_DISPLAY_WIDTH; i++) {
            output[i] = ' ';
        }
    }
    output[LCD_DISPLAY_WIDTH] = '\0';
}

void change_display(const char* line_1, const char* line_2, const char* line_3, const char* line_4, int current_item_index) {
    char lines_to_display[LCD_DISPLAY_HEIGHT][LCD_DISPLAY_WIDTH + 1];
	format_display_line(line_1, lines_to_display[0], 0 == current_item_index);
	format_display_line(line_2, lines_to_display[1], 1 == current_item_index);
	format_display_line(line_3, lines_to_display[2], 2 == current_item_index);
	format_display_line(line_4, lines_to_display[3], 3 == current_item_index);

    for (size_t i = 0; i < LCD_DISPLAY_HEIGHT; i++)
    {
	    display_char_position_write (FIRST_COLUMN_NUMBER, i);
	    display_string_write(lines_to_display[i]);
    }
}

void update_selected(int current_item_index) {
	for (size_t i = 0; i < LCD_DISPLAY_HEIGHT; i++)
    {
	    display_char_position_write (FIRST_COLUMN_NUMBER + 1, i);
	    display_string_write(" ");
    }
	display_char_position_write (FIRST_COLUMN_NUMBER + 1, current_item_index);
	display_string_write("x");
}

/********************** external functions definition ************************/
void task_screen_init(void *parameters)
{
	/* Print out: Task Initialized */
	LOGGER_LOG("  %s is running - %s\r\n", GET_NAME(task_screen_init), p_task_screen);
	LOGGER_LOG("  %s is a %s\r\n", GET_NAME(task_screen), p_task_screen_);

	g_task_screen_cnt = G_TASK_ACT_CNT_INIT;

	/* Print out: Task execution counter */
	LOGGER_LOG("   %s = %d\r\n", GET_NAME(g_task_screen_cnt), (int)g_task_screen_cnt);

	init_queue_event_task_screen();
	display_init(DISPLAY_CONNECTION_I2C_PCF8574_IO_EXPANDER);

	g_task_screen_tick = DELAY_INI;
}

void task_screen_update(void *parameters)
{
	task_screen_dta_t *p_task_screen_dta;
	bool b_time_update_required = false;

	/* Update Task Screen Counter */
	g_task_screen_cnt++;

	/* Protect shared resource (g_task_screen_tick) */
	__asm("CPSID i");	/* disable interrupts*/
    if (DELAY_INI < g_task_screen_tick && true)
    {
    	g_task_screen_tick--;
    	b_time_update_required = true;
    }
    __asm("CPSIE i");	/* enable interrupts*/

    while (b_time_update_required)
    {
		/* Protect shared resource (g_task_screen_tick) */
		__asm("CPSID i");	/* disable interrupts*/
		if (DELAY_INI < g_task_screen_tick && false)
		{
			g_task_screen_tick--;
			b_time_update_required = true;
		}
		else
		{
			b_time_update_required = false;
		}
		__asm("CPSIE i");	/* enable interrupts*/

		/* Update Task Screen Data Pointer */
		p_task_screen_dta = &task_screen_dta;

		if (true == any_event_task_screen())
		{
			*p_task_screen_dta = get_event_task_screen();

			if (p_task_screen_dta->context_switch)
			{
				change_display(
					p_task_screen_dta->lines[0],
					p_task_screen_dta->lines[1],
					p_task_screen_dta->lines[2],
					p_task_screen_dta->lines[3],
					p_task_screen_dta->selected
				);
			}
			else
			{
				update_selected(p_task_screen_dta->selected);
			}
		}
		else
		{
			keep_alive();
		}
    }
}

/********************** end of file ******************************************/
