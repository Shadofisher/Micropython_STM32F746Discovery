/*
 * ts_init.h
 *
 *  Created on: 22 Jul 2017
 *      Author: graeme
 */

#ifndef TS_INIT_H_
#define TS_INIT_H_

void TS_IO_Write(uint8_t Addr, uint8_t Reg, uint8_t Value);
void TS_IO_Init(void);
void TS_IO_Delay(uint32_t Delay);
uint8_t TS_IO_Read(uint8_t Addr, uint8_t Reg);


void TS_Init(void);

/** @defgroup TS_Driver_structure  Touch Sensor Driver structure
  * @{
  */
typedef struct
{
  void       (*Init)(uint16_t);
  uint16_t   (*ReadID)(uint16_t);
  void       (*Reset)(uint16_t);
  void       (*Start)(uint16_t);
  uint8_t    (*DetectTouch)(uint16_t);
  void       (*GetXY)(uint16_t, uint16_t*, uint16_t*);
  void       (*EnableIT)(uint16_t);
  void       (*ClearIT)(uint16_t);
  uint8_t    (*GetITStatus)(uint16_t);
  void       (*DisableIT)(uint16_t);
}TS_DrvTypeDef;

/** @defgroup STM32746G_DISCOVERY_TS_Exported_Constants STM32746G_DISCOVERY_TS Exported Constants
  * @{
  */

typedef enum
{
  TS_OK                = 0x00, /*!< Touch Ok */
  TS_ERROR             = 0x01, /*!< Touch Error */
  TS_TIMEOUT           = 0x02, /*!< Touch Timeout */
  TS_DEVICE_NOT_FOUND  = 0x03  /*!< Touchscreen device not found */
}TS_StatusTypeDef;

/**
 *  @brief TS_GestureIdTypeDef
 *  Define Possible managed gesture identification values returned by touch screen
 *  driver.
 */
typedef enum
{
  GEST_ID_NO_GESTURE = 0x00, /*!< Gesture not defined / recognized */
  GEST_ID_MOVE_UP    = 0x01, /*!< Gesture Move Up */
  GEST_ID_MOVE_RIGHT = 0x02, /*!< Gesture Move Right */
  GEST_ID_MOVE_DOWN  = 0x03, /*!< Gesture Move Down */
  GEST_ID_MOVE_LEFT  = 0x04, /*!< Gesture Move Left */
  GEST_ID_ZOOM_IN    = 0x05, /*!< Gesture Zoom In */
  GEST_ID_ZOOM_OUT   = 0x06, /*!< Gesture Zoom Out */
  GEST_ID_NB_MAX     = 0x07  /*!< max number of gesture id */

} TS_GestureIdTypeDef;


/* User can use this section to tailor I2Cx/I2Cx instance used and associated
   resources */
/* Definition for AUDIO and LCD I2Cx resources */
#define DISCOVERY_AUDIO_I2Cx                             I2C3
#define DISCOVERY_AUDIO_I2Cx_CLK_ENABLE()                __HAL_RCC_I2C3_CLK_ENABLE()
#define DISCOVERY_AUDIO_DMAx_CLK_ENABLE()                __HAL_RCC_DMA1_CLK_ENABLE()
#define DISCOVERY_AUDIO_I2Cx_SCL_SDA_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOH_CLK_ENABLE()

#define DISCOVERY_AUDIO_I2Cx_FORCE_RESET()               __HAL_RCC_I2C3_FORCE_RESET()
#define DISCOVERY_AUDIO_I2Cx_RELEASE_RESET()             __HAL_RCC_I2C3_RELEASE_RESET()

/* Definition for I2Cx Pins */
#define DISCOVERY_AUDIO_I2Cx_SCL_PIN                     GPIO_PIN_7
#define DISCOVERY_AUDIO_I2Cx_SCL_SDA_GPIO_PORT           GPIOH
#define DISCOVERY_AUDIO_I2Cx_SCL_SDA_AF                  GPIO_AF4_I2C3
#define DISCOVERY_AUDIO_I2Cx_SDA_PIN                     GPIO_PIN_8

/* I2C interrupt requests */
#define DISCOVERY_AUDIO_I2Cx_EV_IRQn                     I2C3_EV_IRQn
#define DISCOVERY_AUDIO_I2Cx_ER_IRQn                     I2C3_ER_IRQn

/* Definition for external, camera and Arduino connector I2Cx resources */
#define DISCOVERY_EXT_I2Cx                               I2C1
#define DISCOVERY_EXT_I2Cx_CLK_ENABLE()                  __HAL_RCC_I2C1_CLK_ENABLE()
#define DISCOVERY_EXT_DMAx_CLK_ENABLE()                  __HAL_RCC_DMA1_CLK_ENABLE()
#define DISCOVERY_EXT_I2Cx_SCL_SDA_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()

#define DISCOVERY_EXT_I2Cx_FORCE_RESET()                 __HAL_RCC_I2C1_FORCE_RESET()
#define DISCOVERY_EXT_I2Cx_RELEASE_RESET()               __HAL_RCC_I2C1_RELEASE_RESET()

/* Definition for I2Cx Pins */
#define DISCOVERY_EXT_I2Cx_SCL_PIN                       GPIO_PIN_8
#define DISCOVERY_EXT_I2Cx_SCL_SDA_GPIO_PORT             GPIOB
#define DISCOVERY_EXT_I2Cx_SCL_SDA_AF                    GPIO_AF4_I2C1
#define DISCOVERY_EXT_I2Cx_SDA_PIN                       GPIO_PIN_9

/* I2C interrupt requests */
#define DISCOVERY_EXT_I2Cx_EV_IRQn                       I2C1_EV_IRQn
#define DISCOVERY_EXT_I2Cx_ER_IRQn                       I2C1_ER_IRQn
#define DISCOVERY_I2Cx_TIMING                      ((uint32_t)0x40912732)


#endif /* TS_INIT_H_ */
