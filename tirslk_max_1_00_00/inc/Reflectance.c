// Reflectance.c
// Provide functions to take measurements using the kit's built-in
// QTRX reflectance sensor array.  Pololu part number 3672. This works by outputting to the
// sensor, waiting, then reading the digital value of each of the
// eight phototransistors.  The more reflective the target surface is,
// the faster the voltage decays.
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

// reflectance even LED illuminate connected to P5.3
// reflectance odd LED illuminate connected to P9.2
// reflectance sensor 1 connected to P7.0 (robot's right, robot off road to left)
// reflectance sensor 2 connected to P7.1
// reflectance sensor 3 connected to P7.2
// reflectance sensor 4 connected to P7.3 center
// reflectance sensor 5 connected to P7.4 center
// reflectance sensor 6 connected to P7.5
// reflectance sensor 7 connected to P7.6
// reflectance sensor 8 connected to P7.7 (robot's left, robot off road to right)

#include <stdint.h>
#include "msp432.h"
#include "..\tirslk_max_1_00_00\inc\Clock.h"

int W[8]= {334,238,142,48,-48,-142,-238,-334};
int HexPos[8]={0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

int line_result= 0; //global reflectance results

// ------------Reflectance_Init------------
// Initialize the GPIO pins associated with the QTR-8RC
// reflectance sensor.  Infrared illumination LEDs are
// initially off.
// Input: none
// Output: none
void Reflectance_Init(void){
    // write this as part of Lab 6

    //Clearing P5.3, and 9.2 then setting it to an output (i.e. 1)
    P5->SEL0 &= ~0x04; //Clearing bit 3 with 1111 1011
    P5->SEL1 &= ~0x04; //Clearing bit 3 with 1111 1011
    P5->DIR |= 0x04; //Setting direction registry bit 3 to 1 (Output)
    P5->OUT &= ~0x04; //Set P5.3 initially LOW (0) by ANDING with 1111 1011

    P9->SEL0 &= ~0x03; //Clearing bit 3 with 1111 1011
    P9->SEL1 &= ~0x03; //Clearing bit 3 with 1111 1011
    P9->DIR |= 0x03; //Setting direction registry bit 3 to 1 (Output)
    P9->OUT &= ~0x03; //Set P5.3 initially LOW (0) by ANDING with 1111 1011

//    P7->SEL0 &= ~0x01; //Clearing bit 0
//    P7->SEL1 &= ~0x01; //Clearing bit 0
//    P7->DIR &= ~0x01; //Set Pin 7 bit 0 to an input (i.e. 0) by ANDING it with a 1111 1110 (FE)

    P7->SEL0 &= 0x00; //Clearing all the bits
    P7->SEL1 &= 0x00; //Clearing all the bits
    P7->DIR &= 0x00; //all the pins to input (0)

    P4->SEL0 &= ~0x01; //Clearing bit 0
    P4->SEL1 &= ~0x01; //Clearing bit 0
    P4->DIR |= 0x01; //Set Pin 4 bit 0 to equal to 1 (output)

//    printf("initialized\n");
}

// ------------Reflectance_Read------------
// Read the eight sensors
// Turn on the 8 IR LEDs
// Pulse the 8 sensors high for 10 us
// Make the sensor pins input
// wait t us
// Read sensors
// Turn off the 8 IR LEDs
// Input: time to wait in usec
// Output: sensor readings
// Assumes: Reflectance_Init() has been called
uint8_t Reflectance_Read(uint32_t time){
    uint8_t result;
    // write this as part of Lab 6

    P5->OUT |= 0x04; //Set p5.3 to HIGH
    P9->OUT |= 0x02; //Set p9.2 to HIGH

    P7->DIR |= ~0x00; //Set p7 to Output (1)
    P7->OUT |= ~0x00; //Set p7 all to HIGH
    Clock_Delay1us(10);   // wait 10 us
    P7->DIR &= 0x00; //Set p7 to Input (0)

    Clock_Delay1us(time);
    result = P7->IN; //Read from P7

    P5->OUT &= ~0x04; //Set p5.3 to LOW
    P9->OUT &= ~0x02; //Set p9.2 to LOW

    return result;
}

// ------------Reflectance_Center------------
// Read the two center sensors
// Turn on the 8 IR LEDs
// Pulse the 8 sensors high for 10 us
// Make the sensor pins input
// wait t us
// Read sensors
// Turn off the 8 IR LEDs
// Input: time to wait in usec
// Output: 0 (off road), 1 off to left, 2 off to right, 3 on road
// (Left,Right) Sensors
// 1,1          both sensors   on line
// 0,1          just right     off to left
// 1,0          left left      off to right
// 0,0          neither        lost
// Assumes: Reflectance_Init() has been called
uint8_t Reflectance_Center(uint32_t time){
    // write this as part of Lab 6
  return 0; // replace this line
}


// Perform sensor integration
// Input: data is 8-bit result from line sensor
// Output: position in 0.1mm relative to center of line
int32_t Reflectance_Position(uint8_t data){
    // write this as part of Lab 6
    int32_t result=0;
    int32_t total_count=0;

    for (uint8_t n= 0; n< 8; n++){
//        uint8_t bit_value= ((data | ~(pow(2,n))) == 0xFF); //If the whole thing is 1 after changes?
        if ((data & HexPos[n]) == HexPos[n]){ //Anding the mask with a 1 in the specific area
            result+= W[n];
            total_count+= 1;
        }
    }
//    printf("Result: %d, total count:%d ", result, total_count);

    if (total_count> 0){
        return result/total_count;
    }else{
        return -999;
    }
}

char *Reflectance_String(uint8_t data){
    static char answer[8];
    for (uint8_t n= 0; n< 8; n++){
//        uint8_t bit_value= ((data | ~(pow(2,n))) == 0xFF); //If the whole thing is 1 after changes?
        if ((data & HexPos[n]) == HexPos[n]){ //Anding the mask with a 1 in the specific area
            answer[7-n]= '1';
        }else{
            answer[7-n]= '0';
        }
    }
    return answer;
}

/* Function to get no of set bits in binary
   representation of positive integer n */
unsigned int countSetBits(unsigned int n)
{
    unsigned int count = 0;
    while (n) {
        count += n & 1;
        n >>= 1;
    }
    return count;
}

//uint8_t Reflectance_Tune_Val(uint8_t data){
//    /*
//     * Value returned as follows:
//     *
//     * 0 for nothing or more than 3 1's
//     * 2 1's together
//     * 3 1's together, or 1' 1
//     *
//     * The goal is to eventually find a set of values such that it is consistently 2
//     *
//     */
//}


// ------------Reflectance_Start------------
// Begin the process of reading the eight sensors
// Turn on the 8 IR LEDs
// Pulse the 8 sensors high for 10 us
// Make the sensor pins input
// Input: none
// Output: none
// Assumes: Reflectance_Init() has been called
void Reflectance_Start(void){
    P5->OUT |= 0x04; //Set p5.3 to HIGH
    P9->OUT |= 0x02; //Set p9.2 to HIGH
    P7->DIR |= ~0x00; //Set p7 to Output (1)
    P7->OUT |= ~0x00; //Set p7 all to HIGH
//    Clock_Delay1us(10);   // wait 10 us
//    P7->DIR &= 0x00; //Set p7 to Input (0)
}


// ------------Reflectance_End------------
// Finish reading the eight sensors
// Read sensors
// Turn off the 8 IR LEDs
// Input: none
// Output: sensor readings
// Assumes: Reflectance_Init() has been called
// Assumes: Reflectance_Start() was called 1 ms ago
uint8_t Reflectance_End(void){
    // write this as part of Lab 10
    line_result = P7->IN; //Read from P7
    P5->OUT &= ~0x04; //Set p5.3 to LOW
    P9->OUT &= ~0x02; //Set p9.2 to LOW
    return line_result;
}

