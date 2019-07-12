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

#include "py/mphal.h"
#include "py/nlr.h"
#include "py/runtime.h"

#if MICROPY_HW_HAS_LCD

#include "pin.h"
#include "genhdr/pins.h"
#include "bufhelper.h"
#include "spi.h"
#include "font_petme128_8x8.h"
#include "gf_font.h"
#include "lcd.h"

SDRAM_HandleTypeDef hsdram;
FMC_SDRAM_TimingTypeDef SDRAM_Timing;
FMC_SDRAM_CommandTypeDef command;
static void SDRAM_Initialization_Sequence(SDRAM_HandleTypeDef *hsdram, FMC_SDRAM_CommandTypeDef *Command);
void SDRAM_Init(void);

#define SDRAM_MEMORY_WIDTH            FMC_SDRAM_MEM_BUS_WIDTH_16

/* #define SDCLOCK_PERIOD                   FMC_SDRAM_CLOCK_PERIOD_2 */
#define SDCLOCK_PERIOD                FMC_SDRAM_CLOCK_PERIOD_3


#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200)
#define BUFFER_SIZE         ((uint32_t)0x0100)
#define WRITE_READ_ADDR     ((uint32_t)0x0800)
#define REFRESH_COUNT       ((uint32_t)0x056A)   /* SDRAM refresh counter (90MHz SDRAM clock) */





#define  ILI9341_HSYNC            ((uint32_t)9)   /* Horizontal synchronization */
#define  ILI9341_HBP              ((uint32_t)29)    /* Horizontal back porch      */
#define  ILI9341_HFP              ((uint32_t)2)    /* Horizontal front porch     */
#define  ILI9341_VSYNC            ((uint32_t)1)   /* Vertical synchronization   */
#define  ILI9341_VBP              ((uint32_t)3)    /* Vertical back porch        */
#define  ILI9341_VFP              ((uint32_t)2)    /* Vertical front porch       */

/**
  * @brief  ILI9341 Registers
  */

/* Level 1 Commands */
#define LCD_SWRESET             0x01   /* Software Reset */
#define LCD_READ_DISPLAY_ID     0x04   /* Read display identification information */
#define LCD_RDDST               0x09   /* Read Display Status */
#define LCD_RDDPM               0x0A   /* Read Display Power Mode */
#define LCD_RDDMADCTL           0x0B   /* Read Display MADCTL */
#define LCD_RDDCOLMOD           0x0C   /* Read Display Pixel Format */
#define LCD_RDDIM               0x0D   /* Read Display Image Format */
#define LCD_RDDSM               0x0E   /* Read Display Signal Mode */
#define LCD_RDDSDR              0x0F   /* Read Display Self-Diagnostic Result */
#define LCD_SPLIN               0x10   /* Enter Sleep Mode */
#define LCD_SLEEP_OUT           0x11   /* Sleep out register */
#define LCD_PTLON               0x12   /* Partial Mode ON */
#define LCD_NORMAL_MODE_ON      0x13   /* Normal Display Mode ON */
#define LCD_DINVOFF             0x20   /* Display Inversion OFF */
#define LCD_DINVON              0x21   /* Display Inversion ON */
#define LCD_GAMMA               0x26   /* Gamma register */
#define LCD_DISPLAY_OFF         0x28   /* Display off register */
#define LCD_DISPLAY_ON          0x29   /* Display on register */
#define LCD_COLUMN_ADDR         0x2A   /* Colomn address register */
#define LCD_PAGE_ADDR           0x2B   /* Page address register */
#define LCD_GRAM                0x2C   /* GRAM register */
#define LCD_RGBSET              0x2D   /* Color SET */
#define LCD_RAMRD               0x2E   /* Memory Read */
#define LCD_PLTAR               0x30   /* Partial Area */
#define LCD_VSCRDEF             0x33   /* Vertical Scrolling Definition */
#define LCD_TEOFF               0x34   /* Tearing Effect Line OFF */
#define LCD_TEON                0x35   /* Tearing Effect Line ON */
#define LCD_MAC                 0x36   /* Memory Access Control register*/
#define LCD_VSCRSADD            0x37   /* Vertical Scrolling Start Address */
#define LCD_IDMOFF              0x38   /* Idle Mode OFF */
#define LCD_IDMON               0x39   /* Idle Mode ON */
#define LCD_PIXEL_FORMAT        0x3A   /* Pixel Format register */
#define LCD_WRITE_MEM_CONTINUE  0x3C   /* Write Memory Continue */
#define LCD_READ_MEM_CONTINUE   0x3E   /* Read Memory Continue */
#define LCD_SET_TEAR_SCANLINE   0x44   /* Set Tear Scanline */
#define LCD_GET_SCANLINE        0x45   /* Get Scanline */
#define LCD_WDB                 0x51   /* Write Brightness Display register */
#define LCD_RDDISBV             0x52   /* Read Display Brightness */
#define LCD_WCD                 0x53   /* Write Control Display register*/
#define LCD_RDCTRLD             0x54   /* Read CTRL Display */
#define LCD_WRCABC              0x55   /* Write Content Adaptive Brightness Control */
#define LCD_RDCABC              0x56   /* Read Content Adaptive Brightness Control */
#define LCD_WRITE_CABC          0x5E   /* Write CABC Minimum Brightness */
#define LCD_READ_CABC           0x5F   /* Read CABC Minimum Brightness */
#define LCD_READ_ID1            0xDA   /* Read ID1 */
#define LCD_READ_ID2            0xDB   /* Read ID2 */
#define LCD_READ_ID3            0xDC   /* Read ID3 */

