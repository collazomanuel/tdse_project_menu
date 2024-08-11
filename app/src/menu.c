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
 * @file   : menu.c
 * @date   : Ago 14, 2024
 * @author : Manuel Collazo <mcollazo@fi.uba.ar>
 * @version	v1.0.0
 */

/********************** inclusions *******************************************/
#include <stdio.h>
#include <string.h>

/* Project includes. */
#include "main.h"

/* Demo includes. */
#include "logger.h"
#include "dwt.h"

/* Application & Tasks includes. */
#include "menu.h"

/********************** macros and definitions *******************************/
#define DEFAULT_MENU_ITEM_COUNT 4
#define SUB_MENU_ITEM_COUNT 3

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

/********************** external data declaration ****************************/

/********************** internal functions definition ************************/

/********************** external functions definition ************************/
void menu_initialize(menu_t* menu, menu_t* parent_menu, menu_item_t* items, int item_count) {
    menu->items = items;
    menu->item_count = item_count;
    menu->current_item = 0;
    menu->parent_menu = parent_menu;
}

void menu_move_up(menu_t* menu) {
    if (menu->current_item > 0) {
        menu->current_item--;
    }
}

void menu_move_down(menu_t* menu) {
    if (menu->current_item < menu->item_count - 1) {
        menu->current_item++;
    }
}

void menu_go_to_parent(menu_t** current_menu) {
    if ((*current_menu)->parent_menu != NULL) {
        *current_menu = (*current_menu)->parent_menu;
    }
}

void menu_select(menu_t** current_menu) {
    if ((*current_menu)->items[(*current_menu)->current_item].sub_menu != NULL) {
        *current_menu = (*current_menu)->items[(*current_menu)->current_item].sub_menu;
    }
}

int menu_get_items_count(menu_t* menu) {
    return menu->item_count;
}

char* menu_get_item_label(menu_t* menu, int index) {
    if (index < 0 || index >= menu->item_count) {
        return NULL;
    }
    return menu->items[index].label;
}

void menu_initialize_default(menu_t* menu) {
    static menu_item_t default_items[DEFAULT_MENU_ITEM_COUNT];
    static menu_item_t sub_menu_items[DEFAULT_MENU_ITEM_COUNT][SUB_MENU_ITEM_COUNT];
    static menu_t sub_menus[DEFAULT_MENU_ITEM_COUNT];
    for (int i = 0; i < DEFAULT_MENU_ITEM_COUNT; i++) {
        for (int j = 0; j < SUB_MENU_ITEM_COUNT; j++) {
            sprintf(sub_menu_items[i][j].label, "Submenu %d Item %d", i + 1, j + 1);
            sub_menu_items[i][j].sub_menu = NULL;
        }
        menu_initialize(&sub_menus[i], menu, sub_menu_items[i], SUB_MENU_ITEM_COUNT);
    }
    for (int i = 0; i < DEFAULT_MENU_ITEM_COUNT; i++) {
        sprintf(default_items[i].label, "Menu Item %d", i + 1);
        default_items[i].sub_menu = &sub_menus[i];
    }
    menu_initialize(menu, NULL, default_items, DEFAULT_MENU_ITEM_COUNT);
}

int menu_get_current_item_index(menu_t* menu) {
	return menu->current_item;
}

/********************** end of file ******************************************/
