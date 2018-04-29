//
// Lucky Resistor's AS1130 Library
// ---------------------------------------------------------------------------
// (c)2017 by Lucky Resistor. See LICENSE for details.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>
//
#include "LRAS1130.h"

#ifdef ARDUINO_ARCH_AVR
// Make it compatible with the standart
#include <string.h>
namespace std { using ::memset; } 
namespace std { using ::memcpy; }
#else
//#include <cstring.h>
#endif


/// @mainpage
///
/// @section intro_sec Introduction
///
/// This library contains a class for a simple access to the AS1130 chip. 
/// The goal behind the library is to create a compact layer which will
/// generate simple and easy to understand code without adding too much
/// complexity and size to the final project.
///
/// @section requirements_sec Requirements
///
/// This library is writte for Arduino compatible chips. It requires a 
/// modern C++ compiler (C++11). The code also uses the "Wire" library 
/// from the Arduino project for the I2C communication.
///
/// @section classes_sec Classes
///
/// There is only the lr::AS1130 class. Read the documentation of this class
/// for all details.
///

/// The address for the register selection.
///
const uint8_t cRegisterSelectionAddress = 0xfd;

extern I2C_HandleTypeDef hi2c1;




bool AS1130_isChipConnected()
{
  /*uint8_t data[2] = {cRegisterSelectionAddress, RS_NOP};
  HAL_StatusTypeDef status;
  status = HAL_I2C_Master_Transmit(&hi2c1, hi2c1.Init.OwnAddress1, data, sizeof(data), 100);

  if (status = HAL_OK || status == HAL_BUSY)
	  return true;

  return false;*/
  return true;
}


void AS1130_setRamConfiguration(RamConfiguration ramConfiguration)
{
	AS1130_writeControlRegisterBits(CR_Config, CF_MemoryConfigMask, ramConfiguration);
}


void AS1130_setOnOffFrame(uint8_t frameIndex, uint8_t *picture, uint8_t pwmSetIndex)
{
  // Prepare all register bytes.
  const uint8_t registerDataSize = 0x18;
  //uint8_t registerData[registerDataSize];
  //AS1130Picture12x11_writeRegisters(registerData, picture.getData(), pwmSetIndex);
  // Write the bytes
  const uint8_t frameAddress = (RS_OnOffFrame + frameIndex);
  AS1130_writeToMemory(frameAddress, 0x00, picture, registerDataSize);
}


void AS1130_setOnOffFrameAllOff(uint8_t frameIndex, uint8_t pwmSetIndex)
{
  const uint8_t frameAddress = (RS_OnOffFrame + frameIndex);
  // Prepare all frame bytes.
  const uint8_t registerDataSize = 0x18;
  uint8_t registerData[registerDataSize];
  memset(registerData, 0, registerDataSize);
  // Write the first segment with the PWM set index.
  registerData[1] = (pwmSetIndex<<5);
  // Send to chip.
  AS1130_writeToMemory(frameAddress, 0x00, registerData, registerDataSize);
}


void AS1130_setOnOffFrameAllOn(uint8_t frameIndex, uint8_t pwmSetIndex)
{
  const uint8_t frameAddress = (RS_OnOffFrame + frameIndex);
  // Prepare all frame bytes.
  const uint8_t registerDataSize = 0x18;
  uint8_t registerData[registerDataSize];
  // Write the first segment with the PWM set index.
  registerData[0] = 0xff;
  registerData[1] = (pwmSetIndex<<5)|0x03;
  // Write all other segments 
  for (uint8_t i = 1; i < 12; ++i) {
    registerData[i*2] = 0xff;
    registerData[i*2+1] = 0x07;
  }
  // Send to chip.
  AS1130_writeToMemory(frameAddress, 0x00, registerData, registerDataSize);
}