/* Level 2 Commands */
#define LCD_RGB_INTERFACE       0xB0   /* RGB Interface Signal Control */
#define LCD_FRMCTR1             0xB1   /* Frame Rate Control (In Normal Mode) */
#define LCD_FRMCTR2             0xB2   /* Frame Rate Control (In Idle Mode) */
#define LCD_FRMCTR3             0xB3   /* Frame Rate Control (In Partial Mode) */
#define LCD_INVTR               0xB4   /* Display Inversion Control */
#define LCD_BPC                 0xB5   /* Blanking Porch Control register */
#define LCD_DFC                 0xB6   /* Display Function Control register */
#define LCD_ETMOD               0xB7   /* Entry Mode Set */
#define LCD_BACKLIGHT1          0xB8   /* Backlight Control 1 */
#define LCD_BACKLIGHT2          0xB9   /* Backlight Control 2 */
#define LCD_BACKLIGHT3          0xBA   /* Backlight Control 3 */
#define LCD_BACKLIGHT4          0xBB   /* Backlight Control 4 */
#define LCD_BACKLIGHT5          0xBC   /* Backlight Control 5 */
#define LCD_BACKLIGHT7          0xBE   /* Backlight Control 7 */
#define LCD_BACKLIGHT8          0xBF   /* Backlight Control 8 */
#define LCD_POWER1              0xC0   /* Power Control 1 register */
#define LCD_POWER2              0xC1   /* Power Control 2 register */
#define LCD_VCOM1               0xC5   /* VCOM Control 1 register */
#define LCD_VCOM2               0xC7   /* VCOM Control 2 register */
#define LCD_NVMWR               0xD0   /* NV Memory Write */
#define LCD_NVMPKEY             0xD1   /* NV Memory Protection Key */
#define LCD_RDNVM               0xD2   /* NV Memory Status Read */
#define LCD_READ_ID4            0xD3   /* Read ID4 */
#define LCD_PGAMMA              0xE0   /* Positive Gamma Correction register */
#define LCD_NGAMMA              0xE1   /* Negative Gamma Correction register */
#define LCD_DGAMCTRL1           0xE2   /* Digital Gamma Control 1 */
#define LCD_DGAMCTRL2           0xE3   /* Digital Gamma Control 2 */
#define LCD_INTERFACE           0xF6   /* Interface control register */

/* Extend register commands */
#define LCD_POWERA               0xCB   /* Power control A register */
#define LCD_POWERB               0xCF   /* Power control B register */
#define LCD_DTCA                 0xE8   /* Driver timing control A */
#define LCD_DTCB                 0xEA   /* Driver timing control B */
#define LCD_POWER_SEQ            0xED   /* Power on sequence register */
#define LCD_3GAMMA_EN            0xF2   /* 3 Gamma enable register */
#define LCD_PRC                  0xF7   /* Pump ratio control register */

#define LCD_CS_LOW()       HAL_GPIO_WritePin(LCD_NCS_GPIO_PORT, LCD_NCS_PIN, GPIO_PIN_RESET)
#define LCD_CS_HIGH()      HAL_GPIO_WritePin(LCD_NCS_GPIO_PORT, LCD_NCS_PIN, GPIO_PIN_SET)

/* Set WRX High to send data */
#define LCD_WRX_LOW()      HAL_GPIO_WritePin(LCD_WRX_GPIO_PORT, LCD_WRX_PIN, GPIO_PIN_RESET)
#define LCD_WRX_HIGH()     HAL_GPIO_WritePin(LCD_WRX_GPIO_PORT, LCD_WRX_PIN, GPIO_PIN_SET)

/* Set WRX High to send data */
#define LCD_RDX_LOW()      HAL_GPIO_WritePin(LCD_RDX_GPIO_PORT, LCD_RDX_PIN, GPIO_PIN_RESET)
#define LCD_RDX_HIGH()     HAL_GPIO_WritePin(LCD_RDX_GPIO_PORT, LCD_RDX_PIN, GPIO_PIN_SET)

/**
  * @brief  LCD Control pin
  */
#define LCD_NCS_PIN                             GPIO_PIN_2
#define LCD_NCS_GPIO_PORT                       GPIOC
#define LCD_NCS_GPIO_CLK_ENABLE()               __HAL_RCC_GPIOC_CLK_ENABLE()
#define LCD_NCS_GPIO_CLK_DISABLE()              __HAL_RCC_GPIOC_CLK_DISABLE()
/**
  * @}
  */
