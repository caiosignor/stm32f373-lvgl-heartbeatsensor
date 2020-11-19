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

#ifndef XPT_COMMANDS_H
#define XPT_COMMANDS_H

#define XPT_START_BIT   (1 << 7)
#define XPT_X_POS_BIT   (1 << 6) | (0 << 5) | (1 << 4)
#define XPT_Y_POS_BIT   (0 << 6) | (0 << 5) | (1 << 4)
#define XPT_12BITS_MODE (0 << 3)
#define XPT_8BITS_MODE  (1 << 3)
#define XPT_SER_DFR_BIT (0 << 2)
#define XPT_PD1_BIT     (1 << 1)
#define XPT_PD0_BIT     (1 << 0)

#endif
