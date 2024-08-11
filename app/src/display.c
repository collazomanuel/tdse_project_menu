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
 * @file   : display.c
 * @date   : Ago 14, 2024
 * @author : Manuel Collazo <mcollazo@fi.uba.ar>
 * @version	v1.0.0
 */

/********************** inclusions *******************************************/
#include <stdbool.h>

/* Project includes. */
#include "main.h"

/* Demo includes. */

/* Application & Tasks includes. */
#include "display.h"

/********************** macros and definitions *******************************/
#define DISPLAY_IR_CLEAR_DISPLAY   0b00000001
#define DISPLAY_IR_ENTRY_MODE_SET  0b00000100
#define DISPLAY_IR_DISPLAY_CONTROL 0b00001000
#define DISPLAY_IR_FUNCTION_SET    0b00100000
#define DISPLAY_IR_SET_DDRAM_ADDR  0b10000000

#define DISPLAY_IR_ENTRY_MODE_SET_INCREMENT 0b00000010
#define DISPLAY_IR_ENTRY_MODE_SET_DECREMENT 0b00000000
#define DISPLAY_IR_ENTRY_MODE_SET_SHIFT     0b00000001
#define DISPLAY_IR_ENTRY_MODE_SET_NO_SHIFT  0b00000000

#define DISPLAY_IR_DISPLAY_CONTROL_DISPLAY_ON  0b00000100
#define DISPLAY_IR_DISPLAY_CONTROL_DISPLAY_OFF 0b00000000
#define DISPLAY_IR_DISPLAY_CONTROL_CURSOR_ON   0b00000010
#define DISPLAY_IR_DISPLAY_CONTROL_CURSOR_OFF  0b00000000
#define DISPLAY_IR_DISPLAY_CONTROL_BLINK_ON    0b00000001
#define DISPLAY_IR_DISPLAY_CONTROL_BLINK_OFF   0b00000000

#define DISPLAY_IR_FUNCTION_SET_8BITS    0b00010000
#define DISPLAY_IR_FUNCTION_SET_4BITS    0b00000000
#define DISPLAY_IR_FUNCTION_SET_2LINES   0b00001000
#define DISPLAY_IR_FUNCTION_SET_1LINE    0b00000000
#define DISPLAY_IR_FUNCTION_SET_5x10DOTS 0b00000100
#define DISPLAY_IR_FUNCTION_SET_5x8DOTS  0b00000000

#define DISPLAY_20x4_LINE1_FIRST_CHARACTER_ADDRESS 0
#define DISPLAY_20x4_LINE2_FIRST_CHARACTER_ADDRESS 64
#define DISPLAY_20x4_LINE3_FIRST_CHARACTER_ADDRESS 20
#define DISPLAY_20x4_LINE4_FIRST_CHARACTER_ADDRESS 84

#define DISPLAY_RS_INSTRUCTION 0
#define DISPLAY_RS_DATA        1

#define DISPLAY_RW_WRITE 0
#define DISPLAY_RW_READ  1

#define DISPLAY_PIN_RS  4
#define DISPLAY_PIN_RW  5
#define DISPLAY_PIN_EN  6
#define DISPLAY_PIN_D0  7
#define DISPLAY_PIN_D1  8
#define DISPLAY_PIN_D2  9
#define DISPLAY_PIN_D3 10
#define DISPLAY_PIN_D4 11
#define DISPLAY_PIN_D5 12
#define DISPLAY_PIN_D6 13
#define DISPLAY_PIN_D7 14

#define DISPLAY_PIN_A_PCF8574 3

#define I2C1_SDA PB_9
#define I2C1_SCL PB_8

#define PCF8574_I2C_BUS_8BIT_WRITE_ADDRESS 78

/********************** internal data declaration ****************************/
static display_t display;
static pcf8574_t pcf8574;
static bool initial_8_bit_communication_is_completed;

/********************** internal functions declaration ***********************/
static void display_pin_write(uint8_t pin_name, int value);
static void display_data_bus_write(uint8_t data_byte);
static void display_code_write(bool type, uint8_t data_bus);

/********************** internal data definition *****************************/

/********************** external data declaration ****************************/
extern I2C_HandleTypeDef hi2c1;