/**
  * @brief  LCD Command/data pin
  */
#define LCD_WRX_PIN                             GPIO_PIN_13
#define LCD_WRX_GPIO_PORT                       GPIOD
#define LCD_WRX_GPIO_CLK_ENABLE()               __HAL_RCC_GPIOD_CLK_ENABLE()
#define LCD_WRX_GPIO_CLK_DISABLE()              __HAL_RCC_GPIOD_CLK_DISABLE()

#define LCD_RDX_PIN                             GPIO_PIN_12
#define LCD_RDX_GPIO_PORT                       GPIOD
#define LCD_RDX_GPIO_CLK_ENABLE()               __HAL_RCC_GPIOD_CLK_ENABLE()
#define LCD_RDX_GPIO_CLK_DISABLE()              __HAL_RCC_GPIOD_CLK_DISABLE()


/* Size of read registers */
#define LCD_READ_ID4_SIZE        3      /* Size of Read ID4 */


LTDC_HandleTypeDef LtdcHandle;
static void MX_GPIO_Init(void);
static void MX_LTDC_Init(void);
void ili9341_Init(void);


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

  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

    /**Configure the main internal regulator output voltage
    */
  __HAL_RCC_PWR_CLK_ENABLE();


  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
  PeriphClkInitStruct.PLLSAI.PLLSAIN = 50;
  PeriphClkInitStruct.PLLSAI.PLLSAIR = 2;
  PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
   // _Error_Handler(__FILE__, __LINE__);
  }
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

    __HAL_RCC_LTDC_CLK_ENABLE();
    LtdcHandle.Instance = LTDC;
    /* Configure the LTDC */
	LtdcHandle.Init.HorizontalSync = 9;

    if(HAL_LTDC_Init(&LtdcHandle) != HAL_OK)
    {
      /* Initialization Error */
     // Error_Handler();
    }


    SDRAM_Init();

    MX_GPIO_Init();
    MX_LTDC_Init();

    ili9341_Init();

    /* Polarity configuration */
    /* Initialize the horizontal synchronization polarity as active low */
    LtdcHandle.Init.HSPolarity = LTDC_HSPOLARITY_AL;
    /* Initialize the vertical synchronization polarity as active low */
    LtdcHandle.Init.VSPolarity = LTDC_VSPOLARITY_AL;
    /* Initialize the data enable polarity as active low */
    LtdcHandle.Init.DEPolarity = LTDC_DEPOLARITY_AL;
    /* Initialize the pixel clock polarity as input pixel clock */
    LtdcHandle.Init.PCPolarity = LTDC_PCPOLARITY_IPC;

    /* Timing configuration  (Typical configuration from ILI9341 datasheet)
        HSYNC=10 (9+1)
        HBP=20 (29-10+1)
        ActiveW=240 (269-20-10+1)
        HFP=10 (279-240-20-10+1)

        VSYNC=2 (1+1)
        VBP=2 (3-2+1)
        ActiveH=320 (323-2-2+1)
        VFP=4 (327-320-2-2+1)
    */

    /* Timing configuration */
    /* Horizontal synchronization width = Hsync - 1 */
    LtdcHandle.Init.HorizontalSync = 9;
    /* Vertical synchronization height = Vsync - 1 */
    LtdcHandle.Init.VerticalSync = 1;
    /* Accumulated horizontal back porch = Hsync + HBP - 1 */
    LtdcHandle.Init.AccumulatedHBP = 29;
    /* Accumulated vertical back porch = Vsync + VBP - 1 */
    LtdcHandle.Init.AccumulatedVBP = 3;
    /* Accumulated active width = Hsync + HBP + Active Width - 1 */
    LtdcHandle.Init.AccumulatedActiveH = 323;
    /* Accumulated active height = Vsync + VBP + Active Heigh - 1 */
    LtdcHandle.Init.AccumulatedActiveW = 269;
    /* Total height = Vsync + VBP + Active Heigh + VFP - 1 */
    LtdcHandle.Init.TotalHeigh = 327;
    /* Total width = Hsync + HBP + Active Width + HFP - 1 */
    LtdcHandle.Init.TotalWidth = 279;

    /* Configure R,G,B component values for LCD background color */
    LtdcHandle.Init.Backcolor.Blue = 0;
    LtdcHandle.Init.Backcolor.Green = 0;
    LtdcHandle.Init.Backcolor.Red = 0;

    LtdcHandle.Instance = LTDC;

    HAL_LTDC_MspInit(&LtdcHandle);





    return lcd;
}

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

