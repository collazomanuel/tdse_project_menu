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
 * @file   : display.h
 * @date   : Ago 14, 2024
 * @author : Manuel Collazo <mcollazo@fi.uba.ar>
 * @version	v1.0.0
 */

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/
#include <stdint.h>

/********************** macros ***********************************************/
// Functional states
#ifndef OFF
#define OFF    0
#endif
#ifndef ON
#define ON     (!OFF)
#endif

// Electrical states
#ifndef LOW
#define LOW    0
#endif
#ifndef HIGH
#define HIGH   (!LOW)
#endif

/********************** typedef **********************************************/
typedef enum {
     DISPLAY_CONNECTION_GPIO_4_BITS,
     DISPLAY_CONNECTION_GPIO_8_BITS,
     DISPLAY_CONNECTION_I2C_PCF8574_IO_EXPANDER
} display_connection_t;

typedef struct {
   display_connection_t connection;
} display_t;

typedef struct {
   int address;
   uint8_t data;
   bool display_pin_rs;
   bool display_pin_rw;
   bool display_pin_en;
   bool display_pin_a;
   bool display_pin_d4;
   bool display_pin_d5;
   bool display_pin_d6;
   bool display_pin_d7;
} pcf8574_t;

/********************** external data declaration ****************************/

/********************** external functions declaration ***********************/
void display_init(display_connection_t connection);
void display_char_position_write(uint8_t char_position_x, uint8_t char_position_y);
void display_string_write(const char * str);
void keep_alive();

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* _DISPLAY_H_ */

