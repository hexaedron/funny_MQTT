#include <ch32v003fun.h>
#include <stdbool.h>

#include "funny_defs.h"

//Globals
static volatile uint32_t     _pin_num      = 0;       // Pins bitmask for EXTI

void system_initEXTI(uint32_t pin, bool risingEdge = true, bool fallingEdge = false)
{ 
  // Setup pin-change-interrupt.  This will trigger when the voltage on the
  // pin rises above the  schmitt trigger threshold.
  AFIO->EXTICR = HW_PORT_NUM(pin) << (HW_PIN_NUM(pin) * 2);
  EXTI->INTENR = 1 << HW_PIN_NUM(pin);     // Enable the interrupt request signal for external interrupt channel
  
  if(risingEdge)  EXTI->RTENR = 1 << HW_PIN_NUM(pin);     // Rising edge trigger
  if(fallingEdge) EXTI->FTENR = 1 << HW_PIN_NUM(pin);     // Falling edge trigger
  
  _pin_num |= 1 << HW_PIN_NUM(pin); // Set the state of interrupt mask

  NVIC_EnableIRQ(EXTI7_0_IRQn);
}

//extern "C" INTERRUPT_HANDLER 
//void EXTI7_0_IRQHandler(void) 
//{
//  uint32_t pinState = funDigitalRead(PC6);
//
//  if(pinState == 1)
//  {
//    _btn_released = true;
//  }
//  else
//  {
//    _btn_pressed = true;
//  }
//
//  EXTI->INTFR = _pin_num; // Acknowledge the only interrupt we use in our init functions
//}