/// \method text(str, x, y, colour)
///
/// Draw the given text to the position `(x, y)` using the given colour (0 or 1).
///
/// This method writes to the hidden buffer.  Use `show()` to show the buffer.
#if 0
STATIC mp_obj_t pyb_lcd_text(mp_uint_t n_args, const mp_obj_t *args) {
    // extract arguments
    //pyb_lcd_obj_t *self = args[0];
    size_t len;
    const char *data = mp_obj_str_get_data(args[1], &len);
    int x0 = mp_obj_get_int(args[2]);
    int y0 = mp_obj_get_int(args[3]);
    int col = mp_obj_get_int(args[4]);

    // loop over chars
    for (const char *top = data + len; data < top; data++) {
        // get char and make sure its in range of font
        uint chr = *(byte*)data;
        if (chr < 32 || chr > 127) {
            chr = 127;
        }
        // get char data
        const uint8_t *chr_data = &font_petme128_8x8[(chr - 32) * 8];
        // loop over char data
        for (uint j = 0; j < 8; j++, x0++) {
            if (0 <= x0 && x0 < LCD_PIX_BUF_W) { // clip x
                uint vline_data = chr_data[j]; // each byte of char data is a vertical column of 8 pixels, LSB at top
                for (int y = y0; vline_data; vline_data >>= 1, y++) { // scan over vertical column
                    if (vline_data & 1) { // only draw if pixel set
                        if (0 <= y && y < LCD_PIX_BUF_H) { // clip y
                            uint byte_pos = (10*480 + 0xd0000000+x0*2 + /*LCD_PIX_BUF_W*/480 * y);
                            if (col == 0) {
                                // clear pixel
                                //self->pix_buf2[byte_pos] &= ~(1 << (y & 7));
                            	*(uint32_t *)(byte_pos) = 0xffff;

                            } else {
                                // set pixel
                                //self->pix_buf2[byte_pos] |= 1 << (y & 7);
                            	*(uint32_t *)( byte_pos) = 0x0000;
                            }
                        }
                    }
                }
            }
        }
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_lcd_text_obj, 5, 5, pyb_lcd_text);
#else
#if 1
STATIC mp_obj_t pyb_lcd_text(mp_uint_t n_args, const mp_obj_t *args) {
    // extract arguments
    //pyb_lcd_obj_t *self = args[0];
    size_t len;
    unsigned int n,m;
    const char *data = mp_obj_str_get_data(args[1], &len);
    int x0 = mp_obj_get_int(args[2]);
    int y0 = mp_obj_get_int(args[3]);
    int colour = mp_obj_get_int(args[4]);
  //
    uint16_t test;

    for (m = 0; m < 18; m++)
    {
        //y0++;
        test = GF_Font11x18[((data[0]-32)*18)+(m)];
    	for (n = 0; n < 16; n++)
        {
        	if (((test >> (15-n)) & 0x01) == 1)
        	{
        		//*(uint16_t *)(0xd0000000 + ((y0+(16-n))*480 + x0*2 + (m*2)/*+ n*2 */)) = colour;
        		*(uint16_t *)(0xd0000000 + ((y0+m)*480) +x0*2 + n*2) = colour;
        	}//else
        	//{
        	//	*(uint16_t *)(0xd0000000 + (y0*480 + x0*2 + n*2)) = 0xffff;
        	//}
        }

    }


    return mp_const_none;
}
#else
STATIC mp_obj_t pyb_lcd_text(mp_uint_t n_args, const mp_obj_t *args) {
    // extract arguments
    //pyb_lcd_obj_t *self = args[0];
    size_t len;
    unsigned int n,m;
    const char *data = mp_obj_str_get_data(args[1], &len);
    int x0 = mp_obj_get_int(args[2]);
    int y0 = mp_obj_get_int(args[3]);
    int colour = mp_obj_get_int(args[4]);
  //
    unsigned char test;

    for (m = 0; m < 8; m++)
    {
        //y0++;
        test = font_petme128_8x8[((data[0]-32)*8)+(m)];
    	for (n = 0; n < 8; n++)
        {
        	if (((test >> (7-n)) & 0x01) == 1)
        	{
        		*(uint16_t *)(0xd0000000 + ((y0+(8-n))*480 + x0*2 + (m*2)/*+ n*2 */)) = colour;
        	}//else
        	//{
        	//	*(uint16_t *)(0xd0000000 + (y0*480 + x0*2 + n*2)) = 0xffff;
        	//}
        }

    }


    return mp_const_none;
}

#endif

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_lcd_text_obj, 5, 5, pyb_lcd_text);
#endif
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
STATIC mp_obj_t pyb_lcd_line(mp_uint_t n_args, const mp_obj_t *args) {
    //pyb_lcd_obj_t *self = self_in;
#if (1)
    uint16_t colour = mp_obj_get_int(args[1]);
	memset((uint16_t*)(0xD0000000),colour,153600);
#endif
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_lcd_line_obj,5,5,pyb_lcd_line);

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
		dest = (0xd0000000 + ((y0+y)*480) + (x0*2) - 2);
		src =  (0xd0000000 + ((y0+y)*480) + (x0*2));
		memcpy((unsigned int *)dest,(unsigned int *)src,(size_x*2));
	}

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_lcd_memmove_obj,5,5,pyb_lcd_memmove);

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

    for (n = 0; n < length_y; n++)
    {
        start_pos = (0xd0000000 + ((y0+n)*480 + x0*2));
        for (m = 0; m < length_x; m++)
        {
        	*(uint16_t *)(start_pos + m*2) = colour;
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
    *(uint16_t *)(0xd0000000 + (y*480 + x*2)) = colour;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_lcd_pixels_obj,4,4,pyb_lcd_pixels);

/// \method show()
///
/// Show the hidden buffer on the screen.
STATIC mp_obj_t pyb_lcd_thickpixels(mp_uint_t n_args, const mp_obj_t *args) {
    //pyb_lcd_obj_t *self = self_in;
    unsigned int x = mp_obj_get_int(args[1]);
    unsigned int y = mp_obj_get_int(args[2]);
    unsigned int colour = mp_obj_get_int(args[3]);
    *(uint16_t *)(0xd0000000 + (y*480 + x*2)) = colour;
    *(uint16_t *)(0xd0000000 + (y*480 + x*2) -2) = colour;
    *(uint16_t *)(0xd0000000 + (y*480 + x*2) +2) = colour;

    *(uint16_t *)(0xd0000000 + ((y-1)*480 + x*2)) = colour;
    *(uint16_t *)(0xd0000000 + ((y-1)*480 + x*2)+2) = colour;
    *(uint16_t *)(0xd0000000 + ((y-1)*480 + x*2)-2) = colour;

    *(uint16_t *)(0xd0000000 + ((y+1)*480 + x*2)) = colour;
    *(uint16_t *)(0xd0000000 + ((y+1)*480 + x*2)+2) = colour;
    *(uint16_t *)(0xd0000000 + ((y+1)*480 + x*2)-2) = colour;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_lcd_thickpixels_obj,4,4,pyb_lcd_thickpixels);


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
    { MP_ROM_QSTR(MP_QSTR_pixels), MP_ROM_PTR(&pyb_lcd_pixels_obj) },
    { MP_ROM_QSTR(MP_QSTR_thickpixels), MP_ROM_PTR(&pyb_lcd_thickpixels_obj) },
    { MP_ROM_QSTR(MP_QSTR_fillarea), MP_ROM_PTR(&pyb_lcd_fillarea_obj) },
    { MP_ROM_QSTR(MP_QSTR_memmove), MP_ROM_PTR(&pyb_lcd_memmove_obj) },
};