void AS1130_setBlinkAndPwmSetAll(uint8_t setIndex, bool doesBlink, uint8_t pwmValue)
{
  const uint8_t setAddress = (RS_BlinkAndPwmSet + setIndex);
  if (doesBlink) {
	AS1130_fillMemory(setAddress, 0x00, 0xff, 24);
  } else {
	AS1130_fillMemory(setAddress, 0x00, 0x00, 24);
  }
  // Set all PWM values to the maximum.
  AS1130_fillMemory(setAddress, 0x18, pwmValue, 132);
}


void AS1130_setPwmValue(uint8_t setIndex, uint8_t ledIndex, uint8_t value)
{
  const uint8_t setAddress = (RS_BlinkAndPwmSet + setIndex);
  const uint8_t address = 0x18 + ((ledIndex>>4)*11) + (ledIndex&0xf);
  uint8_t * pValue = &value;
  AS1130_writeToMemory(setAddress, address, pValue, sizeof(value));
}


void AS1130_setDotCorrection(uint8_t *data)
{
  for (uint8_t i = 0; i < 12; ++i) {
	  AS1130_writeToMemory(RS_DotCorrection, i, (data) + i, sizeof(data[i]));
  }
}


void AS1130_setInterruptMask(uint8_t mask)
{
	AS1130_writeControlRegister(CR_InterruptMask, mask);
}


void AS1130_setInterruptFrame(uint8_t lastFrame)
{
	AS1130_writeControlRegister(CR_InterruptFrameDefinition, lastFrame);
}


void AS1130_setInterfaceMonitoring(uint8_t timeout, bool enabled)
{
  uint8_t data = 0;
  if (enabled) {
    data = 1;
  }
  data |= ((timeout & 0x3f) << 1);
  AS1130_writeControlRegister(CR_InterfaceMonitoring, data);
}


void AS1130_setClockSynchronization(Synchronization synchronization, ClockFrequency clockFrequency)
{
	AS1130_writeControlRegister(CR_ClockSynchronization, synchronization|clockFrequency);
}


void AS1130_setCurrentSource(Current current)
{
	AS1130_writeControlRegister(CR_CurrentSource, current);
}


void AS1130_setScanLimit(ScanLimit scanLimit)
{
	AS1130_writeControlRegisterBits(CR_DisplayOption, DOF_ScanLimitMask, scanLimit);
}


void AS1130_setBlinkEnabled(bool enabled)
{
	AS1130_setOrClearControlRegisterBits(CR_MovieMode, MMF_BlinkEnabled, !enabled);
}


void AS1130_startPicture(uint8_t frameIndex, bool blinkAll)
{
  uint8_t data = PF_DisplayPicture;
  data |= (frameIndex & PF_PictureAddressMask);
  if (blinkAll) {
    data |= PF_BlinkPicture;
  }
  AS1130_writeControlRegister(CR_Picture, data);
}


void AS1130_stopPicture()
{
	AS1130_writeControlRegister(CR_Picture, 0x00);
}


void AS1130_setMovieEndFrame(MovieEndFrame movieEndFrame)
{
	AS1130_setOrClearControlRegisterBits(CR_MovieMode, MMF_EndLast, movieEndFrame == MovieEndWithLastFrame);
}


void AS1130_setMovieFrameCount(uint8_t count)
{
	AS1130_writeControlRegisterBits(CR_MovieMode, MMF_MovieFramesMask, count-1);
}


void AS1130_setFrameDelayMs(uint16_t delayMs)
{
  delayMs *= 10;
  delayMs /= 325;
  if (delayMs > 0x000f) {
    delayMs = 0x000f;
  }
  AS1130_writeControlRegisterBits(CR_FrameTimeScroll, FTSF_FrameDelay, (uint8_t)(delayMs));
}


void AS1130_setScrollingEnabled(bool enable)
{
	AS1130_setOrClearControlRegisterBits(CR_FrameTimeScroll, FTSF_EnableScrolling, enable);
}


