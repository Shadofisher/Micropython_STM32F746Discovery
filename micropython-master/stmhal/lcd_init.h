/*
 * lcd_init.h
 *
 *  Created on: 21 Jul 2017
 *      Author: graeme
 */

#ifndef LCD_INIT_H_
#define LCD_INIT_H_

#define LTDC_ACTIVE_LAYER	     ((uint32_t)1) /* Layer 1 */

uint32_t BSP_LCD_GetYSize(void);
uint32_t BSP_LCD_GetXSize(void);

uint8_t BSP_LCD_Init(void);
void BSP_LCD_LayerDefaultInit(uint16_t LayerIndex, uint32_t FB_Address);
void Display_DemoDescription(void);

#define SDRAM_DEVICE_ADDR  ((uint32_t)0xC0000000)
#define LCD_FRAME_BUFFER	SDRAM_DEVICE_ADDR
#endif /* LCD_INIT_H_ */