/********************** internal functions definition ************************/
static void display_code_write(bool type, uint8_t data_bus)
{
    if (type == DISPLAY_RS_INSTRUCTION)
        display_pin_write(DISPLAY_PIN_RS, DISPLAY_RS_INSTRUCTION);
    else
        display_pin_write(DISPLAY_PIN_RS, DISPLAY_RS_DATA);
    display_pin_write(DISPLAY_PIN_RW, DISPLAY_RW_WRITE);
    display_data_bus_write(data_bus);
}

static void display_pin_write(uint8_t pin_name, int value)
{
	switch (display.connection) {
		case DISPLAY_CONNECTION_GPIO_8_BITS:
			switch (pin_name) {
				case DISPLAY_PIN_D0: HAL_GPIO_WritePin(D2_GPIO_Port,  D2_Pin,  value);   break;
				case DISPLAY_PIN_D1: HAL_GPIO_WritePin(D4_GPIO_Port,  D4_Pin,  value);   break;
				case DISPLAY_PIN_D2: HAL_GPIO_WritePin(D5_GPIO_Port,  D5_Pin,  value);   break;
				case DISPLAY_PIN_D3: HAL_GPIO_WritePin(D6_GPIO_Port,  D6_Pin,  value);   break;
				case DISPLAY_PIN_D4: HAL_GPIO_WritePin(D7_GPIO_Port,  D7_Pin,  value);   break;
				case DISPLAY_PIN_D5: HAL_GPIO_WritePin(D8_GPIO_Port,  D8_Pin,  value);   break;
				case DISPLAY_PIN_D6: HAL_GPIO_WritePin(D9_GPIO_Port,  D9_Pin,  value);   break;
				case DISPLAY_PIN_D7: HAL_GPIO_WritePin(D10_GPIO_Port, D10_Pin, value);   break;
				case DISPLAY_PIN_RS: HAL_GPIO_WritePin(D11_GPIO_Port, D11_Pin, value);   break;
				case DISPLAY_PIN_EN: HAL_GPIO_WritePin(D12_GPIO_Port, D12_Pin, value);   break;
				case DISPLAY_PIN_RW: break;
				default: break;
			}
			break;
		case DISPLAY_CONNECTION_GPIO_4_BITS:
			switch (pin_name) {
				case DISPLAY_PIN_D4: HAL_GPIO_WritePin(D7_GPIO_Port,  D7_Pin,  value);   break;
				case DISPLAY_PIN_D5: HAL_GPIO_WritePin(D8_GPIO_Port,  D8_Pin,  value);   break;
				case DISPLAY_PIN_D6: HAL_GPIO_WritePin(D9_GPIO_Port,  D9_Pin,  value);   break;
				case DISPLAY_PIN_D7: HAL_GPIO_WritePin(D10_GPIO_Port, D10_Pin, value);   break;
				case DISPLAY_PIN_RS: HAL_GPIO_WritePin(D11_GPIO_Port, D11_Pin, value);   break;
				case DISPLAY_PIN_EN: HAL_GPIO_WritePin(D12_GPIO_Port, D12_Pin, value);   break;
				case DISPLAY_PIN_RW: break;
				default: break;
			}
			break;
		case DISPLAY_CONNECTION_I2C_PCF8574_IO_EXPANDER:
			if (value) {
				switch(pin_name) {
					case DISPLAY_PIN_D4: pcf8574.display_pin_d4 = ON; break;
					case DISPLAY_PIN_D5: pcf8574.display_pin_d5 = ON; break;
					case DISPLAY_PIN_D6: pcf8574.display_pin_d6 = ON; break;
					case DISPLAY_PIN_D7: pcf8574.display_pin_d7 = ON; break;
					case DISPLAY_PIN_RS: pcf8574.display_pin_rs = ON; break;
					case DISPLAY_PIN_EN: pcf8574.display_pin_en = ON; break;
					case DISPLAY_PIN_RW: pcf8574.display_pin_rw = ON; break;
					case DISPLAY_PIN_A_PCF8574: pcf8574.display_pin_a = ON; break;
					default: break;
	            }
			}
	        else {
	        	switch(pin_name) {
					case DISPLAY_PIN_D4: pcf8574.display_pin_d4 = OFF; break;
					case DISPLAY_PIN_D5: pcf8574.display_pin_d5 = OFF; break;
					case DISPLAY_PIN_D6: pcf8574.display_pin_d6 = OFF; break;
					case DISPLAY_PIN_D7: pcf8574.display_pin_d7 = OFF; break;
					case DISPLAY_PIN_RS: pcf8574.display_pin_rs = OFF; break;
					case DISPLAY_PIN_EN: pcf8574.display_pin_en = OFF; break;
					case DISPLAY_PIN_RW: pcf8574.display_pin_rw = OFF; break;
					case DISPLAY_PIN_A_PCF8574: pcf8574.display_pin_a = OFF; break;
					default: break;
	            }
	        }
			pcf8574.data = 0b00000000;
			if (pcf8574.display_pin_rs) pcf8574.data |= 0b00000001;
			if (pcf8574.display_pin_rw) pcf8574.data |= 0b00000010;
			if (pcf8574.display_pin_en) pcf8574.data |= 0b00000100;
			if (pcf8574.display_pin_a ) pcf8574.data |= 0b00001000;
			if (pcf8574.display_pin_d4) pcf8574.data |= 0b00010000;
			if (pcf8574.display_pin_d5) pcf8574.data |= 0b00100000;
			if (pcf8574.display_pin_d6) pcf8574.data |= 0b01000000;
			if (pcf8574.display_pin_d7) pcf8574.data |= 0b10000000;
			//i2cPcf8574.write(pcf8574.address, &pcf8574.data, 1);
            HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)pcf8574.address, &pcf8574.data, 16, HAL_MAX_DELAY);
			break;
	}
}