void AS1130_setScrollingBlockSize(ScrollingBlockSize scrollingBlockSize)
{
	AS1130_setOrClearControlRegisterBits(CR_FrameTimeScroll, FTSF_BlockSize, scrollingBlockSize == ScrollIn5LedBlocks);
}


void AS1130_setScrollingDirection(ScrollingDirection scrollingDirection)
{
	AS1130_setOrClearControlRegisterBits(CR_FrameTimeScroll, FTSF_ScrollDirection, scrollingDirection == ScrollingLeft);
}


void AS1130_setFrameFadingEnabled(bool enable)
{
	AS1130_setOrClearControlRegisterBits(CR_FrameTimeScroll, FTSF_FrameFade, enable);
}


void AS1130_setBlinkFrequency(BlinkFrequency blinkFrequency)
{
	AS1130_setOrClearControlRegisterBits(CR_DisplayOption, DOF_BlinkFrequency, blinkFrequency == BlinkFrequency3s);
}


void AS1130_setMovieLoopCount(MovieLoopCount movieLoopCount)
{
	AS1130_writeControlRegisterBits(CR_DisplayOption, DOF_LoopsMask, movieLoopCount);
}


void AS1130_startMovie(uint8_t firstFrameIndex, bool blinkAll)
{
  uint8_t data = MF_DisplayMovie;
  data |= (firstFrameIndex & MF_MovieAddressMask);
  if (blinkAll) {
    data |= MF_BlinkMovie;
  }
  AS1130_writeControlRegister(CR_Movie, data);
}


void AS1130_stopMovie()
{
	AS1130_writeControlRegister(CR_Movie, 0x00);
}


void AS1130_setLowVddResetEnabled(bool enabled)
{
	AS1130_setOrClearControlRegisterBits(CR_Config, CF_LowVddReset, enabled);
}


void AS1130_setLowVddStatusEnabled(bool enabled)
{
	AS1130_setOrClearControlRegisterBits(CR_Config, CF_LowVddStatus, enabled);
}


void AS1130_setLedErrorCorrectionEnabled(bool enabled)
{
	AS1130_setOrClearControlRegisterBits(CR_Config, CF_LedErrorCorrection, enabled);
}


void AS1130_setDotCorrectionEnabled(bool enabled)
{
	AS1130_setOrClearControlRegisterBits(CR_Config, CF_DotCorrection, enabled);
}


void AS1130_setTestAllLedsEnabled(bool enabled)
{
	AS1130_setOrClearControlRegisterBits(CR_ShutdownAndOpenShort, SOSF_TestAll, enabled);
}


void AS1130_setAutomaticTestEnabled(bool enabled)
{
	AS1130_setOrClearControlRegisterBits(CR_ShutdownAndOpenShort, SOSF_AutoTest, enabled);
}


void AS1130_startChip()
{
	AS1130_setControlRegisterBits(CR_ShutdownAndOpenShort, SOSF_Shutdown);
}


void AS1130_stopChip()
{
	AS1130_clearControlRegisterBits(CR_ShutdownAndOpenShort, SOSF_Shutdown);
}


void AS1130_resetChip()
{
	AS1130_clearControlRegisterBits(CR_ShutdownAndOpenShort, SOSF_Initialize);
    HAL_Delay(100);
}


void AS1130_runManualTest()
{
	AS1130_setControlRegisterBits(CR_ShutdownAndOpenShort, SOSF_ManualTest);
  while (AS1130_isLedTestRunning()) {
    HAL_Delay(10);
  }
  AS1130_clearControlRegisterBits(CR_ShutdownAndOpenShort, SOSF_ManualTest);
}


LedStatus AS1130_getLedStatus(uint8_t ledIndex)
{
  if (ledIndex > 0xba) {
    return LedStatusDisabled;
  }
  if ((ledIndex & 0x0f) > 0xa) {
    return LedStatusDisabled;
  }
  const uint8_t ledBitMask = (1<<(ledIndex&0x7));
  const uint8_t registerIndex = CR_OpenLedBase + (ledIndex>>3);
  const uint8_t mask = AS1130_readFromMemory(RS_Control, registerIndex);
  if ((mask & ledBitMask) == 0) {
    return LedStatusOpen;
  } else {
    return LedStatusOk;
  }
}