STATIC MP_DEFINE_CONST_DICT(pyb_lcd_locals_dict, pyb_lcd_locals_dict_table);

const mp_obj_type_t pyb_lcd_type = {
    { &mp_type_type },
    .name = MP_QSTR_LCD,
    .make_new = pyb_lcd_make_new,
    .locals_dict = (mp_obj_dict_t*)&pyb_lcd_locals_dict,
};



static void MX_GPIO_Init(void)
{
	  GPIO_InitTypeDef GPIO_Init_Structure;

	  /*##-1- Enable peripherals and GPIO Clocks #################################*/
	  /* Enable the LTDC Clock */
	  __HAL_RCC_LTDC_CLK_ENABLE();

	  /* Enable GPIOs clock */
	  __HAL_RCC_GPIOA_CLK_ENABLE();
	  __HAL_RCC_GPIOB_CLK_ENABLE();
	  __HAL_RCC_GPIOC_CLK_ENABLE();
	  __HAL_RCC_GPIOD_CLK_ENABLE();
	  __HAL_RCC_GPIOF_CLK_ENABLE();
	  __HAL_RCC_GPIOG_CLK_ENABLE();

	  /*##-2- Configure peripheral GPIO ##########################################*/
	  /******************** LTDC Pins configuration *************************/
	  /*
	   +------------------------+-----------------------+----------------------------+
	   +                       LCD pins assignment                                   +
	   +------------------------+-----------------------+----------------------------+
	   |  LCD_TFT R2 <-> PC.10  |  LCD_TFT G2 <-> PA.06 |  LCD_TFT B2 <-> PD.06      |
	   |  LCD_TFT R3 <-> PB.00  |  LCD_TFT G3 <-> PG.10 |  LCD_TFT B3 <-> PG.11      |
	   |  LCD_TFT R4 <-> PA.11  |  LCD_TFT G4 <-> PB.10 |  LCD_TFT B4 <-> PG.12      |
	   |  LCD_TFT R5 <-> PA.12  |  LCD_TFT G5 <-> PB.11 |  LCD_TFT B5 <-> PA.03      |
	   |  LCD_TFT R6 <-> PB.01  |  LCD_TFT G6 <-> PC.07 |  LCD_TFT B6 <-> PB.08      |
	   |  LCD_TFT R7 <-> PG.06  |  LCD_TFT G7 <-> PD.03 |  LCD_TFT B7 <-> PB.09      |
	   -------------------------------------------------------------------------------
	            |  LCD_TFT HSYNC <-> PC.06  | LCDTFT VSYNC <->  PA.04 |
	            |  LCD_TFT CLK   <-> PG.07  | LCD_TFT DE   <->  PF.10 |
	             -----------------------------------------------------

	  */

	  /* LTDC pins configuraiton: PA3 -- 12 */
	  GPIO_Init_Structure.Pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_6 |
	                                GPIO_PIN_11 | GPIO_PIN_12;
	  GPIO_Init_Structure.Mode = GPIO_MODE_AF_PP;
	  GPIO_Init_Structure.Pull = GPIO_NOPULL;
	  GPIO_Init_Structure.Speed = GPIO_SPEED_FAST;
	  GPIO_Init_Structure.Alternate= GPIO_AF14_LTDC;
	  HAL_GPIO_Init(GPIOA, &GPIO_Init_Structure);

	  /* LTDC pins configuraiton: PB8 -- 11 */
	  GPIO_Init_Structure.Pin = GPIO_PIN_8 | \
	                             GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11;
	  HAL_GPIO_Init(GPIOB, &GPIO_Init_Structure);

	  /* LTDC pins configuraiton: PC6 -- 10 */
	  GPIO_Init_Structure.Pin = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_10;
	  HAL_GPIO_Init(GPIOC, &GPIO_Init_Structure);

	  /* LTDC pins configuraiton: PD3 -- 6 */
	  GPIO_Init_Structure.Pin = GPIO_PIN_3 | GPIO_PIN_6;
	  HAL_GPIO_Init(GPIOD, &GPIO_Init_Structure);

	  /* LTDC pins configuraiton: PF10 */
	  GPIO_Init_Structure.Pin = GPIO_PIN_10;
	  HAL_GPIO_Init(GPIOF, &GPIO_Init_Structure);

	  /* LTDC pins configuraiton: PG6 -- 11 */
	  GPIO_Init_Structure.Pin = GPIO_PIN_6 | GPIO_PIN_7 | \
	                             GPIO_PIN_11;
	  HAL_GPIO_Init(GPIOG, &GPIO_Init_Structure);

	  /* LTDC pins configuraiton: PB0 -- 1 */
	  GPIO_Init_Structure.Pin = GPIO_PIN_0 | GPIO_PIN_1;
	  GPIO_Init_Structure.Alternate = GPIO_AF9_LTDC;
	  HAL_GPIO_Init(GPIOB, &GPIO_Init_Structure);

	  /* LTDC pins configuraiton: PG10 -- 12 */
	  GPIO_Init_Structure.Pin = GPIO_PIN_10 | GPIO_PIN_12;
	  HAL_GPIO_Init(GPIOG, &GPIO_Init_Structure);

}

