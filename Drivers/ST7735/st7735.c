/*
This is the core graphics library for all our displays, providing a common
set of graphics primitives (points, lines, circles, etc.).  It needs to be
paired with a hardware-specific library for each display device we carry
(to handle the lower-level functions).

Adafruit invests time and resources providing this open source code, please
support Adafruit & open-source hardware by purchasing products from Adafruit!

Copyright (c) 2013 Adafruit Industries.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include <stm32f10x.h>

#include <systicktimer.h>
#if USE_FSMC
 #include <fsmcdrv.h>
#else
 #include <spi.h>
#endif

#include "st7735.h"

uint8_t tabcolor, colstart, rowstart;

int16_t _width  = ST7735_TFTWIDTH;
int16_t _height = ST7735_TFTHEIGHT_18;

//-------------------------------------------------------------------------------------------//

void writeCommand(uint8_t c)
{
  WRITE_CMD(c);
}

void writeData(uint8_t c)
{
  WRITE_DATA(c);
}

void writeWordData(uint16_t c)
{
  SEND_DATA16(c);
}

void commandList(const uint8_t *addr)
{
  uint16_t ms;
  uint8_t numArgs;
  uint8_t numCommands = *addr++; // Number of commands to follow
  
  while(numCommands--) {        // For each command...
    writeCommand(*addr++);      // Read, issue command
    numArgs  = *addr++;         // Number of args to follow
    ms       = numArgs & DELAY; // If hibit set, delay follows args
    numArgs &= ~DELAY;          // Mask out delay bit
    while(numArgs--) {          // For each argument...
      writeData(*addr++);       // Read, issue argument
    }

    if(ms) {
      ms = *addr++;            // Read post-command delay time (ms)
      if(ms == 255) ms = 500;  // If 255, delay for 500 ms
      _delayMS(ms);
    }
  }
}

void initTFT_GPIO(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;     // Mode: output "Push-Pull"
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;    // Set speed
#if USE_FSMC
  GPIO_InitStruct.GPIO_Pin = TFT_RES_PIN;
#else
  GPIO_InitStruct.GPIO_Pin = TFT_DC_PIN | TFT_RES_PIN | TFT_SS_PIN;
#endif
  GPIO_Init(GPIOB, &GPIO_InitStruct);        // Aply settings to port B
}

void hardRstTFT(void)
{
  // toggle RST low to reset
  SET_TFT_RES_HI;  _delayMS(500);
  SET_TFT_RES_LOW; _delayMS(500);
  SET_TFT_RES_HI;  _delayMS(500);
}

void initLCD(void)
{
  colstart  = rowstart = 0;
  
#if USE_FSMC
  initFSMC();
#else
  init_SPI1();
  init_DMA1_SPI1();
#endif
  initTFT_GPIO();
  
#if TFT_CS_ALWAS_ACTIVE
  GRAB_TFT_CS;
#endif
  
  SET_TFT_CS_HI;
  SET_TFT_DC_HI;
  
  hardRstTFT();
  initR(INITR_BLACKTAB);
}

// Initialization code common to both 'B' and 'R' type displays
void commonInit(const uint8_t *cmdList)
{
  if(cmdList) commandList(cmdList);
}

// Initialization for ST7735B screens
void initB(void)
{
  commonInit(Bcmd);
}

// Initialization for ST7735R screens (green or red tabs)
void initR(uint8_t options)
{
  commonInit(Rcmd1);
  if(options == INITR_GREENTAB) {
    commandList(Rcmd2green);
    colstart = 2;
    rowstart = 1;
  } else if(options == INITR_144GREENTAB) {
    //_height = ST7735_TFTHEIGHT_144;
    commandList(Rcmd2green144);
    colstart = 2;
    rowstart = 3;
  } else {
    // colstart, rowstart left at default '0' values
    commandList(Rcmd2red);
  }
  commandList(Rcmd3);

  // if black, change MADCTL color filter
  if (options == INITR_BLACKTAB) {
    writeCommand(ST7735_MADCTL);
    writeData(0xC0);
  }

  tabcolor = options;
}

// blow your mind
void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{    
  WAIT_DMA_BSY;              // wait untill DMA transfer end
  
  SET_CMD();                 // writecommand:
  SEND_DATA(ST7735_CASET);   // Column addr set
  
  SET_DATA();                // writeData:
  SEND_2_DATA(x0, x1);       // XSTART, XEND
  
  SET_CMD();                 // writecommand:
  SEND_DATA(ST7735_RASET);   // Row addr set
  
  SET_DATA();                // writeData:
  SEND_2_DATA(y0, y1);       // YSTART, YEND
  
  SET_CMD();                 // writecommand:
  SEND_DATA(ST7735_RAMWR);   // write to RAM
  
  SET_DATA();                // ready accept data 
}

void setVAddrWindow(uint16_t x0, uint16_t y0, uint16_t y1)
{
  WAIT_DMA_BSY;              // wait untill DMA transfer end
  
  SET_CMD();                 // writecommand:
  SEND_DATA(ST7735_CASET);   // Column addr set
  
  SET_DATA();                // writeData:
  SEND_2_DATA(x0, x0);       // XSTART, XEND
  
  SET_CMD();                 // writecommand:
  SEND_DATA(ST7735_RASET);   // Row addr set
  
  SET_DATA();                // writeData:
  SEND_2_DATA(y0, y1);       // YSTART, YEND
  
  SET_CMD();                 // writecommand:
  SEND_DATA(ST7735_RAMWR);   // write to RAM
  
  SET_DATA();                // ready accept data
}

void setHAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1)
{
  WAIT_DMA_BSY;              // wait untill DMA transfer end
  
  SET_CMD();                 // writecommand:
  SEND_DATA(ST7735_CASET);   // Column addr set
  
  SET_DATA();                // writeData:
  SEND_2_DATA(x0, x1);       // XSTART, XEND
  
  SET_CMD();                 // writecommand:
  SEND_DATA(ST7735_RASET);   // Row addr set
  
  SET_DATA();                // writeData:
  SEND_2_DATA(y0, y0);       // YSTART, YEND
  
  SET_CMD();                 // writecommand:
  SEND_DATA(ST7735_RAMWR);   // write to RAM
  
  SET_DATA();                // ready accept data
}

void setAddrPixel(uint16_t x0, uint16_t y0)
{ 
  WAIT_DMA_BSY;              // wait untill DMA transfer end
  
  SET_CMD();                 // writecommand:
  SEND_DATA(ST7735_CASET);   // Column addr set
  
  SET_DATA();                // writeData:
  SEND_2_DATA(x0, x0);       // XSTART, XEND
  
  SET_CMD();                 // writecommand:
  SEND_DATA(ST7735_RASET);   // Row addr set
  
  SET_DATA();                // writeData:
  SEND_2_DATA(y0, y0);       // YSTART, YEND
  
  SET_CMD();                 // writecommand:
  SEND_DATA(ST7735_RAMWR);   // write to RAM
  
  SET_DATA();                // ready accept data
}

void setRotation(uint8_t m)
{
  uint8_t rotation = m % 4; // can't be higher than 3
  uint8_t madctlParam = 0;
  
  switch (rotation)
  {
  case 0: {
    if (tabcolor == INITR_BLACKTAB) {
      madctlParam = (MADCTL_MX | MADCTL_MY | MADCTL_RGB);
    } else {
      madctlParam = (MADCTL_MX | MADCTL_MY | MADCTL_BGR);
    }
    _width  = ST7735_TFTWIDTH;
    
    if (tabcolor == INITR_144GREENTAB) 
      _height = ST7735_TFTHEIGHT_144;
    else
      _height = ST7735_TFTHEIGHT_18;
    
  } break;
  
  case 1: {
    if (tabcolor == INITR_BLACKTAB) {
      madctlParam = (MADCTL_MY | MADCTL_MV | MADCTL_RGB);
    } else {
      madctlParam = (MADCTL_MY | MADCTL_MV | MADCTL_BGR);
    }
    
    if (tabcolor == INITR_144GREENTAB) 
      _width = ST7735_TFTHEIGHT_144;
    else
      _width = ST7735_TFTHEIGHT_18;
    
    _height = ST7735_TFTWIDTH;
  } break;
  
  case 2: {
    if (tabcolor == INITR_BLACKTAB) {
      madctlParam = (MADCTL_RGB);
    } else {
      madctlParam = (MADCTL_BGR);
    }
    _width  = ST7735_TFTWIDTH;
    if (tabcolor == INITR_144GREENTAB) 
      _height = ST7735_TFTHEIGHT_144;
    else
      _height = ST7735_TFTHEIGHT_18;
    
  } break;
  
  case 3: {
    if (tabcolor == INITR_BLACKTAB) {
      madctlParam = (MADCTL_MX | MADCTL_MV | MADCTL_RGB);
    } else {
      madctlParam = (MADCTL_MX | MADCTL_MV | MADCTL_BGR);
    }
    if (tabcolor == INITR_144GREENTAB) 
      _width = ST7735_TFTHEIGHT_144;
    else
      _width = ST7735_TFTHEIGHT_18;
    
    _height = ST7735_TFTWIDTH;
  } break;
  }
  
  writeCommand(ST7735_MADCTL);
  writeData(madctlParam);
}

// how much to scroll
void scrollAddress(uint16_t VSP)
{
  //writeCommand(ST7735_VSCRSADD); // Vertical scrolling start address
  writeWordData(VSP);
}

// set scrollong zone
void setScrollArea(uint16_t TFA, uint16_t BFA)
{
  //writeCommand(ST7735_VSCRDEF); // Vertical scroll definition
  writeWordData(TFA);
  //writeWordData(ST7735_TFTHEIGHT - TFA - BFA);
  writeWordData(BFA);
}

uint16_t scroll(uint16_t lines, uint16_t yStart)
{
  for (uint16_t i = 0; i < lines; i++) {
    if ((++yStart) == (_height - TFT_BOT_FIXED_AREA)) yStart = TFT_TOP_FIXED_AREA;
    scrollAddress(yStart);
  }
  return  yStart;
}

uint16_t scrollSmooth(uint16_t lines, uint16_t yStart, uint8_t wait)
{
  for (uint16_t i = 0; i < lines; i++) {
    if ((++yStart) == (_height - TFT_BOT_FIXED_AREA)) yStart = TFT_TOP_FIXED_AREA;
    scrollAddress(yStart);
    _delayMS(wait);
  }
  return  yStart;
}

void setSleep(bool enable)
{
  if (enable) {
    writeCommand(ST7735_SLPIN);
    writeCommand(ST7735_DISPOFF);
  } else {
    writeCommand(ST7735_SLPOUT);
    writeCommand(ST7735_DISPON);
    _delayMS(5);
  }
}

void setIdleMode(bool mode)
{
  if (mode) {
    //writeCommand(ST7735_IDLEON);
  } else {
    //writeCommand(ST7735_IDLEOFF);
  }
}

void setDispBrightness(uint8_t brightness)
{
  //writeCommand(ST7735_WRDBR);
  writeWordData(brightness);
}

void setInvertion(bool i)
{
  writeCommand(i ? ST7735_INVON : ST7735_INVOFF);
}
