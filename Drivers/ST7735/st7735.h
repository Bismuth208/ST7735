#ifndef _ST7735_H
#define _ST7735_H

#include <stdbool.h>
#include <stdint.h>

// some flags for initR() :(
#define INITR_GREENTAB   0x0
#define INITR_REDTAB     0x1
#define INITR_BLACKTAB   0x2

#define INITR_18GREENTAB    INITR_GREENTAB
#define INITR_18REDTAB      INITR_REDTAB
#define INITR_18BLACKTAB    INITR_BLACKTAB
#define INITR_144GREENTAB   0x1

#define ST7735_TFTWIDTH  128
// for 1.44" display
#define ST7735_TFTHEIGHT_144 128
// for 1.8" display
#define ST7735_TFTHEIGHT_18  160

//-------------------------------------------------------------------------------------------//

#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID   0x04
#define ST7735_RDDST   0x09

#define ST7735_SLPIN   0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON   0x12
#define ST7735_NORON   0x13

#define ST7735_INVOFF  0x20
#define ST7735_INVON   0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_RAMRD   0x2E

#define ST7735_PTLAR   0x30
#define ST7735_COLMOD  0x3A
#define ST7735_MADCTL  0x36

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR  0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_PWCTR4  0xC3
#define ST7735_PWCTR5  0xC4
#define ST7735_VMCTR1  0xC5

#define ST7735_RDID1   0xDA
#define ST7735_RDID2   0xDB
#define ST7735_RDID3   0xDC
#define ST7735_RDID4   0xDD

#define ST7735_PWCTR6  0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1


#define MADCTL_MY       0x80
#define MADCTL_MX       0x40
#define MADCTL_MV       0x20
#define MADCTL_ML       0x10
#define MADCTL_RGB      0x00
#define MADCTL_BGR      0x08
#define MADCTL_MH       0x04


//-------------------------------------------------------------------------------------------//

#define DELAY           0x80  // delay marker

#define TFT_BOT_FIXED_AREA 0  // Number of lines in bottom fixed area (lines counted from bottom of screen)
#define TFT_TOP_FIXED_AREA 0 // Number of lines in top fixed area (lines counted from top of screen)

#define WIDTH  ST7735_TFTWIDTH
#define HEIGHT ST7735_TFTHEIGHT_18

//-------------------------------------------------------------------------------------------//
//nss - pb10; dc - pb11; res - pb1
#define TFT_SS_PIN      GPIO_Pin_10     //CS
#define TFT_DC_PIN      GPIO_Pin_11     //DC
#define TFT_RES_PIN     GPIO_Pin_1      //RES on PB1

#define LCD_BACKLIGHT_PIN  GPIO_Pin_6
#define BACKLIGHT_GPIO     GPIOB

#define GPIO_SET_PIN(GPIOx, GPIO_Pin)     GPIOx->BSRR = GPIO_Pin;
#define GPIO_RESET_PIN(GPIOx, GPIO_Pin)   GPIOx->BRR = GPIO_Pin;


#define USE_FSMC      0

#if USE_FSMC
 #define SET_TFT_CS_HI
 #define SET_TFT_CS_LOW

 #define SET_TFT_DC_HI
 #define SET_TFT_DC_LOW

// just a protection
 #define TFT_CS_ALWAS_ACTIVE 0

#else
// free MCU from toggling CS GPIO
// Set this to 0 if not only one TFT is slave on that SPI
 #define TFT_CS_ALWAS_ACTIVE 1

 #define SET_TFT_DC_HI    GPIO_SET_PIN(GPIOB, TFT_DC_PIN)
 #define SET_TFT_DC_LOW   GPIO_RESET_PIN(GPIOB, TFT_DC_PIN)

 //#define SET_TFT_CS_HI    GPIO_SET_PIN(GPIOB, TFT_SS_PIN)
 //#define SET_TFT_CS_LOW   GPIO_RESET_PIN(GPIOB, TFT_SS_PIN)
