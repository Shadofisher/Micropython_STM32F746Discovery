/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "py/mphal.h"
#include "py/nlr.h"

/*************************************GF Added**********************************/
#include "py/mpfile.h"
#include "py/builtin.h"
#include "py/misc.h"
/*******************************************************************************/

#include "py/runtime.h"
#if MICROPY_HW_HAS_LCD

#include "pin.h"
#include "genhdr/pins.h"
#include "bufhelper.h"
#include "spi.h"
#include "font_petme128_8x8.h"
#include "lcd.h"
#include "lcd_init.h"
#include "ts_init.h"
#include "font24.h"
#include "../../lib/oofatfs/ff.h"
extern const uint8_t Font24_Table[];


/// \moduleref pyb
/// \class LCD - LCD control for the LCD touch-sensor pyskin
///
/// The LCD class is used to control the LCD on the LCD touch-sensor pyskin,
/// LCD32MKv1.0.  The LCD is a 128x32 pixel monochrome screen, part NHD-C12832A1Z.
///
/// The pyskin must be connected in either the X or Y positions, and then
/// an LCD object is made using:
///
///     lcd = pyb.LCD('X')      # if pyskin is in the X position
///     lcd = pyb.LCD('Y')      # if pyskin is in the Y position
///
/// Then you can use:
///
///     lcd.light(True)                 # turn the backlight on
///     lcd.write('Hello world!\n')     # print text to the screen
///
/// This driver implements a double buffer for setting/getting pixels.
/// For example, to make a bouncing dot, try:
///
///     x = y = 0
///     dx = dy = 1
///     while True:
///         # update the dot's position
///         x += dx
///         y += dy
///
///         # make the dot bounce of the edges of the screen
///         if x <= 0 or x >= 127: dx = -dx
///         if y <= 0 or y >= 31: dy = -dy
///
///         lcd.fill(0)                 # clear the buffer
///         lcd.pixel(x, y, 1)          # draw the dot
///         lcd.show()                  # show the buffer
///         pyb.delay(50)               # pause for 50ms

#define LCD_INSTR (0)
#define LCD_DATA (1)

#define LCD_CHAR_BUF_W (16)
#define LCD_CHAR_BUF_H (4)

#define LCD_PIX_BUF_W (128)
#define LCD_PIX_BUF_H (32)
#define LCD_PIX_BUF_BYTE_SIZE (LCD_PIX_BUF_W * LCD_PIX_BUF_H / 8)

typedef struct _pyb_lcd_obj_t {
    mp_obj_base_t base;

    // hardware control for the LCD
    SPI_HandleTypeDef *spi;
    const pin_obj_t *pin_cs1;
    const pin_obj_t *pin_rst;
    const pin_obj_t *pin_a0;
    const pin_obj_t *pin_bl;

    // character buffer for stdout-like output
    char char_buffer[LCD_CHAR_BUF_W * LCD_CHAR_BUF_H];
    int line;
    int column;
    int next_line;

    // double buffering for pixel buffer
    byte pix_buf[LCD_PIX_BUF_BYTE_SIZE];
    byte pix_buf2[LCD_PIX_BUF_BYTE_SIZE];
} pyb_lcd_obj_t;

STATIC void lcd_delay(void) {
    __asm volatile ("nop\nnop");
}

void NewSystemClock_Config(void)
{


    /**Configure the main internal regulator output voltage
    */
//	LTDC_HandleTypeDef  hLtdcHandler;
//	hLtdcHandler.Init.HorizontalSync = 0;
//	if (hLtdcHandler.Init.HorizontalSync == 0)
//	{
//		hLtdcHandler.Init.HorizontalSync = 1;
//	}

  __HAL_RCC_PWR_CLK_ENABLE();

}


STATIC void lcd_out(pyb_lcd_obj_t *lcd, int instr_data, uint8_t i) {
    lcd_delay();
    mp_hal_pin_low(lcd->pin_cs1); // CS=0; enable
    if (instr_data == LCD_INSTR) {
        mp_hal_pin_low(lcd->pin_a0); // A0=0; select instr reg
    } else {
        mp_hal_pin_high(lcd->pin_a0); // A0=1; select data reg
    }
    lcd_delay();
    HAL_SPI_Transmit(lcd->spi, &i, 1, 1000);
    lcd_delay();
    mp_hal_pin_high(lcd->pin_cs1); // CS=1; disable
}

