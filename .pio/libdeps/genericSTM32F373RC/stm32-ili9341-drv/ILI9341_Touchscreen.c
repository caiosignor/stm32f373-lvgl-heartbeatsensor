//	MIT License
//
//	Copyright (c) 2017 Matej Artnak
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
//	How to use this library
//-----------------------------------
//
//	-If using MCUs other than STM32F7 you will have to change the #include "stm32f7xx_hal.h" in the ILI9341_Touchscreen.h to your respective .h file
//	-define GPIO inputs and outputs then map the Pins and Ports inside the ILI9341_Touchscreen.h
//	-Library does not require any initialization calls apart from GPIO initialization. Initialize GPIOs before calling library functions!
//
//		Touchpad GPIO defines
//			Outputs:
//				CLK
//				MOSI
//				CS
//
//			Inputs:
//				IRQ
//				MISO
//
//
//	-Touchpad library bitbangs SPI interface and only requires basic GPIOs.
//	-Setting GPIOs as FREQ_VERY_HIGH Recommended
//
//	-Warning! Library is written for "ILI9341_Set_Rotation(SCREEN_VERTICAL_1)"	(See ILI9341_STM32_Driver for information about function)
//	-If using different layout you will have to Re-Map X and Y coordinates of the Screen
//
//
//	-NO_OF_POSITION_SAMPLES makes location output less noisy but increases reading time. Increase and decrease to your liking
//
//----------EXAMPLE------------------
/*
if(TP_Touchpad_Pressed())
	{					
		uint16_t x_pos = 0;
		uint16_t y_pos = 0;
		
		uint16_t position_array[2];
		if(TP_Read_Coordinates(position_array) == TOUCHPAD_DATA_OK)
		{
		x_pos = position_array[0];
		y_pos = position_array[1];
		}							
	}
*/
//-----------------------------------

#include "User_Conf.h"
#include "ILI9341_Touchscreen.h"
#include "usart.h"
#include "ILI9341_STM32_Driver.h"

//Internal Touchpad command, do not call directly
uint8_t TP_Read(uint8_t command)
{
	uint8_t i = 0;

	HAL_SPI_Receive(HSPI_INSTANCE, &i, 1, 10000);

	return i;
}

//Internal Touchpad command, do not call directly
uint16_t TP_Write(uint8_t value)
{
	uint8_t i = 0x00;
	while ((HSPI_INSTANCE)->State != HAL_SPI_STATE_READY)
		;
	HAL_GPIO_WritePin(TOUCH_CS_GPIO_Port, TOUCH_CS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(HSPI_INSTANCE, (uint8_t *)&value, 1, 10000);
	HAL_SPI_Receive(HSPI_INSTANCE, (uint8_t *)&i, 1, 1000);
	HAL_GPIO_WritePin(TOUCH_CS_GPIO_Port, TOUCH_CS_Pin, GPIO_PIN_SET);
	while ((HSPI_INSTANCE)->State != HAL_SPI_STATE_READY)
		;
	return i;
}

//Read coordinates of touchscreen press. Position[0] = X, Position[1] = Y
uint8_t TP_Read_Coordinates(uint16_t Coordinates[2])
{
	uint32_t avg_x = 0, avg_y = 0;
	uint16_t rawx = 0, rawy = 0;
	uint32_t calculating_x = 0, calculating_y = 0;

	uint32_t samples = NO_OF_POSITION_SAMPLES;
	uint32_t counted_samples = 0;

	// while (samples > 0 && TP_Touchpad_Pressed())
	while (samples > 0)
	{

		const uint8_t cmd_read_y = XPT_START_BIT | XPT_Y_POS_BIT;
		const uint8_t cmd_read_x = XPT_START_BIT | XPT_X_POS_BIT;

		rawy = TP_Write(cmd_read_y);
		avg_y += rawy;
		calculating_y += rawy;

		rawx = TP_Write(cmd_read_x);

		avg_x += rawx;
		calculating_x += rawx;
		samples--;
		counted_samples++;
	};

	if ((counted_samples >= NO_OF_POSITION_SAMPLES))
	{

		calculating_x /= counted_samples;
		calculating_y /= counted_samples;

		rawx = calculating_x;
		rawy = calculating_y;

		// these points are calculated according to an: https://www.analog.com/media/en/technical-documentation/application-notes/AN-1021.pdf//
		// the 3 reference points are: (75,150) (150,210) (220,75)
		// the follow matrixes are used to compute de KX1, KX2, KX3; KY1, KY2, KY3
		//   [31  60 1]   [ KX1 ]    [ 75  ]
		//   [85  96 1] * [ KX2 ] =  [ 150 ]
		//   [125 34 1]   [ KX3 ]    [ 220 ]
		//	   and
		//   [31  60 1]   [ KY1 ]    [ 150 ]
		//   [85  96 1] * [ KY2 ] =  [ 210 ]
		//   [125 34 1]   [ KY3 ]    [ 75  ]

		Coordinates[0] = (1.4975 * rawx) + (-0.1629 * rawy) + (38.3521);
		Coordinates[1] = (-0.2381 * rawx) + (2.0238 * rawy) + (35.9524);

		// ILI9341_Draw_Pixel(Coordinates[0], Coordinates[1], WHITE);

		return 1;
	}
	else
	{
		Coordinates[0] = 0;
		Coordinates[1] = 0;
		return 0;
	}

	return 1;
}

//Check if Touchpad was pressed. Returns TOUCHPAD_PRESSED (1) or TOUCHPAD_NOT_PRESSED (0)
#if USE_TOUCH_INTERRUPT
uint8_t TP_Touchpad_Pressed(void)
{
	const uint8_t comand = XPT_START_BIT;
	HAL_Delay(10);
	// TP_Write(comand);
	TP_Write(comand);
	if (!IRQ_Pin_State())
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
#endif