#endif /* USE_FSMC */

#define SET_TFT_RES_HI     GPIO_SET_PIN(GPIOB, TFT_RES_PIN)
#define SET_TFT_RES_LOW    GPIO_RESET_PIN(GPIOB, TFT_RES_PIN)


#if TFT_CS_ALWAS_ACTIVE
 //#undef SET_TFT_CS_HI
 //#undef SET_TFT_CS_LOW
 #define SET_TFT_CS_HI
 #define SET_TFT_CS_LOW
// make SET_TFT_CS_LOW; always active
 #define GRAB_TFT_CS     GPIO_RESET_PIN(GPIOB, TFT_SS_PIN)
#else
 #define GRAB_TFT_CS     SET_TFT_CS_HI
#endif /* TFT_CS_ALWAS_ACTIVE */


#define SET_CMD()  SET_TFT_DC_LOW
#define SET_DATA() SET_TFT_DC_HI
                        
                        
                        
#if USE_FSMC
 #define WAIT_DMA_BSY
 #define SEND_DATA(a)      FSMC_SEND_CMD(a)
 #define SEND_DATA16(a)    sendData16_FSMC(a)
 #define SEND_2_DATA(a,b)  FSMC_SEND_DATA32(a,b)

 #define SEND_ARR16_FAST(pointer, size) sendData16_Arr_FSMC(pointer, size)
 #define REPEAT_DATA16(data, size)       repeatData16_Arr_FSMC(data, size);

 #define WRITE_CMD(c)      FSMC_SEND_CMD(((uint16_t)(0x00<<8)|c));
 #define WRITE_DATA(c)     FSMC_SEND_DATA(((uint16_t)(0x00<<8)|c));

#else
 #define WAIT_DMA_BSY      wait_DMA1_SPI1_busy()
 #define SEND_DATA(a)      sendData8_SPI1(a)
 #define SEND_DATA16(a)    sendData16_SPI1(a)
 #define SEND_2_DATA(a,b)  sendData32_SPI1(a,b)

 #define SEND_ARR16_FAST(pointer, size) sendData16_Fast_DMA1_SPI1(pointer, size)
 #define REPEAT_DATA16(data, size)      repeatData16_DMA1_SPI1(data, size);

 #define WRITE_CMD(c)       SET_CMD(); sendData8_SPI1(c);
 #define WRITE_DATA(c)      SET_DATA(); sendData8_SPI1(c);

#endif /* USE_FSMC */

                        
extern int16_t _width, _height;

//-------------------------------------------------------------------------------------------//

