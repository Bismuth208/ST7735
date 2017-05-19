#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include <stm32f10x.h>

#include <systicktimer.h>
#include <gfx.h>
#if USE_FSMC
 #include <fsmcdrv.h>
#else
 #include <spi.h>
#endif

#define T_GPU_VERSION   "Build: " __DATE__" " __TIME__ " \n"

//-------------------------------------------------------------//
static uint32_t nextInt = 9; //he he he, return 9...       ...dilbert

//-------------------------------------------------------------//

uint32_t randNum(void)
{
  nextInt ^= nextInt >> 6;
  nextInt ^= nextInt << 11;
  nextInt ^= nextInt >> 15;
  return ( nextInt * 3538123 );
}

//===========================================================================//
__noreturn void matrixScreen()
{
  uint8_t colX, rowsY;
  
  colX = _width / 5;
  rowsY = _height / 8;
  
  for(;;) {
    
    cp437(randNum() % 2);
    
    drawChar((randNum() % colX) * 6, (randNum() % rowsY) * 8, // pos X and Y                           
             (((randNum() % 192 + 32) & 0xFC) << 3),          // text color
             COLOR_BLACK,                                     // BG color
             (randNum() % 255), 1);                           // number of char and size
  
  }
}

//===========================================================================//
void init_GPIO_RCC(void)
{
  SET_BIT(RCC->APB2ENR, RCC_APB2Periph_GPIOA); // GPIOA
  SET_BIT(RCC->APB2ENR, RCC_APB2Periph_GPIOB); // GPIOB
  
#ifdef STM32F10X_MD
  SET_BIT(RCC->APB2ENR, RCC_APB2Periph_GPIOC); // GPIOC
#endif

#ifdef STM32F10X_HD
  SET_BIT(RCC->APB2ENR, RCC_APB2Periph_GPIOE); // GPIOE
  
 #if USE_FSMC
  SET_BIT(RCC->APB2ENR, RCC_APB2Periph_GPIOC); // GPIOC
  SET_BIT(RCC->APB2ENR, RCC_APB2Periph_GPIOD); // GPIOD
 #endif  
#endif
}

void startupInit()
{
  /*
  * NOTE!
  * STM32 confugired for 72Mhz Sys clock!
  * To change that go to "system_stm32f10x.c"
  * and find "#define SYSCLK_FREQ_72MHz  72000000"
  * select what you need
  */
  SystemInit(); // init clocks and another s...
  initSysTickTimer();
  
  init_GPIO_RCC();
  initLCD();
  
  setRotation(1);
}

//------------------------- yep, here's how it all began... -------------------//
__noreturn void main(void)
{
  startupInit();

  matrixScreen();
}
//-----------------------------------------------------------------------------//

#ifdef  USE_FULL_ASSERT
/**
* @brief  Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  file: pointer to the source file name
* @param  line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  
  /* Infinite loop */
  while (1)
  {
  }
}
#endif