bool AS1130_isLedTestRunning()
{
  const uint8_t data = AS1130_readControlRegister(CR_Status);
  return (data & SF_TestOn) != 0;
}


bool AS1130_isMovieRunning()
{
  const uint8_t data = AS1130_readControlRegister(CR_Status);
  return (data & SF_MovieOn) != 0;
}


uint8_t AS1130_getDisplayedFrame()
{
  const uint8_t data = AS1130_readControlRegister(CR_Status);
  return (data>>2);
}


uint8_t AS1130_getInterruptStatus()
{
  return AS1130_readControlRegister(CR_InterruptStatus);
}


void AS1130_writeToChip(uint8_t address, uint8_t data)
{
  uint8_t sendData[2] = { address, data};
  HAL_I2C_Master_Transmit(&hi2c1, hi2c1.Init.OwnAddress1, sendData, 2, 100);
}

void AS1130_pWriteToChip(uint8_t *pdata, uint8_t size)
{

  HAL_I2C_Master_Transmit(&hi2c1, hi2c1.Init.OwnAddress1, pdata, size, 100);
}


void AS1130_writeToMemory(uint8_t registerSelection, uint8_t address, uint8_t *data, uint8_t size)
{
  AS1130_writeToChip(cRegisterSelectionAddress, registerSelection);
  HAL_I2C_Master_Transmit(&hi2c1, address, data, size, 100);
}


void AS1130_fillMemory(uint8_t registerSelection, uint8_t address, uint8_t value, uint8_t size)
{

  uint8_t *pdata = malloc(size + 1);
  if (pdata == NULL)
    return;

  memset(pdata + 1, value, size);
  memcpy(pdata, &address, 1);

  AS1130_writeToChip(cRegisterSelectionAddress, registerSelection);
  AS1130_pWriteToChip(pdata, size);

  free(pdata);
}


uint8_t AS1130_readFromMemory(uint8_t registerSelection, uint8_t address)
{
  uint8_t retVal = 0;
  AS1130_writeToChip(cRegisterSelectionAddress, registerSelection);
  HAL_I2C_Master_Receive(&hi2c1, hi2c1.Init.OwnAddress1, &retVal, sizeof(retVal), 100);
  return retVal;
}


void AS1130_writeControlRegister(ControlRegister controlRegister, uint8_t data)
{
	AS1130_writeToMemory(RS_Control, controlRegister, &data, sizeof(data));
}


uint8_t AS1130_readControlRegister(ControlRegister controlRegister)
{
  return AS1130_readFromMemory(RS_Control, controlRegister);
}


void AS1130_writeControlRegisterBits(ControlRegister controlRegister, uint8_t mask, uint8_t data)
{
  uint8_t registerData = AS1130_readControlRegister(controlRegister);
  registerData &= (~mask);
  registerData |= (data & mask);
  AS1130_writeControlRegister(controlRegister, registerData);
}


void AS1130_setControlRegisterBits(ControlRegister controlRegister, uint8_t mask)
{
	AS1130_writeControlRegisterBits(controlRegister, mask, mask);
}


void AS1130_clearControlRegisterBits(ControlRegister controlRegister, uint8_t mask)
{
	AS1130_writeControlRegisterBits(controlRegister, mask, 0);
}


void AS1130_setOrClearControlRegisterBits(ControlRegister controlRegister, uint8_t mask, bool setBits)
{
  if (setBits) {
	  AS1130_writeControlRegisterBits(controlRegister, mask, mask);
  } else {
	  AS1130_writeControlRegisterBits(controlRegister, mask, 0);
  }
}


