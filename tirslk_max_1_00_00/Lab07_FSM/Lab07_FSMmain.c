// Lab07_FSMmain.c
// Runs on MSP432
// Student version of FSM lab, FSM with 2 inputs and 2 outputs.
// Rather than real sensors and motors, it uses LaunchPad I/O
// Daniel and Jonathan Valvano
// July 11, 2019

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

#include <stdint.h>
#include "msp.h"
#include "../inc/clock.h"
#include "../inc/LaunchPad.h"
#include "../inc/Texas.h"

/*(Left,Right) Motors, call LaunchPad_Output (positive logic)
3   1,1     both motors, yellow means go straight
2   1,0     left motor,  green  means turns right
1   0,1     right motor, red    means turn left
0   0,0     both off,    dark   means stop
(Left,Right) Sensors, call LaunchPad_Input (positive logic)
3   1,1     both buttons pushed means on line,
2   1,0     SW2 pushed          means off to right
1   0,1     SW1 pushed          means off to left
0   0,0     neither button      means lost
 */

// Linked data structure
struct State {
  uint32_t out;                // 2-bit output
  uint32_t delay;              // time to delay in 1ms
  const struct State *next[4]; // Next if 2-bit input is 0-3
};
typedef const struct State State_t;

#define Center    &fsm[0]
#define Left      &fsm[1]
#define Right     &fsm[2]
#define Left2      &fsm[3]
#define Right2     &fsm[4]
#define E_Left      &fsm[5]
#define E_Right     &fsm[6]
#define Rec_Center &fsm[7]
#define Stop &fsm[8]


State_t fsm[9]={
  {0x01, 500, { Right, Left,   Right,  Center }},  // Center, red
  {0x02, 500, { E_Right,  Left2, Right,  Center }},  // Left, green
  {0x03, 500, { E_Left, Left,   Right2, Center }},   // Right, yellow

  {0x04, 500, { E_Left, Left,   Right, Center }},   // Right2, blue
  {0x04, 500, { E_Right, Left,   Right, Center }},   // Left2, blue

  {0x05, 500, { Rec_Center, Left,   Right, Center }},   // E_Right, pink
  {0x05, 500, { Rec_Center, Left,   Right, Center }},   // E_Left, pink

  {0x06, 500, { Stop, Left,   Right, Center }},   // Recover_Center, sky blue
  {0x07, 500, { Stop, Stop,   Stop, Stop }},   // Stop, white
};

State_t *Spt;  // pointer to the current state
uint32_t Input;
uint32_t Output;
/*Run FSM continuously
1) Output depends on State (LaunchPad LED)
2) Wait depends on State
3) Input (LaunchPad buttons)
4) Next depends on (Input,State)
 */
int main(void){ uint32_t heart=0;
  Clock_Init48MHz();
  LaunchPad_Init();
  TExaS_Init(LOGICANALYZER);  // optional
  Spt = Center;
  while(1){
    Output = Spt->out;            // set output from FSM
    LaunchPad_Output(Output);     // do output to two motors
    TExaS_Set(Input<<2|Output);   // optional, send data to logic analyzer
    Clock_Delay1ms(Spt->delay);   // wait
    Input = LaunchPad_Input();    // read sensors
    Spt = Spt->next[Input];       // next depends on input and state
    heart = heart^1;
    LaunchPad_LED(heart);         // optional, debugging heartbeat
  }
}

// Color    LED(s) Port2
// dark     ---    0
// red      R--    0x01
// blue     --B    0x04
// green    -G-    0x02
// yellow   RG-    0x03
// sky blue -GB    0x06
// white    RGB    0x07
// pink     R-B    0x05