// write a string to the LCD at the current cursor location
// output it straight away (doesn't use the pixel buffer)
STATIC void lcd_write_strn(pyb_lcd_obj_t *lcd, const char *str, unsigned int len) {
    int redraw_min = lcd->line * LCD_CHAR_BUF_W + lcd->column;
    int redraw_max = redraw_min;
    for (; len > 0; len--, str++) {
        // move to next line if needed
        if (lcd->next_line) {
            if (lcd->line + 1 < LCD_CHAR_BUF_H) {
                lcd->line += 1;
            } else {
                lcd->line = LCD_CHAR_BUF_H - 1;
                for (int i = 0; i < LCD_CHAR_BUF_W * (LCD_CHAR_BUF_H - 1); i++) {
                    lcd->char_buffer[i] = lcd->char_buffer[i + LCD_CHAR_BUF_W];
                }
                for (int i = 0; i < LCD_CHAR_BUF_W; i++) {
                    lcd->char_buffer[LCD_CHAR_BUF_W * (LCD_CHAR_BUF_H - 1) + i] = ' ';
                }
                redraw_min = 0;
                redraw_max = LCD_CHAR_BUF_W * LCD_CHAR_BUF_H;
            }
            lcd->next_line = 0;
            lcd->column = 0;
        }
        if (*str == '\n') {
            lcd->next_line = 1;
        } else if (*str == '\r') {
            lcd->column = 0;
        } else if (*str == '\b') {
            if (lcd->column > 0) {
                lcd->column--;
                redraw_min = 0; // could optimise this to not redraw everything
            }
        } else if (lcd->column >= LCD_CHAR_BUF_W) {
            lcd->next_line = 1;
            str -= 1;
            len += 1;
        } else {
            lcd->char_buffer[lcd->line * LCD_CHAR_BUF_W + lcd->column] = *str;
            lcd->column += 1;
            int max = lcd->line * LCD_CHAR_BUF_W + lcd->column;
            if (max > redraw_max) {
                redraw_max = max;
            }
        }
    }

    // we must draw upside down, because the LCD is upside down
    for (int i = redraw_min; i < redraw_max; i++) {
        uint page = i / LCD_CHAR_BUF_W;
        uint offset = 8 * (LCD_CHAR_BUF_W - 1 - (i - (page * LCD_CHAR_BUF_W)));
        lcd_out(lcd, LCD_INSTR, 0xb0 | page); // page address set
        lcd_out(lcd, LCD_INSTR, 0x10 | ((offset >> 4) & 0x0f)); // column address set upper
        lcd_out(lcd, LCD_INSTR, 0x00 | (offset & 0x0f)); // column address set lower
        int chr = lcd->char_buffer[i];
        if (chr < 32 || chr > 126) {
            chr = 127;
        }
        const uint8_t *chr_data = &font_petme128_8x8[(chr - 32) * 8];
        for (int j = 7; j >= 0; j--) {
            lcd_out(lcd, LCD_DATA, chr_data[j]);
        }
    }
}




/// \classmethod \constructor(skin_position)
///
/// Construct an LCD object in the given skin position.  `skin_position` can be 'X' or 'Y', and
/// should match the position where the LCD pyskin is plugged in.
STATIC mp_obj_t pyb_lcd_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    // get LCD position
   // const char *lcd_id = mp_obj_str_get_str(args[0]);

    // create lcd object
	pyb_lcd_obj_t *lcd = m_new_obj(pyb_lcd_obj_t);
    lcd->base.type = &pyb_lcd_type;
    NewSystemClock_Config();

    BSP_LCD_Init();	//Initialise the LCD
    BSP_LCD_LayerDefaultInit(LTDC_ACTIVE_LAYER, LCD_FRAME_BUFFER);
    Display_DemoDescription();
    TS_Init();

    return lcd;
}