static void MX_LTDC_Init(void)
{

}


void LCD_IO_WriteData(uint8_t RegValue)
{
  /* Set WRX to send data */
  LCD_WRX_HIGH();

  /* Reset LCD control line(/CS) and Send data */
  LCD_CS_LOW();
  //SPIx_Write(RegValue);
  HAL_SPI_Transmit(&SPIHandle5, &RegValue, 1, 1000);

  /* Deselect: Chip Select high */
  LCD_CS_HIGH();
}

/**
  * @brief  Writes register address.
  */
void LCD_IO_WriteReg(uint8_t Reg)
{
  /* Reset WRX to send command */
  LCD_WRX_LOW();

  /* Reset LCD control line(/CS) and Send command */
  LCD_CS_LOW();
  //SPIx_Write(Reg);
  HAL_SPI_Transmit(&SPIHandle5, &Reg, 1, 1000);

  /* Deselect: Chip Select high */
  LCD_CS_HIGH();
}


void ili9341_WriteReg(uint8_t LCD_Reg)
{
  LCD_IO_WriteReg(LCD_Reg);
}

/**
  * @brief  Writes data to the selected LCD register.
  * @param  LCD_Reg: address of the selected register.
  * @retval None
  */
void ili9341_WriteData(uint16_t RegValue)
{
  LCD_IO_WriteData(RegValue);
}

void LCD_IO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Configure NCS in Output Push-Pull mode */
  LCD_WRX_GPIO_CLK_ENABLE();
  GPIO_InitStructure.Pin     = LCD_WRX_PIN;
  GPIO_InitStructure.Mode    = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Pull    = GPIO_NOPULL;
  GPIO_InitStructure.Speed   = GPIO_SPEED_FAST;
  HAL_GPIO_Init(LCD_WRX_GPIO_PORT, &GPIO_InitStructure);

  LCD_RDX_GPIO_CLK_ENABLE();
  GPIO_InitStructure.Pin     = LCD_RDX_PIN;
  GPIO_InitStructure.Mode    = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Pull    = GPIO_NOPULL;
  GPIO_InitStructure.Speed   = GPIO_SPEED_FAST;
  HAL_GPIO_Init(LCD_RDX_GPIO_PORT, &GPIO_InitStructure);

  /* Configure the LCD Control pins ----------------------------------------*/
  LCD_NCS_GPIO_CLK_ENABLE();

  /* Configure NCS in Output Push-Pull mode */
  GPIO_InitStructure.Pin     = LCD_NCS_PIN;
  GPIO_InitStructure.Mode    = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Pull    = GPIO_NOPULL;
  GPIO_InitStructure.Speed   = GPIO_SPEED_FAST;
  HAL_GPIO_Init(LCD_NCS_GPIO_PORT, &GPIO_InitStructure);

  /* Set or Reset the control line */
  LCD_CS_LOW();
  LCD_CS_HIGH();
}



