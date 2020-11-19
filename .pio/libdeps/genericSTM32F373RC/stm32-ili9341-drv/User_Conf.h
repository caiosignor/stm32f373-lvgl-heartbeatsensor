//	MIT License
//
//	Copyright (c) 2020 Carlos Signor
//
//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files (the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions:
//
//	The above copyright notice and this permission notice shall be included in all
//	copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//	SOFTWARE.

#ifndef USER_CONF_H
#define USER_CONF_H

#if defined(STM32F1)
#include "stm32f1xx_hal.h"
#elif defined(STM32F3)
#include "stm32f3xx_hal.h"
#else
#error "You need define wich MCU family you are using!"
#endif

/*********************
 *      INCLUDES
 *********************/

#include "main.h"
#include "spi.h"
#include "gpio.h"

/** Spi Instance to Interface With Display and Touch **/
#define HSPI_INSTANCE &hspi3

#if !defined(HSPI_INSTANCE)
#error "You need define HSPI_INSTANCE with a pointer to SPI_HandleTypedef"
#endif

/***********************
 *  Display Interface
 ***********************/

/** Chip Select Port **/
#define LCD_CS_PORT LCD_CS_GPIO_Port
/** Chip Select Pin **/
#define LCD_CS_PIN LCD_CS_Pin
/** Lcd Data Comand Port **/
#define LCD_DC_PORT LCD_DC_GPIO_Port
/** Lcd Data Comand Pin **/
#define LCD_DC_PIN LCD_DC_Pin
/** Lcd Data Comand Pin **/
#define DC_Pin LCD_DC_Pin
/** Lcd Reset Port **/
#define LCD_RST_PORT RST_GPIO_Port
/** Lcd Reset Pin **/
#define LCD_RST_PIN RST_Pin

/***********************
 *   Touch Interface
 ***********************/

#define USE_TOUCH_XPT2046 1
#if USE_TOUCH_XPT2046

#include "XPT_Commands.h"

/** Touch Clock SPI Port **/
#define TP_CLK_PORT GPIOA
/** Touch Clock SPI Pin **/
#define TP_CLK_PIN GPIO_PIN_1
/** Touch Chip Select Port **/
#define TP_CS_PORT TOUCH_CS_GPIO_Port
/** Touch Chip Select Pin **/
#define TP_CS_PIN TOUCH_CS_Pin
/** Touch Miso SPI Port **/
#define TP_MISO_PORT GPIOA
/** Touch Miso SPI Pin **/
#define TP_MISO_PIN GPIO_PIN_2
/** Touch Mosi SPI Port **/
#define TP_MOSI_PORT GPIOA
/** Touch Mosi SPI Pin **/
#define TP_MOSI_PIN GPIO_PIN_3

/** Enable Interrupt Pin of Touch **/
#define USE_TOUCH_INTERRUPT 1
#if USE_TOUCH_INTERRUPT
#define PEN_IRQ_Port TOUCH_PEN_GPIO_Port
#define PEN_IRQ_Pin TOUCH_PEN_Pin
#define IRQ_Pin_State() HAL_GPIO_ReadPin(PEN_IRQ_Port, PEN_IRQ_Pin)
#endif

#endif
#endif