static void display_data_bus_write(uint8_t data_bus)
{
    display_pin_write(DISPLAY_PIN_EN, OFF);
    display_pin_write(DISPLAY_PIN_D7, data_bus & 0b10000000);
    display_pin_write(DISPLAY_PIN_D6, data_bus & 0b01000000);
    display_pin_write(DISPLAY_PIN_D5, data_bus & 0b00100000);
    display_pin_write(DISPLAY_PIN_D4, data_bus & 0b00010000);

	switch (display.connection) {
		case DISPLAY_CONNECTION_GPIO_8_BITS:
			display_pin_write(DISPLAY_PIN_D3, data_bus & 0b00001000);
			display_pin_write(DISPLAY_PIN_D2, data_bus & 0b00000100);
			display_pin_write(DISPLAY_PIN_D1, data_bus & 0b00000010);
			display_pin_write(DISPLAY_PIN_D0, data_bus & 0b00000001);
			break;
		case DISPLAY_CONNECTION_GPIO_4_BITS:
        case DISPLAY_CONNECTION_I2C_PCF8574_IO_EXPANDER:
            if (initial_8_bit_communication_is_completed == true) {
            	display_pin_write(DISPLAY_PIN_EN, ON);
            	HAL_Delay(1);
                display_pin_write(DISPLAY_PIN_EN, OFF);
                HAL_Delay(1);
                display_pin_write(DISPLAY_PIN_D7, data_bus & 0b00001000);
                display_pin_write(DISPLAY_PIN_D6, data_bus & 0b00000100);
                display_pin_write(DISPLAY_PIN_D5, data_bus & 0b00000010);
                display_pin_write(DISPLAY_PIN_D4, data_bus & 0b00000001);
            }
			break;
	}

    display_pin_write(DISPLAY_PIN_EN, ON);
    HAL_Delay(1);
    display_pin_write(DISPLAY_PIN_EN, OFF);
    HAL_Delay(1);
}