void ili9341_Init(void)
{
  /* Initialize ILI9341 low level bus layer ----------------------------------*/
  LCD_IO_Init();

  /* Configure LCD */
  ili9341_WriteReg(0xCA);
  ili9341_WriteData(0xC3);
  ili9341_WriteData(0x08);
  ili9341_WriteData(0x50);
  ili9341_WriteReg(LCD_POWERB);
  ili9341_WriteData(0x00);
  ili9341_WriteData(0xC1);
  ili9341_WriteData(0x30);
  ili9341_WriteReg(LCD_POWER_SEQ);
  ili9341_WriteData(0x64);
  ili9341_WriteData(0x03);
  ili9341_WriteData(0x12);
  ili9341_WriteData(0x81);
  ili9341_WriteReg(LCD_DTCA);
  ili9341_WriteData(0x85);
  ili9341_WriteData(0x00);
  ili9341_WriteData(0x78);
  ili9341_WriteReg(LCD_POWERA);
  ili9341_WriteData(0x39);
  ili9341_WriteData(0x2C);
  ili9341_WriteData(0x00);
  ili9341_WriteData(0x34);
  ili9341_WriteData(0x02);
  ili9341_WriteReg(LCD_PRC);
  ili9341_WriteData(0x20);
  ili9341_WriteReg(LCD_DTCB);
  ili9341_WriteData(0x00);
  ili9341_WriteData(0x00);
  ili9341_WriteReg(LCD_FRMCTR1);
  ili9341_WriteData(0x00);
  ili9341_WriteData(0x1B);
  ili9341_WriteReg(LCD_DFC);
  ili9341_WriteData(0x0A);
  ili9341_WriteData(0xA2);
  ili9341_WriteReg(LCD_POWER1);
  ili9341_WriteData(0x10);
  ili9341_WriteReg(LCD_POWER2);
  ili9341_WriteData(0x10);
  ili9341_WriteReg(LCD_VCOM1);
  ili9341_WriteData(0x45);
  ili9341_WriteData(0x15);
  ili9341_WriteReg(LCD_VCOM2);
  ili9341_WriteData(0x90);
  ili9341_WriteReg(LCD_MAC);
  ili9341_WriteData(0xC8);
  ili9341_WriteReg(LCD_3GAMMA_EN);
  ili9341_WriteData(0x00);
  ili9341_WriteReg(LCD_RGB_INTERFACE);
  ili9341_WriteData(0xC2);
  ili9341_WriteReg(LCD_DFC);
  ili9341_WriteData(0x0A);
  ili9341_WriteData(0xA7);
  ili9341_WriteData(0x27);
  ili9341_WriteData(0x04);

  /* Colomn address set */
  ili9341_WriteReg(LCD_COLUMN_ADDR);
  ili9341_WriteData(0x00);
  ili9341_WriteData(0x00);
  ili9341_WriteData(0x00);
  ili9341_WriteData(0xEF);
  /* Page address set */
  ili9341_WriteReg(LCD_PAGE_ADDR);
  ili9341_WriteData(0x00);
  ili9341_WriteData(0x00);
  ili9341_WriteData(0x01);
  ili9341_WriteData(0x3F);
  ili9341_WriteReg(LCD_INTERFACE);
  ili9341_WriteData(0x01);
  ili9341_WriteData(0x00);
  ili9341_WriteData(0x06);

  ili9341_WriteReg(LCD_GRAM);
  HAL_Delay(200);

  ili9341_WriteReg(LCD_GAMMA);
  ili9341_WriteData(0x01);

  ili9341_WriteReg(LCD_PGAMMA);
  ili9341_WriteData(0x0F);
  ili9341_WriteData(0x29);
  ili9341_WriteData(0x24);
  ili9341_WriteData(0x0C);
  ili9341_WriteData(0x0E);
  ili9341_WriteData(0x09);
  ili9341_WriteData(0x4E);
  ili9341_WriteData(0x78);
  ili9341_WriteData(0x3C);
  ili9341_WriteData(0x09);
  ili9341_WriteData(0x13);
  ili9341_WriteData(0x05);
  ili9341_WriteData(0x17);
  ili9341_WriteData(0x11);
  ili9341_WriteData(0x00);
  ili9341_WriteReg(LCD_NGAMMA);
  ili9341_WriteData(0x00);
  ili9341_WriteData(0x16);
  ili9341_WriteData(0x1B);
  ili9341_WriteData(0x04);
  ili9341_WriteData(0x11);
  ili9341_WriteData(0x07);
  ili9341_WriteData(0x31);
  ili9341_WriteData(0x33);
  ili9341_WriteData(0x42);
  ili9341_WriteData(0x05);
  ili9341_WriteData(0x0C);
  ili9341_WriteData(0x0A);
  ili9341_WriteData(0x28);
  ili9341_WriteData(0x2F);
  ili9341_WriteData(0x0F);

  ili9341_WriteReg(LCD_SLEEP_OUT);
  HAL_Delay(200);
  ili9341_WriteReg(LCD_DISPLAY_ON);
  /* GRAM start writing */
  ili9341_WriteReg(LCD_GRAM);
}