/// \method show()
///
/// Show the hidden buffer on the screen.
STATIC mp_obj_t pyb_lcd_line(mp_uint_t n_args, const mp_obj_t *args) {
    //pyb_lcd_obj_t *self = self_in;
#if (1)
    uint16_t colour = mp_obj_get_int(args[1]);
	memset((uint16_t*)(0xC0000000),colour,518400);
#endif
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_lcd_line_obj,5,5,pyb_lcd_line);


STATIC mp_obj_t pyb_lcd_putpixelline(mp_uint_t n_args, const mp_obj_t *args) {
	int n;
	unsigned int * data = MP_OBJ_TO_PTR(args[1]);
	int x0 = mp_obj_get_int(args[2]);
	int y0 = mp_obj_get_int(args[3]);
	int len = mp_obj_get_int(args[4]);
	for (n = 0; n < len; n++)
	{
		*(uint32_t *)(0xc0000000 + y0 *1920 + x0*4 + n*4) = data[n]| 0xff000000;
	}

    return mp_const_none;

}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_lcd_putpixelline_obj, 5, 5, pyb_lcd_putpixelline);



STATIC mp_obj_t pyb_lcd_drawline(mp_uint_t n_args, const mp_obj_t *args) {
	int n;
	int x0 = mp_obj_get_int(args[1]);
	int y0 = mp_obj_get_int(args[2]);
	int x1 = mp_obj_get_int(args[3]);
	int y1 = mp_obj_get_int(args[4]);
	int x_interval,new_x,new_y;
	int y_interval;
	int interval;
	int signx,signy;
	unsigned int colour = mp_obj_get_int(args[5]);
    colour |= 0xff000000;

    x_interval = abs(x1-x0);
    y_interval = abs(y1-y0);

    if (x_interval >= y_interval)
    {
        interval = x_interval;
    }
    else
    {
        interval = y_interval;
    }

    for (n = 0; n < interval; n++)
    {
        signx=1;
        signy=1;
        if (x_interval >= y_interval)
        {
        	if (x0>x1)
                signx=-1;
            if (y0>y1)
                signy=-1;
            new_y = y0 + (int)(n*(y1-y0)/(interval));
            new_x = (x0+n*signx);
            //lcd.pixels(new_x,new_y,colour);
            *(uint32_t *)(0xc0000000 + (new_y*1920 + new_x*4)) = colour;

        }else
        {
            if (x0>x1)
                signx=-1;
            if (y0>y1)
                signy=-1;
            new_x = x0 + (int)(n*abs(x1-x0)/(interval));
            new_y = (y0+n*signy);
            //lcd.pixels(new_x,new_y,colour);
            *(uint32_t *)(0xc0000000 + (new_y*1920 + new_x*4)) = colour;
        }
    }
    return mp_const_none;

}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_lcd_drawline_obj, 6, 6, pyb_lcd_drawline);



STATIC mp_obj_t pyb_lcd_showpartimage(mp_uint_t n_args, const mp_obj_t *argss)
{
	size_t len;
	const char *filename = mp_obj_str_get_data(argss[1], &len);
	int x0     = mp_obj_get_int(argss[2]);
	int y0     = mp_obj_get_int(argss[3]);
	int width  = mp_obj_get_int(argss[4]);
	int height = mp_obj_get_int(argss[5]);
	int n;
	unsigned char buf[1920];

	unsigned int * start;
   // 	mp_obj_t bytes_read;



	//unsigned char buf[1920];
	width = width*4;
	mp_file_t * mp_file;
	mp_obj_t filename_obj = mp_obj_new_str(filename, strlen(filename), false);
	mp_obj_t mode_obj = mp_obj_new_str("r", strlen("r"), true);
	mp_obj_t args[2] = { filename_obj, mode_obj };
	mp_file = mp_file_from_file_obj(mp_builtin_open(2, args, (mp_map_t *)&mp_const_empty_map));

	mp_file->readinto_fn = mp_load_attr(mp_file->file_obj, MP_QSTR_readinto);
    	mp_obj_t bytearray = mp_obj_new_bytearray_by_ref(width, buf);
    	mp_obj_t bytes_read = mp_call_function_1(mp_file->readinto_fn, bytearray);

    	for (int n = 0; n < height; n++)
    	{
        	start = (unsigned char *)(0xc0000000 + (x0 *4) + (y0 +n)*1920);
        	memcpy(start,buf,width);
       		bytes_read = mp_call_function_1(mp_file->readinto_fn, bytearray);
    	}
	//mp_file = mp_open("test.txt", "r");

	return bytes_read;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_lcd_showpartimage_obj,6,6,pyb_lcd_showpartimage);


