/*
 * Copyright (c) 2024 Manuel Collazo <mcollazo@fi.uba.ar>.
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
 * @file   : menu.h
 * @date   : Ago 14, 2024
 * @author : Manuel Collazo <mcollazo@fi.uba.ar>
 * @version	v1.0.0
 */

#ifndef _MENU_H_
#define _MENU_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/
#include <stdbool.h>

/********************** macros ***********************************************/
#define MAX_LABEL_LENGTH 20

/********************** typedef **********************************************/
typedef struct menu_item_t {
    char label[MAX_LABEL_LENGTH];
    struct menu_t* sub_menu;
} menu_item_t;

typedef struct menu_t  {
    menu_item_t *items;
    int item_count;
    int current_item;
    struct menu_t* parent_menu;
} menu_t;

/********************** external data declaration ****************************/

/********************** external functions declaration ***********************/
void menu_initialize(menu_t* menu, menu_t* parent_menu, menu_item_t* items, int item_count);
void menu_move_up(menu_t* menu);
void menu_move_down(menu_t* menu);
void menu_go_to_parent(menu_t** menu);
void menu_select(menu_t** current_menu);
int menu_get_items_count(menu_t* menu);
char* menu_get_item_label(menu_t* menu, int index);
void menu_initialize_default(menu_t* menu);
int menu_get_current_item_index(menu_t* menu);

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* _MENU_H_ */