void SDRAM_Init(void)
{
	  hsdram.Instance = FMC_SDRAM_DEVICE;

	  /* Timing configuration for 90 MHz of SDRAM clock frequency (180MHz/2) */
	  /* TMRD: 2 Clock cycles */
	  SDRAM_Timing.LoadToActiveDelay    = 2;
	  /* TXSR: min=70ns (6x11.90ns) */
	  SDRAM_Timing.ExitSelfRefreshDelay = 7;
	  /* TRAS: min=42ns (4x11.90ns) max=120k (ns) */
	  SDRAM_Timing.SelfRefreshTime      = 4;
	  /* TRC:  min=63 (6x11.90ns) */
	  SDRAM_Timing.RowCycleDelay        = 7;
	  /* TWR:  2 Clock cycles */
	  SDRAM_Timing.WriteRecoveryTime    = 2;
	  /* TRP:  15ns => 2x11.90ns */
	  SDRAM_Timing.RPDelay              = 2;
	  /* TRCD: 15ns => 2x11.90ns */
	  SDRAM_Timing.RCDDelay             = 2;

	  hsdram.Init.SDBank             = FMC_SDRAM_BANK2;
	  hsdram.Init.ColumnBitsNumber   = FMC_SDRAM_COLUMN_BITS_NUM_8;
	  hsdram.Init.RowBitsNumber      = FMC_SDRAM_ROW_BITS_NUM_12;
	  hsdram.Init.MemoryDataWidth    = SDRAM_MEMORY_WIDTH;
	  hsdram.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
	  hsdram.Init.CASLatency         = FMC_SDRAM_CAS_LATENCY_3;
	  hsdram.Init.WriteProtection    = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
	  hsdram.Init.SDClockPeriod      = SDCLOCK_PERIOD;
	  hsdram.Init.ReadBurst          = FMC_SDRAM_RBURST_DISABLE;
	  hsdram.Init.ReadPipeDelay      = FMC_SDRAM_RPIPE_DELAY_1;

	  HAL_SDRAM_Init(&hsdram, &SDRAM_Timing);

	  SDRAM_Initialization_Sequence(&hsdram, &command);





}

static void SDRAM_Initialization_Sequence(SDRAM_HandleTypeDef *hsdram, FMC_SDRAM_CommandTypeDef *Command)
{
  __IO uint32_t tmpmrd =0;
  /* Step 3:  Configure a clock configuration enable command */
  Command->CommandMode 			 = FMC_SDRAM_CMD_CLK_ENABLE;
  Command->CommandTarget 		 = FMC_SDRAM_CMD_TARGET_BANK2;
  Command->AutoRefreshNumber 	 = 1;
  Command->ModeRegisterDefinition = 0;

  /* Send the command */
  HAL_SDRAM_SendCommand(hsdram, Command, 0x1000);

  /* Step 4: Insert 100 ms delay */
  HAL_Delay(100);

  /* Step 5: Configure a PALL (precharge all) command */
  Command->CommandMode 			 = FMC_SDRAM_CMD_PALL;
  Command->CommandTarget 	     = FMC_SDRAM_CMD_TARGET_BANK2;
  Command->AutoRefreshNumber 	 = 1;
  Command->ModeRegisterDefinition = 0;

  /* Send the command */
  HAL_SDRAM_SendCommand(hsdram, Command, 0x1000);

  /* Step 6 : Configure a Auto-Refresh command */
  Command->CommandMode 			 = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
  Command->CommandTarget 		 = FMC_SDRAM_CMD_TARGET_BANK2;
  Command->AutoRefreshNumber 	 = 4;
  Command->ModeRegisterDefinition = 0;

  /* Send the command */
  HAL_SDRAM_SendCommand(hsdram, Command, 0x1000);

  /* Step 7: Program the external memory mode register */
  tmpmrd = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_2          |
                     SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |
                     SDRAM_MODEREG_CAS_LATENCY_3           |
                     SDRAM_MODEREG_OPERATING_MODE_STANDARD |
                     SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;

  Command->CommandMode = FMC_SDRAM_CMD_LOAD_MODE;
  Command->CommandTarget 		 = FMC_SDRAM_CMD_TARGET_BANK2;
  Command->AutoRefreshNumber 	 = 1;
  Command->ModeRegisterDefinition = tmpmrd;

  /* Send the command */
  HAL_SDRAM_SendCommand(hsdram, Command, 0x1000);

  /* Step 8: Set the refresh rate counter */
  /* (15.62 us x Freq) - 20 */
  /* Set the device refresh counter */
  HAL_SDRAM_ProgramRefreshRate(hsdram, REFRESH_COUNT);
}


#endif // MICROPY_HW_HAS_LCD
