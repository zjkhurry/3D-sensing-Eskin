/*
 * MGC3130.h
 *
 *  Created on: 2022��6��6��
 *      Author: jingkzhou3
 */

#ifndef MGC3130_H_
#define MGC3130_H_

#include <stdbool.h>
#include <stdint.h>
//open this macro and you can see the details of the program
// #define ENABLE_DBG

#define MGC3130_IIC_ADDR 0x42
  /**
   * @struct sInfo_t
   * @brief Used as information storage inside the library
   */
  typedef struct {
    uint32_t gestureInfo;        /**< Gestures information */
    uint32_t touchInfo;          /**< Touch information */
    uint32_t airWheelInfo;       /**< airWheel information */
    uint16_t xPosition;          /**< X-axis position information */
    uint16_t yPosition;          /**< Y-axis position information */
    uint16_t zPosition;          /**< Z-axis position information */
  }__attribute__ ((packed)) sInfo_t;


  typedef struct {
    float CIC_e;
    float CIC_w;
    float CIC_n;
    float CIC_s;
    float CIC_c;
    float SD_e;
    float SD_w;
    float SD_n;
    float SD_s;
    float SD_c;
  }__attribute__ ((packed)) rawdata_t;

  /**
   * @enum eErrorCode_t
   * @brief The state of information interaction in communication
   */
  typedef enum{
    eNoError = 0x0000,             /**< OK */
    eUnknownCommand = 0x0001,      /**< Message ID is unknown */
    eWrongParameterValue = 0x0014, /**< The value of the Argument/Parameter of a RuntimeParameter command is out of the valid range */
    eUnknownParameterID = 0x0015,  /**< The MessageID or RuntimeParameterID is unknown or out of the valid range */
    eWakeupHappend = 0x001A,       /**< A wake-up by Host was detected */
  }eErrorCode_t;

  /**
   * @enum eGestureInfo_t
   * @brief Gesture Info
   */
  typedef enum{
    eNoGesture = 0,           /**< No gesture */
    eGarbageModel,            /**< Garbage model */
    eFilckR,                  /**< Flick Left to Right */
    eFilckL,                  /**< Flick Right to Left */
    eFilckU,                  /**< Flick Down to Up */
    eFilckD,                  /**< Flick Up to Down */
    eCircleClockwise,         /**< Circle clockwise (only active if AirWheel disabled) */
    eCircleCounterclockwise,  /**< Circle counterclockwise (only active if AirWheel disabled) */
  }eGestureInfo_t;

  /**
   * @enum eTouchInfo_t
   * @brief Touch Info
   */
  typedef enum{
    eTouchDown         = 1,           /**< Touch Down electrode */
    eTouchLeft         = 2,           /**< Touch Left electrode */
    eTouchUp           = 4,           /**< Touch Up electrode */
    eTouchRight        = 8,           /**< Touch Right electrode */
    eTouchCenter       = 16,          /**< Touch Center electrode */
    eTapDown           = 32,          /**< Tap Down electrode*/
    eTapLeft           = 64,          /**< Tap Left electrode */
    eTapUp             = 128,         /**< Tap Up electrode */
    eTapRight          = 256,         /**< Tap Right electrode */
    eTapCenter         = 512,         /**< Tap Center electrode */
    eDoubleTapDown     = 1024,        /**< Double Tap Down electrode*/
    eDoubleTapLeft     = 2048,        /**< Double Tap Left electrode */
    eDoubleTapUp       = 4096,        /**< Double Tap Up electrode */
    eDoubleTapRight    = 8192,        /**< Double Tap Right electrode */
    eDoubleTapCenter   = 16384,       /**< Double Tap Center electrode */
  }eTouchInfo_t;

  /**
   * @enum Electrodes
   * @brief elecrtodes
   */
  typedef enum{
    south= 0,
    north,
    west,
    east,
    central,
  }elecrtodes;


  /**
   * @fn begin
   * @brief initialization function
   * @return bool,returns the initialization status
   * @retval true Initialization succeeded
   * @retval fasle Initialization  failed
   */
  bool begin(void);

  /**
   * @fn reset
   * @brief reset the sensor
   */
  void reset(void);

  /**
   * @fn sensorDataRecv
   * @brief get the sensor data
   */
  void sensorDataRecv(void);

  /**
   * @fn enableGestures
   * @brief enable gesture recognition function
   * @return Result of enabling gesture recognition
   * @retval -1  setup fails
   * @retval  0  setup succeeds
   */
  int8_t enableGestures(void);

  /**
   * @fn disableGestures
   * @brief turn off gesture recognition function
   * @return Result of disabling gesture recognition
   * @retval -1  setup fails
   * @retval  0  setup succeeds
   */
  int8_t disableGestures(void);

  /**
   * @fn disableAirWheel
   * @brief turn off AirWheel function
   * @return Result of disabling AirWheel function
   * @retval -1  setup fails
   * @retval  0  setup succeeds
   */
  int8_t disableAirWheel(void);

  /**
   * @fn enableAirWheel
   * @brief enable AirWheel function
   * @return Result of enabling AirWheel function
   * @retval -1  setup fails
   * @retval  0  setup succeeds
   */
  int8_t enableAirWheel(void);

  /**
   * @fn disableApproachDetection
   * @brief turn off proximity detection function
   * @return Result of disabling proximity detection function
   * @retval -1  setup fails
   * @retval  0  setup succeeds
   */
  int8_t disableApproachDetection(void);

  /**
   * @fn enableApproachDetection
   * @brief enable proximity detection function
   * @return Result of enabling proximity detection function
   * @retval -1  setup fails
   * @retval  0  setup succeeds
   */
  int8_t enableApproachDetection(void);

  /**
   * @fn disableTouchDetection
   * @brief turn off touch detection function
   * @return Result of disabling touch detection function
   * @retval -1  setup fails
   * @retval  0  setup succeeds
   */
  int8_t disableTouchDetection(void);

  /**
   * @fn enableTouchDetection
   * @brief enable touch detection function
   * @return Result of enabling touch detection function
   * @retval -1  setup fails
   * @retval  0  setup succeeds
   */
  int8_t enableTouchDetection(void);

  int8_t enableRawData();

  int8_t lockRawData();

  /**
   * @fn getPositionX
   * @brief get the X-axis position
   * @return X-axis position
   */
  uint16_t getPositionX(void);

  /**
   * @fn getPositionY
   * @brief get the Y-axis position
   * @return Y-axis position
   */
  uint16_t getPositionY(void);

  /**
   * @fn getPositionZ
   * @brief get the Z-axis position
   * @return Z-axis position
   */
  uint16_t getPositionZ(void);

  /**
   * @fn getTouchInfo
   * @brief get touch information
   * @return touch information
   * @retval eDoubleTapCenter  Double Tap Center electrode
   * @retval eDoubleTapRight   Double Tap Right electrode
   * @retval eDoubleTapUp      Double Tap Up electrode
   * @retval eDoubleTapLeft    Double Tap Left electrode
   * @retval eDoubleTapDown    Double Tap Down electrode
   * @retval eTapCenter        Tap Center electrode
   * @retval eTapRight         Tap Right electrode
   * @retval eTapUp            Tap Up electrode
   * @retval eTapLeft          Tap Left electrode
   * @retval eTapDown          Tap Down electrode
   * @retval eTouchCenter      Touch Center electrode
   * @retval eTouchRight       Touch Right electrode
   * @retval eTouchUp          Touch Up electrode
   * @retval eTouchLeft        Touch Left electrode
   * @retval eTouchDown        Touch Down electrode
   */
  uint16_t getTouchInfo(void);

  /**
   * @fn getGestureInfo
   * @brief get gesture information
   * @return gesture information
   * @retval eFilckR                   Flick Left to Right
   * @retval eFilckL                   Flick Right to Left
   * @retval eFilckU                   Flick Down to Up
   * @retval eFilckD                   Flick Up to Down
   * @retval eCircleClockwise          Circle clockwise (only active if AirWheel disabled)
   * @retval eCircleCounterclockwise   Circle counterclockwise (only active if AirWheel disabled)
   */
  uint8_t getGestureInfo(void);

  /**
   * @fn havePositionInfo
   * @brief monitor position information
   * @return Results of monitoring position information
   * @retval true  position information exists
   * @retval false There is no position information
   */
  bool havePositionInfo(void);

  bool haveRawData();

  /**
   * @fn lockDataOutput
   * @brief lock the output data format of the sensor
   * @return Result of setting
   * @retval -1  setup fails
   * @retval  0  setup succeeds
   */
  int8_t lockDataOutput(void);

  /**
   * @fn enableDataOutput
   * @brief set the output data format of the sensor
   * @return Result of setting
   * @retval -1  setup fails
   * @retval  0  setup succeeds
   */
  int8_t enableDataOutput(void);

  /**
   * @fn tsInput
   * @brief set the ts_pin of the master computer to input mode(TS:transfer status line)
   */
  void tsInput(void);

  /**
   * @fn tsOutput
   * @brief set the ts_pin of the master computer to output mode(TS:transfer status line)
   */
  void tsOutput(void);

  /**
   * @fn tsWrite
   * @brief set the ts_pin of the master computer to output status(TS:transfer status line)
   * @param mode output status HIGH/LOW
   * @n HIGH  Move the Transfer Status line higher
   * @n LOW   Lower the Transfer Status line
   */
  void tsWrite(uint8_t mode);

  /**
   * @fn tsRead
   * @brief get the status of transfer status line
   * @return  return the status of transfer status line
   * @retval HIGH
   * @retval LOW
   */
  uint8_t tsRead(void);

  /**
   * @fn read
   * @brief get the sensor IIC data
   * @param pBuf write the store and buffer of the data
   * @param size data length to be written
   * @return return the actually written length, returning 0 means reading data fails
   */
  uint8_t read(uint8_t* pBuf, uint8_t size);

  /**
   * @fn setRuntimeparameter
   * @brief write the sensor IIC data
   * @param pBuf write the store and buffer of the data
   * @param size data length to be written
   * @return return the actually written length
   */
  uint8_t setRuntimeparameter(uint8_t* pBuf,uint8_t size);

  void delay_ms(unsigned long num_ms);

  rawdata_t rawDataRecv();

  int8_t enableCaliMode();

  int8_t disableCaliMode();

  uint8_t getSignalMatching(uint8_t ele);

  uint8_t getElectrodeMapping(uint8_t ele);

  uint8_t getRuntimeparameter(uint8_t* pBuf,uint8_t size);

  int8_t setSignalMatching(uint8_t ele,uint8_t level);

  int8_t setElectrodeMapping(uint8_t ele,uint8_t rx);

  uint8_t autoSiganlMatching();

  int8_t makePersistentAFE();

  uint32_t getAirWheel();

  int8_t forceRecalibration();

  int8_t deepSleep();

#endif /* MGC3130_H_ */
