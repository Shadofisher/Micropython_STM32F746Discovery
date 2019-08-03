/*
 * huff.c
 *
 *  Created on: 28 Jul 2019
 *      Author: graeme
 */






/******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "py/nlr.h"

/*************************************GF Added**********************************/
#include "py/mpfile.h"
#include "py/builtin.h"
#include "py/misc.h"
/*******************************************************************************/


#include "py/runtime.h"
#include "py/mphal.h"

#include "timer.h"
#include "huff.h"
#include "pin.h"
#include "genhdr/pins.h"



typedef struct _pyb_huff_obj_t {
    mp_obj_base_t base;

    // hardware control for the LCD
} pyb_huff_obj_t;

/* Micro Python bindings                                                      */
void huff_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    //pyb_huff_obj_t *self = self_in;
    mp_printf(print, "Huff");
}

/// \classmethod \constructor(id)
/// Create an LED object associated with the given LED:
///
///   - `id` is the LED number, 1-4.
STATIC mp_obj_t huff_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    pyb_huff_obj_t *huff = m_new_obj(pyb_huff_obj_t);

    huff->base.type = &pyb_huff_type;

    // get led number
    //mp_int_t led_id = mp_obj_get_int(args[0]);

    // return static led object
    return huff;
}
#define RAD(A)  (M_PI*((float)(A))/180.0)
static void huff_xform(unsigned int x, unsigned int y)
{
	double r,angle;
	unsigned int n;
	unsigned int pixelvalue;

	pixelvalue = 0xffffffff;

	angle = 0.0;
	for (n = 0; n < 90; n++)
	{
		r = (int)(x*cosf(RAD(angle)) + y*sinf(RAD(angle)));
		angle = (angle + 1.0);
		if (*(uint32_t *)(0xc0000000 + ((int)r*1920 + ((int)angle+240)*4)) != 0xff000000)
		{
			*(uint32_t *)(0xc0000000 + ((int)r*1920 + ((int)angle+240)*4)) = pixelvalue;
			pixelvalue -= 50;

		}else
		{
			*(uint32_t *)(0xc0000000 + ((int)r*1920 + ((int)angle+240)*4)) = 0xffffffff;
		}
	}

	return;
}

//STATIC mp_obj_t pyb_huff_readfile(mp_uint_t n_args, const mp_obj_t *args)
STATIC const mp_obj_type_t mp_file_type;

mp_file_t *mp_file_from_file_obj(mp_obj_t file_obj)
{
    mp_file_t *file = m_new_obj_with_finaliser(mp_file_t);
    memset(file, 0, sizeof(*file));
    file->base.type = &mp_file_type;
    file->file_obj = file_obj;
    file->readinto_fn = mp_const_none;
    file->seek_fn = mp_const_none;
    file->tell_fn = mp_const_none;

    return file;
}



STATIC mp_obj_t pyb_huff_readfile(mp_uint_t n_args, const mp_obj_t *argss)
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
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_huff_readfile_obj,2,2,pyb_huff_readfile);


STATIC mp_obj_t pyb_huff_transform(mp_uint_t n_args, const mp_obj_t *args)
{
	//find a pixel
	unsigned int n,m;
	unsigned int * pixel;
	pixel = (unsigned int *)(0xc0000000);
	for (n = 0; n < 272; n++)
	{
		for (m = 0; m < 240 ; m++)
		{
			if (*pixel == 0xff000000)
			{
				*pixel = 0xFFffffff;
				 huff_xform(m,n);
				//x = m;
				//y = n;
			}else
			{
				//*pixel = 0;
			}
			pixel++;
		}
		pixel += (240);
	}

	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_huff_transform_obj,1,1,pyb_huff_transform);


/// \method show()
///
/// Show the hidden buffer on the screen.
STATIC mp_obj_t pyb_huff_memmove(mp_uint_t n_args, const mp_obj_t *args) {
    //pyb_lcd_obj_t *self = self_in;
	mp_obj_t a;
	//mp_obj_t mp_obj_new_str("Not implemented!", 16);
	a= mp_obj_new_str("Not yet implemented",19,1);
    return a;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_huff_memmove_obj,5,5,pyb_huff_memmove);

STATIC const mp_rom_map_elem_t huff_locals_dict_table[] = {

		{ MP_ROM_QSTR(MP_QSTR_memmove), MP_ROM_PTR(&pyb_huff_memmove_obj) },
		{ MP_ROM_QSTR(MP_QSTR_transform), MP_ROM_PTR(&pyb_huff_transform_obj) },
		{ MP_ROM_QSTR(MP_QSTR_readfile), MP_ROM_PTR(&pyb_huff_readfile_obj) },

};
STATIC MP_DEFINE_CONST_DICT(huff_locals_dict, huff_locals_dict_table);

const mp_obj_type_t pyb_huff_type = {
    { &mp_type_type },
    .name = MP_QSTR_HUFF,
    .print = huff_obj_print,
    .make_new = huff_obj_make_new,
    .locals_dict = (mp_obj_dict_t*)&huff_locals_dict,
};