static const uint8_t Bcmd[] = {                  // Initialization commands for 7735B screens
    18,                       // 18 commands in list:
    ST7735_SWRESET,   DELAY,  //  1: Software reset, no args, w/delay
      50,                     //     50 ms delay
    ST7735_SLPOUT ,   DELAY,  //  2: Out of sleep mode, no args, w/delay
      255,                    //     255 = 500 ms delay
    ST7735_COLMOD , 1+DELAY,  //  3: Set color mode, 1 arg + delay:
      0x05,                   //     16-bit color
      10,                     //     10 ms delay
    ST7735_FRMCTR1, 3+DELAY,  //  4: Frame rate control, 3 args + delay:
      0x00,                   //     fastest refresh
      0x06,                   //     6 lines front porch
      0x03,                   //     3 lines back porch
      10,                     //     10 ms delay
    ST7735_MADCTL , 1      ,  //  5: Memory access ctrl (directions), 1 arg:
      0x08,                   //     Row addr/col addr, bottom to top refresh
    ST7735_DISSET5, 2      ,  //  6: Display settings #5, 2 args, no delay:
      0x15,                   //     1 clk cycle nonoverlap, 2 cycle gate
                              //     rise, 3 cycle osc equalize
      0x02,                   //     Fix on VTL
    ST7735_INVCTR , 1      ,  //  7: Display inversion control, 1 arg:
      0x0,                    //     Line inversion
    ST7735_PWCTR1 , 2+DELAY,  //  8: Power control, 2 args + delay:
      0x02,                   //     GVDD = 4.7V
      0x70,                   //     1.0uA
      10,                     //     10 ms delay
    ST7735_PWCTR2 , 1      ,  //  9: Power control, 1 arg, no delay:
      0x05,                   //     VGH = 14.7V, VGL = -7.35V
    ST7735_PWCTR3 , 2      ,  // 10: Power control, 2 args, no delay:
      0x01,                   //     Opamp current small
      0x02,                   //     Boost frequency
    ST7735_VMCTR1 , 2+DELAY,  // 11: Power control, 2 args + delay:
      0x3C,                   //     VCOMH = 4V
      0x38,                   //     VCOML = -1.1V
      10,                     //     10 ms delay
    ST7735_PWCTR6 , 2      ,  // 12: Power control, 2 args, no delay:
      0x11, 0x15,
    ST7735_GMCTRP1,16      ,  // 13: Magical unicorn dust, 16 args, no delay:
      0x09, 0x16, 0x09, 0x20, //     (seriously though, not sure what
      0x21, 0x1B, 0x13, 0x19, //      these config values represent)
      0x17, 0x15, 0x1E, 0x2B,
      0x04, 0x05, 0x02, 0x0E,
    ST7735_GMCTRN1,16+DELAY,  // 14: Sparkles and rainbows, 16 args + delay:
      0x0B, 0x14, 0x08, 0x1E, //     (ditto)
      0x22, 0x1D, 0x18, 0x1E,
      0x1B, 0x1A, 0x24, 0x2B,
      0x06, 0x06, 0x02, 0x0F,
      10,                     //     10 ms delay
    ST7735_CASET  , 4      ,  // 15: Column addr set, 4 args, no delay:
      0x00, 0x02,             //     XSTART = 2
      0x00, 0x81,             //     XEND = 129
    ST7735_RASET  , 4      ,  // 16: Row addr set, 4 args, no delay:
      0x00, 0x02,             //     XSTART = 1
      0x00, 0x81,             //     XEND = 160
    ST7735_NORON  ,   DELAY,  // 17: Normal display on, no args, w/delay
      10,                     //     10 ms delay
    ST7735_DISPON ,   DELAY,  // 18: Main screen turn on, no args, w/delay
      255 };                  //     255 = 500 ms delay

static const uint8_t Rcmd1[] = {                 // Init for 7735R, part 1 (red or green tab)
    15,                       // 15 commands in list:
    ST7735_SWRESET,   DELAY,  //  1: Software reset, 0 args, w/delay
      150,                    //     150 ms delay
    ST7735_SLPOUT ,   DELAY,  //  2: Out of sleep mode, 0 args, w/delay
      255,                    //     500 ms delay
    ST7735_FRMCTR1, 3      ,  //  3: Frame rate ctrl - normal mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR2, 3      ,  //  4: Frame rate control - idle mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR3, 6      ,  //  5: Frame rate ctrl - partial mode, 6 args:
      0x01, 0x2C, 0x2D,       //     Dot inversion mode
      0x01, 0x2C, 0x2D,       //     Line inversion mode
    ST7735_INVCTR , 1      ,  //  6: Display inversion ctrl, 1 arg, no delay:
      0x07,                   //     No inversion
    ST7735_PWCTR1 , 3      ,  //  7: Power control, 3 args, no delay:
      0xA2,
      0x02,                   //     -4.6V
      0x84,                   //     AUTO mode
    ST7735_PWCTR2 , 1      ,  //  8: Power control, 1 arg, no delay:
      0xC5,                   //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
    ST7735_PWCTR3 , 2      ,  //  9: Power control, 2 args, no delay:
      0x0A,                   //     Opamp current small
      0x00,                   //     Boost frequency
    ST7735_PWCTR4 , 2      ,  // 10: Power control, 2 args, no delay:
      0x8A,                   //     BCLK/2, Opamp current small & Medium low
      0x2A,  
    ST7735_PWCTR5 , 2      ,  // 11: Power control, 2 args, no delay:
      0x8A, 0xEE,
    ST7735_VMCTR1 , 1      ,  // 12: Power control, 1 arg, no delay:
      0x0E,
    ST7735_INVOFF , 0      ,  // 13: Don't invert display, no args, no delay
    ST7735_MADCTL , 1      ,  // 14: Memory access control (directions), 1 arg:
      0xC8,                   //     row addr/col addr, bottom to top refresh
    ST7735_COLMOD , 1      ,  // 15: set color mode, 1 arg, no delay:
      0x05 };                 //     16-bit color