/********************** external functions definition ************************/
void display_init(display_connection_t connection)
{
    display.connection = connection;

    if (display.connection == DISPLAY_CONNECTION_I2C_PCF8574_IO_EXPANDER) {
    	pcf8574.address = PCF8574_I2C_BUS_8BIT_WRITE_ADDRESS;
        pcf8574.data = 0b00000000;
        //i2cPcf8574.frequency(100000);
        display_pin_write(DISPLAY_PIN_A_PCF8574, ON);
    }

    initial_8_bit_communication_is_completed = false;
	HAL_Delay(50);

    display_code_write(DISPLAY_RS_INSTRUCTION, DISPLAY_IR_FUNCTION_SET | DISPLAY_IR_FUNCTION_SET_8BITS);
    HAL_Delay(5);
    display_code_write(DISPLAY_RS_INSTRUCTION, DISPLAY_IR_FUNCTION_SET | DISPLAY_IR_FUNCTION_SET_8BITS);
    HAL_Delay(1);
    display_code_write(DISPLAY_RS_INSTRUCTION, DISPLAY_IR_FUNCTION_SET | DISPLAY_IR_FUNCTION_SET_8BITS);
    HAL_Delay(1);

    switch (display.connection) {
    	case DISPLAY_CONNECTION_GPIO_8_BITS:
    		display_code_write(DISPLAY_RS_INSTRUCTION, DISPLAY_IR_FUNCTION_SET | DISPLAY_IR_FUNCTION_SET_8BITS | DISPLAY_IR_FUNCTION_SET_2LINES | DISPLAY_IR_FUNCTION_SET_5x8DOTS);
    		HAL_Delay(1);
    		break;
        case DISPLAY_CONNECTION_GPIO_4_BITS:
        case DISPLAY_CONNECTION_I2C_PCF8574_IO_EXPANDER:
        	display_code_write(DISPLAY_RS_INSTRUCTION, DISPLAY_IR_FUNCTION_SET | DISPLAY_IR_FUNCTION_SET_4BITS);
        	HAL_Delay(1);
        	initial_8_bit_communication_is_completed = true;
        	display_code_write(DISPLAY_RS_INSTRUCTION, DISPLAY_IR_FUNCTION_SET | DISPLAY_IR_FUNCTION_SET_4BITS | DISPLAY_IR_FUNCTION_SET_2LINES | DISPLAY_IR_FUNCTION_SET_5x8DOTS);
            HAL_Delay(1);
            break;
    }

    display_code_write(DISPLAY_RS_INSTRUCTION, DISPLAY_IR_DISPLAY_CONTROL | DISPLAY_IR_DISPLAY_CONTROL_DISPLAY_OFF | DISPLAY_IR_DISPLAY_CONTROL_CURSOR_OFF | DISPLAY_IR_DISPLAY_CONTROL_BLINK_OFF);
    HAL_Delay(1);
    display_code_write(DISPLAY_RS_INSTRUCTION, DISPLAY_IR_CLEAR_DISPLAY);
    HAL_Delay(1);
    display_code_write(DISPLAY_RS_INSTRUCTION, DISPLAY_IR_ENTRY_MODE_SET | DISPLAY_IR_ENTRY_MODE_SET_INCREMENT | DISPLAY_IR_ENTRY_MODE_SET_NO_SHIFT);
    HAL_Delay(1);
    display_code_write( DISPLAY_RS_INSTRUCTION, DISPLAY_IR_DISPLAY_CONTROL | DISPLAY_IR_DISPLAY_CONTROL_DISPLAY_ON | DISPLAY_IR_DISPLAY_CONTROL_CURSOR_OFF | DISPLAY_IR_DISPLAY_CONTROL_BLINK_OFF);
    HAL_Delay(1);
}

void display_char_position_write(uint8_t char_position_x, uint8_t char_position_y)
{    
    switch (char_position_y) {
        case 0: display_code_write(DISPLAY_RS_INSTRUCTION, DISPLAY_IR_SET_DDRAM_ADDR | (DISPLAY_20x4_LINE1_FIRST_CHARACTER_ADDRESS + char_position_x));   break;
        case 1: display_code_write(DISPLAY_RS_INSTRUCTION, DISPLAY_IR_SET_DDRAM_ADDR | (DISPLAY_20x4_LINE2_FIRST_CHARACTER_ADDRESS + char_position_x));   break;
        case 2: display_code_write(DISPLAY_RS_INSTRUCTION, DISPLAY_IR_SET_DDRAM_ADDR | (DISPLAY_20x4_LINE3_FIRST_CHARACTER_ADDRESS + char_position_x));   break;
        case 3: display_code_write(DISPLAY_RS_INSTRUCTION, DISPLAY_IR_SET_DDRAM_ADDR | (DISPLAY_20x4_LINE4_FIRST_CHARACTER_ADDRESS + char_position_x));   break;
    }
    HAL_Delay(1);
}

void display_string_write(const char * str)
{
    while (*str) {
        display_code_write(DISPLAY_RS_DATA, *str++);
    }
}

void keep_alive()
{
	display_pin_write(DISPLAY_PIN_A_PCF8574, ON);
}

/********************** end of file ******************************************/