STATIC mp_obj_t pyb_lcd_showimage(mp_uint_t n_args, const mp_obj_t *argss)
{
	size_t len;
	const char *filename = mp_obj_str_get_data(argss[1], &len);

	unsigned char * buf = (unsigned char *)0xc0000000;
	mp_file_t * mp_file;
	mp_obj_t filename_obj = mp_obj_new_str(filename, strlen(filename), false);
	mp_obj_t mode_obj = mp_obj_new_str("r", strlen("r"), true);
	mp_obj_t args[2] = { filename_obj, mode_obj };
	mp_file = mp_file_from_file_obj(mp_builtin_open(2, args, (mp_map_t *)&mp_const_empty_map));

	mp_file->readinto_fn = mp_load_attr(mp_file->file_obj, MP_QSTR_readinto);
    mp_obj_t bytearray = mp_obj_new_bytearray_by_ref(/*num_bytes*/522240, buf);
    mp_obj_t bytes_read = mp_call_function_1(mp_file->readinto_fn, bytearray);

	//mp_file = mp_open("test.txt", "r");

	return bytes_read;



}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_lcd_showimage_obj,2,2,pyb_lcd_showimage);



/// \method show()
///
/// Show the hidden buffer on the screen.
STATIC mp_obj_t pyb_lcd_circle(mp_uint_t n_args, const mp_obj_t *args) {
    //pyb_lcd_obj_t *self = self_in;

    uint16_t  Xpos = mp_obj_get_int(args[1]);
    uint16_t  Ypos = mp_obj_get_int(args[2]);
    uint16_t  Radius = mp_obj_get_int(args[3]);
    uint32_t colour = mp_obj_get_int(args[4]);
    int32_t   decision;    /* Decision Variable */
    uint32_t  current_x;   /* Current X Value */
    uint32_t  current_y;   /* Current Y Value */
    decision = 3 - (Radius << 1);
    current_x = 0;
    current_y = Radius;
    colour |= 0xff000000;

//    *(uint32_t *)(0xc0000000 + (y*1920 + x*4)) = colour;

    while (current_x <= current_y)
    {
      //BSP_LCD_DrawPixel((Xpos + current_x), (Ypos - current_y), DrawProp[ActiveLayer].TextColor);
      *(uint32_t *)(0xc0000000 + ((Ypos - current_y)*1920 + (Xpos + current_x)*4)) = colour;

      //BSP_LCD_DrawPixel((Xpos - current_x), (Ypos - current_y), DrawProp[ActiveLayer].TextColor);
      *(uint32_t *)(0xc0000000 + ((Ypos - current_y)*1920 + (Xpos - current_x)*4)) = colour;


      //BSP_LCD_DrawPixel((Xpos + current_y), (Ypos - current_x), DrawProp[ActiveLayer].TextColor);
      *(uint32_t *)(0xc0000000 + ((Ypos - current_x)*1920 + (Xpos + current_y)*4)) = colour;

      //BSP_LCD_DrawPixel((Xpos - current_y), (Ypos - current_x), DrawProp[ActiveLayer].TextColor);
      *(uint32_t *)(0xc0000000 + ((Ypos - current_x)*1920 + (Xpos - current_y)*4)) = colour;

      //BSP_LCD_DrawPixel((Xpos + current_x), (Ypos + current_y), DrawProp[ActiveLayer].TextColor);
      *(uint32_t *)(0xc0000000 + ((Ypos + current_y)*1920 + (Xpos + current_x)*4)) = colour;

      //BSP_LCD_DrawPixel((Xpos - current_x), (Ypos + current_y), DrawProp[ActiveLayer].TextColor);
      *(uint32_t *)(0xc0000000 + ((Ypos + current_y)*1920 + (Xpos - current_x)*4)) = colour;


      //BSP_LCD_DrawPixel((Xpos + current_y), (Ypos + current_x), DrawProp[ActiveLayer].TextColor);
      *(uint32_t *)(0xc0000000 + ((Ypos + current_x)*1920 + (Xpos + current_y)*4)) = colour;

      //BSP_LCD_DrawPixel((Xpos - current_y), (Ypos + current_x), DrawProp[ActiveLayer].TextColor);
      *(uint32_t *)(0xc0000000 + ((Ypos + current_x)*1920 + (Xpos - current_y)*4)) = colour;

      if (decision < 0)
      {
        decision += (current_x << 2) + 6;
      }
      else
      {
        decision += ((current_x - current_y) << 2) + 10;
        current_y--;
      }
      current_x++;
    }


    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_lcd_circle_obj,5,5,pyb_lcd_circle);




