/*
 * font24.h
 *
 *  Created on: 21 Jul 2017
 *      Author: graeme
 */

#ifndef FONT24_H_
#define FONT24_H_

/** @defgroup FONTS_Exported_Types
  * @{
  */
typedef struct _tFont
{
  const uint8_t *table;
  uint16_t Width;
  uint16_t Height;

} sFONT;

extern sFONT Font24;


#endif /* FONT24_H_ */