static const uint8_t Rcmd2green[] = {            // Init for 7735R, part 2 (green tab only)
    2,                        //  2 commands in list:
    ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x02,             //     XSTART = 0
      0x00, 0x7F+0x02,        //     XEND = 127
    ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x01,             //     XSTART = 0
      0x00, 0x9F+0x01 };      //     XEND = 159

static const uint8_t Rcmd2red[] = {              // Init for 7735R, part 2 (red tab only)
    2,                        //  2 commands in list:
    ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x7F,             //     XEND = 127
    ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x9F };           //     XEND = 159

static const uint8_t Rcmd2green144[] = {              // Init for 7735R, part 2 (green 1.44 tab)
    2,                        //  2 commands in list:
    ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x7F,             //     XEND = 127
    ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x7F };           //     XEND = 127

static const uint8_t Rcmd3[] = {                 // Init for 7735R, part 3 (red or green tab)
    4,                        //  4 commands in list:
    ST7735_GMCTRP1, 16      , //  1: Magical unicorn dust, 16 args, no delay:
      0x02, 0x1c, 0x07, 0x12,
      0x37, 0x32, 0x29, 0x2d,
      0x29, 0x25, 0x2B, 0x39,
      0x00, 0x01, 0x03, 0x10,
    ST7735_GMCTRN1, 16      , //  2: Sparkles and rainbows, 16 args, no delay:
      0x03, 0x1d, 0x07, 0x06,
      0x2E, 0x2C, 0x29, 0x2D,
      0x2E, 0x2E, 0x37, 0x3F,
      0x00, 0x00, 0x02, 0x10,
    ST7735_NORON  ,    DELAY, //  3: Normal display on, no args, w/delay
      10,                     //     10 ms delay
    ST7735_DISPON ,    DELAY, //  4: Main screen turn on, no args w/delay
      100 };                  //     100 ms delay

//-------------------------------------------------------------------------------------------//

void writeCommand(uint8_t c);
void writeData(uint8_t d);
void writeWordData(uint16_t c);
  
void commandList(const uint8_t *addr);

void initLCD(void);
void initB(void);                             // for ST7735B displays
void initR(uint8_t options); // for ST7735R
void commonInit(const uint8_t *cmdList);

void tftBegin(void);
void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void setVAddrWindow(uint16_t x0, uint16_t y0, uint16_t y1);
void setHAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1);
void setAddrPixel(uint16_t x0, uint16_t y0);
void setRotation(uint8_t m);
void setScrollArea(uint16_t TFA, uint16_t BFA);
void scrollAddress(uint16_t VSP);
uint16_t scroll(uint16_t lines, uint16_t yStart);
uint16_t scrollSmooth(uint16_t lines, uint16_t yStart, uint8_t wait);
void setSleep(bool enable);
void setIdleMode(bool mode);
void setDispBrightness(uint8_t brightness);
void setInvertion(bool i);   
//void setGamma(uint8_t gamma);


#endif  /* ST7745_H */