/// \method command(instr_data, buf)
///
/// Send an arbitrary command to the LCD.  Pass 0 for `instr_data` to send an
/// instruction, otherwise pass 1 to send data.  `buf` is a buffer with the
/// instructions/data to send.
STATIC mp_obj_t pyb_lcd_command(mp_obj_t self_in, mp_obj_t instr_data_in, mp_obj_t val) {
    pyb_lcd_obj_t *self = self_in;

    // get whether instr or data
    int instr_data = mp_obj_get_int(instr_data_in);

    // get the buffer to send from
    mp_buffer_info_t bufinfo;
    uint8_t data[1];
    pyb_buf_get_for_send(val, &bufinfo, data);

    // send the data
    for (uint i = 0; i < bufinfo.len; i++) {
        lcd_out(self, instr_data, ((byte*)bufinfo.buf)[i]);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(pyb_lcd_command_obj, pyb_lcd_command);


STATIC mp_obj_t pyb_lcd_text(mp_uint_t n_args, const mp_obj_t *args) {
    // extract arguments
    //pyb_lcd_obj_t *self = args[0];
    size_t len;
    unsigned int n,m,j;
    const char *data = mp_obj_str_get_data(args[1], &len);
    int x0 = mp_obj_get_int(args[2]);
    int y0 = mp_obj_get_int(args[3]);
    unsigned int colour = mp_obj_get_int(args[4]);
  //
    //uint16_t test;
    sFONT Font24;
    Font24.table = Font24_Table;
    colour |= 0xff000000;
#if 1
    for (m = 0; m < 24; m++)
    {
        //y0++;
        //test = GF_Font11x18[((data[0]-32)*18)+(m)];
    	//test = Font24.table[(data[0]-32)*24 +m];
    	//test = Font24.table[(24*3)+m*3];
    	for (n = 0; n < 3; n++)
        {
        	for (j = 0; j < 8; j++)
        	{
        		if (((Font24.table[((data[0]-32)*24*3)+m*3+n]) >> (7-j)) & 0x01)
        		{
            		*(uint32_t *)(0xc0000000 + ((y0+m)*1920) +x0*4 + n*32+j*4) = colour;
        		}

        	}
        }

    }

#endif
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_lcd_text_obj, 5, 5, pyb_lcd_text);

/// \method contrast(value)
///
/// Set the contrast of the LCD.  Valid values are between 0 and 47.
STATIC mp_obj_t pyb_lcd_contrast(mp_obj_t self_in, mp_obj_t contrast_in) {
    pyb_lcd_obj_t *self = self_in;
    int contrast = mp_obj_get_int(contrast_in);
    if (contrast < 0) {
        contrast = 0;
    } else if (contrast > 0x2f) {
        contrast = 0x2f;
    }
    lcd_out(self, LCD_INSTR, 0x81); // electronic volume mode set
    lcd_out(self, LCD_INSTR, contrast); // electronic volume register set
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_lcd_contrast_obj, pyb_lcd_contrast);

/// \method light(value)
///
/// Turn the backlight on/off.  True or 1 turns it on, False or 0 turns it off.
STATIC mp_obj_t pyb_lcd_light(mp_obj_t self_in, mp_obj_t value) {
    pyb_lcd_obj_t *self = self_in;
    if (mp_obj_is_true(value)) {
        mp_hal_pin_high(self->pin_bl); // set pin high to turn backlight on
    } else {
        mp_hal_pin_low(self->pin_bl); // set pin low to turn backlight off
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_lcd_light_obj, pyb_lcd_light);

/// \method write(str)
///
/// Write the string `str` to the screen.  It will appear immediately.
STATIC mp_obj_t pyb_lcd_write(mp_obj_t self_in, mp_obj_t str) {
    pyb_lcd_obj_t *self = self_in;
    size_t len;
    const char *data = mp_obj_str_get_data(str, &len);
    lcd_write_strn(self, data, len);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_lcd_write_obj, pyb_lcd_write);

/// \method fill(colour)
///
/// Fill the screen with the given colour (0 or 1 for white or black).
///
/// This method writes to the hidden buffer.  Use `show()` to show the buffer.
STATIC mp_obj_t pyb_lcd_fill(mp_obj_t self_in, mp_obj_t col_in) {
    pyb_lcd_obj_t *self = self_in;
    int col = mp_obj_get_int(col_in);
    if (col) {
        col = 0xff;
    }
    memset(self->pix_buf, col, LCD_PIX_BUF_BYTE_SIZE);
    memset(self->pix_buf2, col, LCD_PIX_BUF_BYTE_SIZE);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_lcd_fill_obj, pyb_lcd_fill);

/// \method get(x, y)
///
/// Get the pixel at the position `(x, y)`.  Returns 0 or 1.
///
/// This method reads from the visible buffer.
STATIC mp_obj_t pyb_lcd_get(mp_obj_t self_in, mp_obj_t x_in, mp_obj_t y_in) {
    pyb_lcd_obj_t *self = self_in;
    int x = mp_obj_get_int(x_in);
    int y = mp_obj_get_int(y_in);
    if (0 <= x && x <= 127 && 0 <= y && y <= 31) {
        uint byte_pos = x + 128 * ((uint)y >> 3);
        if (self->pix_buf[byte_pos] & (1 << (y & 7))) {
            return mp_obj_new_int(1);
        }
    }
    return mp_obj_new_int(0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(pyb_lcd_get_obj, pyb_lcd_get);

/// \method pixel(x, y, colour)
///
/// Set the pixel at `(x, y)` to the given colour (0 or 1).
///
/// This method writes to the hidden buffer.  Use `show()` to show the buffer.
STATIC mp_obj_t pyb_lcd_pixel(mp_uint_t n_args, const mp_obj_t *args) {
    pyb_lcd_obj_t *self = args[0];
    int x = mp_obj_get_int(args[1]);
    int y = mp_obj_get_int(args[2]);
    if (0 <= x && x <= 127 && 0 <= y && y <= 31) {
        uint byte_pos = x + 128 * ((uint)y >> 3);
        if (mp_obj_get_int(args[3]) == 0) {
            self->pix_buf2[byte_pos] &= ~(1 << (y & 7));
        } else {
            self->pix_buf2[byte_pos] |= 1 << (y & 7);
        }
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_lcd_pixel_obj, 4, 4, pyb_lcd_pixel);


/// \method show()
///
/// Show the hidden buffer on the screen.
STATIC mp_obj_t pyb_lcd_show(mp_obj_t self_in) {
    pyb_lcd_obj_t *self = self_in;
    memcpy(self->pix_buf, self->pix_buf2, LCD_PIX_BUF_BYTE_SIZE);
    for (uint page = 0; page < 4; page++) {
        lcd_out(self, LCD_INSTR, 0xb0 | page); // page address set
        lcd_out(self, LCD_INSTR, 0x10); // column address set upper; 0
        lcd_out(self, LCD_INSTR, 0x00); // column address set lower; 0
        for (uint i = 0; i < 128; i++) {
            lcd_out(self, LCD_DATA, self->pix_buf[128 * page + 127 - i]);
        }
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_lcd_show_obj, pyb_lcd_show);


/// \method show()
///
/// Show the hidden buffer on the screen.
STATIC mp_obj_t pyb_lcd_clear(mp_uint_t n_args, const mp_obj_t *args) {
    //pyb_lcd_obj_t *self = self_in;
	unsigned int n,m;
    unsigned int colour = mp_obj_get_int(args[1]);
    colour |= 0xff000000;
    for (n = 0; n < 272; n++)
    {
    	for (m = 0; m < 480; m++)
    	{
    		*(uint32_t*)(0xC0000000 + (1920*n)+ (m*4)) = colour;
    	}
    }
	//memset((uint32_t*)(0xC0000000),colour,518400);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_lcd_clear_obj,2,2,pyb_lcd_clear);

/// \method show()
///
/// Show the hidden buffer on the screen.
STATIC mp_obj_t pyb_lcd_thickpixels(mp_uint_t n_args, const mp_obj_t *args) {
    //pyb_lcd_obj_t *self = self_in;
    unsigned int x = mp_obj_get_int(args[1]);
    unsigned int y = mp_obj_get_int(args[2]);
    unsigned int colour = mp_obj_get_int(args[3]);
    colour |= 0xff000000;
    *(uint32_t *)(0xc0000000 + (y*1920 + x*4)) = colour;
    *(uint32_t *)(0xc0000000 + (y*1920 + x*4) -4) = colour;
    *(uint32_t *)(0xc0000000 + (y*1920 + x*4) +4) = colour;

    *(uint32_t *)(0xc0000000 + ((y-1)*1920 + x*4)) = colour;
    *(uint32_t *)(0xc0000000 + ((y-1)*1920 + x*4)+4) = colour;
    *(uint32_t *)(0xc0000000 + ((y-1)*1920 + x*4)-4) = colour;

    *(uint32_t *)(0xc0000000 + ((y+1)*1920 + x*4)) = colour;
    *(uint32_t *)(0xc0000000 + ((y+1)*1920 + x*4)+4) = colour;
    *(uint32_t *)(0xc0000000 + ((y+1)*1920 + x*4)-4) = colour;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_lcd_thickpixels_obj,4,4,pyb_lcd_thickpixels);



/// \method show()
///
/// Show the hidden buffer on the screen.
STATIC mp_obj_t pyb_lcd_fillarea(mp_uint_t n_args, const mp_obj_t *args) {
    //pyb_lcd_obj_t *self = self_in;
	uint16_t length_x,length_y,n,m;
	unsigned int start_pos;
	unsigned int x0 = mp_obj_get_int(args[1]);
    unsigned int y0 = mp_obj_get_int(args[2]);
    unsigned int x1 = mp_obj_get_int(args[3]);
    unsigned int y1 = mp_obj_get_int(args[4]);
    unsigned int colour = mp_obj_get_int(args[5]);

    length_x = x1-x0;
    length_y = y1-y0;

    colour |= (0xff000000);

    for (n = 0; n < length_y; n++)
    {
        start_pos = (0xc0000000 + ((y0+n)*1920 + x0*4));
        for (m = 0; m < length_x; m++)
        {
        	*(uint32_t *)(start_pos + m*4) = colour;
        }
    }


    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_lcd_fillarea_obj,6,6,pyb_lcd_fillarea);

/// \method show()
///
/// Show the hidden buffer on the screen.
STATIC mp_obj_t pyb_lcd_pixels(mp_uint_t n_args, const mp_obj_t *args) {
    //pyb_lcd_obj_t *self = self_in;
    unsigned int x = mp_obj_get_int(args[1]);
    unsigned int y = mp_obj_get_int(args[2]);
    unsigned int colour = mp_obj_get_int(args[3]);
    colour |= 0xff000000;
    *(uint32_t *)(0xc0000000 + (y*1920 + x*4)) = colour;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_lcd_pixels_obj,4,4,pyb_lcd_pixels);

/// \method show()
///
/// Show the hidden buffer on the screen.
STATIC mp_obj_t pyb_lcd_memmove(mp_uint_t n_args, const mp_obj_t *args) {
    //pyb_lcd_obj_t *self = self_in;

    unsigned int y,dest,src;
	unsigned int x0 = mp_obj_get_int(args[1]);
    unsigned int y0 = mp_obj_get_int(args[2]);
	unsigned int x1 = mp_obj_get_int(args[3]);
    unsigned int y1 = mp_obj_get_int(args[4]);
    unsigned int size_x = x1-x0;
    unsigned int size_y = y1-y0;


	//uint16_t colour = mp_obj_get_int(args[1]);
	//memset((uint16_t*)(0xD0000000),colour,153600);
	for (y=0; y< size_y; y++)
	{
		dest = (0xc0000000 + ((y0+y)*1920) + (x0*4) - 4);
		src =  (0xc0000000 + ((y0+y)*1920) + (x0*4));
		memcpy((unsigned int *)dest,(unsigned int *)src,(size_x*4));
	}

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_lcd_memmove_obj,5,5,pyb_lcd_memmove);



STATIC const mp_rom_map_elem_t pyb_lcd_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_command), MP_ROM_PTR(&pyb_lcd_command_obj) },
    { MP_ROM_QSTR(MP_QSTR_contrast), MP_ROM_PTR(&pyb_lcd_contrast_obj) },
    { MP_ROM_QSTR(MP_QSTR_light), MP_ROM_PTR(&pyb_lcd_light_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&pyb_lcd_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_fill), MP_ROM_PTR(&pyb_lcd_fill_obj) },
    { MP_ROM_QSTR(MP_QSTR_get), MP_ROM_PTR(&pyb_lcd_get_obj) },
    { MP_ROM_QSTR(MP_QSTR_pixel), MP_ROM_PTR(&pyb_lcd_pixel_obj) },
    { MP_ROM_QSTR(MP_QSTR_text), MP_ROM_PTR(&pyb_lcd_text_obj) },
    { MP_ROM_QSTR(MP_QSTR_show), MP_ROM_PTR(&pyb_lcd_show_obj) },
    { MP_ROM_QSTR(MP_QSTR_line), MP_ROM_PTR(&pyb_lcd_line_obj) },
    { MP_ROM_QSTR(MP_QSTR_thickpixels), MP_ROM_PTR(&pyb_lcd_thickpixels_obj) },
    { MP_ROM_QSTR(MP_QSTR_fillarea), MP_ROM_PTR(&pyb_lcd_fillarea_obj) },
	{ MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&pyb_lcd_clear_obj) },
    { MP_ROM_QSTR(MP_QSTR_memmove), MP_ROM_PTR(&pyb_lcd_memmove_obj) },
    { MP_ROM_QSTR(MP_QSTR_pixels), MP_ROM_PTR(&pyb_lcd_pixels_obj) },
    { MP_ROM_QSTR(MP_QSTR_circle), MP_ROM_PTR(&pyb_lcd_circle_obj) },
    { MP_ROM_QSTR(MP_QSTR_putpixelline), MP_ROM_PTR(&pyb_lcd_putpixelline_obj)},
	{ MP_ROM_QSTR(MP_QSTR_drawline), MP_ROM_PTR(&pyb_lcd_drawline_obj)},
	{ MP_ROM_QSTR(MP_QSTR_showimage), MP_ROM_PTR(&pyb_lcd_showimage_obj)},
	{ MP_ROM_QSTR(MP_QSTR_showpartimage), MP_ROM_PTR(&pyb_lcd_showpartimage_obj)}

};
STATIC MP_DEFINE_CONST_DICT(pyb_lcd_locals_dict, pyb_lcd_locals_dict_table);

const mp_obj_type_t pyb_lcd_type = {
    { &mp_type_type },
    .name = MP_QSTR_LCD,
    .make_new = pyb_lcd_make_new,
    .locals_dict = (mp_obj_dict_t*)&pyb_lcd_locals_dict,
};






#endif // MICROPY_HW_HAS_LCD
