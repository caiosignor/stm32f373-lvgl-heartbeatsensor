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
//
//
//-----------------------------------
//	ILI9341 Touchscreen library for STM32
//-----------------------------------
//
//	Very simple Touchscreen library for ILI9341.
//	Extremly basic reading of position. No runtime calibration, No prediction, basic noise removal. Simple but stupid.
//	Basic hardcoded calibration values saved in .h file
//
//	Library is written for STM32 HAL library and supports STM32CUBEMX. To use the library with Cube software
//	you need to tick the box that generates peripheral initialization code in their own respective .c and .h file
//
//
//-----------------------------------

//HARDCODED CALIBRATION, CHANGE IF REQUIRED
#define X_OFFSET 13
#define Y_OFFSET 15
#define X_MAGNITUDE 1.16
#define Y_MAGNITUDE 1.16

//CONVERTING 16bit Value to Screen coordinates
// 65535/273 = 240!
// 65535/204 = 320!
#define X_TRANSLATION 273
#define Y_TRANSLATION 204

#define NO_OF_POSITION_SAMPLES 10

//Internal Touchpad command, do not call directly
// uint8_t TP_Read(void);

//Internal Touchpad command, do not call directly
// void TP_Write(uint8_t value);

//Read coordinates of touchscreen press. Position[0] = X, Position[1] = Y
uint8_t TP_Read_Coordinates(uint16_t Coordinates[2]);

//Check if Touchpad was pressed. Returns TOUCHPAD_PRESSED (1) or TOUCHPAD_NOT_PRESSED (0)
uint8_t TP_Touchpad_Pressed(void);

// #endif
