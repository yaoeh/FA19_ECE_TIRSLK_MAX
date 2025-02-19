// TExaSmain.c
// Runs on MSP432
// Basic TExaS scope operation
// a 10 kHz 8-bit oscilloscope on J3.26/P4.4/A9
// Daniel and Jonathan Valvano
// September 23, 2017

/* This example accompanies the book
   "Embedded Systems: Introduction to Robotics,
   Jonathan W. Valvano, ISBN: 9781074544300, copyright (c) 2019
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/

Simplified BSD License (FreeBSD License)
Copyright (c) 2019, Jonathan Valvano, All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are
those of the authors and should not be interpreted as representing official
policies, either expressed or implied, of the FreeBSD Project.
*/

// This main program outputs a squarewave on P4.5
// TExaS scope uses P4.4 as input to scope
// To see the squarewave connect P4.4 to P4.5

#include <stdint.h>
#include "msp.h"
#include "..\inc\TExaS.h"
#include "..\inc\Clock.h"
#include "..\inc\SysTick.h"
#include "..\inc\LaunchPad.h"
int scopeMain(void){ //scopeMain(void){
  Clock_Init48MHz();  // makes bus clock 48 MHz
  TExaS_Init(SCOPE);
  P4->SEL0 &= ~0x20;  // configure P4.5 GPIO output
  P4->SEL1 &= ~0x20;
  P4->DIR |= 0x20;    // P4.5 output
  while(1){
    P4->OUT ^= 0x20;  // toggle
    Clock_Delay1us(1000); // wait 1 ms
  }
}

int main(void){
uint32_t status,delay,data;
  Clock_Init48MHz(); // makes bus clock 48 MHz
  LaunchPad_Init();  // use buttons to step through frequencies
  TExaS_Init(LOGICANALYZER_P1);
  data = 0;
  while(1){
    status = LaunchPad_Input();
    switch(status){           // switches are negative logic on P1.1 and P1.4
      case 0x00: delay=1000; break;  // neither switch pressed
      case 0x01: delay=2000; break;  // SW2 pressed
      case 0x02: delay=3000; break;  // SW1 pressed
      case 0x03: delay=4000; break;  // both switches pressed
    }
    Clock_Delay1us(delay);
    data = data ^0x01;
    LaunchPad_LED(data); // toggle red LED
  }
}

//square frequency in Hz on P4.5, used in Lab 2
uint32_t const FreqTab[4]={100, 500, 720, 1000}; // Hz
int Lab2Main(void){ //Lab2main(void){
    int i; uint32_t in,last;
  Clock_Init48MHz(); // makes bus clock 48 MHz
  LaunchPad_Init();  // use buttons to step through frequencies
  SysTick_Init();
  TExaS_Init(SCOPE);
  P4->SEL0 &= ~0x20;  // configure P4.5 GPIO output
  P4->SEL1 &= ~0x20;
  P4->DIR |= 0x20;    // P4.5 output
  i = 0;
  in = last = LaunchPad_Input(); // 0 means none
  while(1){
    P4->OUT ^= 0x20;  // toggle
    SysTick_Wait(24000000/FreqTab[i]); // 1/2 period wait
    in = LaunchPad_Input(); // 0 means none
    if(in&&(last==0)){ // on touch
      i = (i+1)&0x03;  // 0,1,2,3
      LaunchPad_LED(1);
      while(LaunchPad_Input()){};
      SysTick_Wait(480000);   // 10 ms
      in = LaunchPad_Input(); // 0 means none
      LaunchPad_LED(0);
      LaunchPad_Output(i);
    }
    last = in;
  }
}

