/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
#include "SmartCar.h"
#include <cstdio>
#include <type_traits>
#include "calculator.h"

#define WAIT_TIME_MS 500
#define Forward 1
#define Backward -1
#define Park 0
#define Right 2
#define Left 3

DigitalOut led1(LED1);
DigitalIn IRsensor(PB_13);

Ticker sampleDistance;
Ticker sampleOdometry;
Ticker sampleCounter;

// example setup for echo and trigger using D2 and D3
DigitalIn echo_back(PC_9);
DigitalOut trigger_back(PB_8);

// example setup for echo and trigger using D2 and D3
DigitalIn echo_front(PB_1);
DigitalOut trigger_front(PB_2);

//Define the threads
// Thread t1;
// Create a BufferedSerial object with a default baud rate
static BufferedSerial pc(USBTX, USBRX);

//global variables to determine the distance to certain ojects
volatile float distanceFront;
volatile float distanceBack;
volatile float speed;

//different possible speeds
const float SpeedHigh = 1.0f;
const float SpeedMid = 0.5f;
const float SpeedLow = 0.1f;
const float GearPark = 0.0f;

const float pi = 3.1415926;

const int wheelRadius = 3; // in cm
const int turnRadius = 11; // in cm


//counter used for measuring distance
volatile int counter = 0;
volatile bool preV = 0;
//location on the map in terms of distane and angle
volatile float d = 0;
volatile float theta = 0;
volatile float x = 0; 
volatile float y = 0;

// timer for echo code
Timer echoDuration;
using namespace std::chrono; // namespace for timers
//constructor for our car
SmartCar Car = SmartCar();

enum LA_States { LA_SMStart, LA_roll, LA_bump } LA_State;

void TickFct_Latch()
{
  switch(LA_State) {   // Transitions
     case LA_SMStart:  // Initial transition
        LA_State = LA_roll;
        break;
    //rolling state we just keep moving forward at an appropriate speed
     case LA_roll:
        if (distanceFront < 10.0) {
            speed = GearPark;
            LA_State = LA_bump;
        } else if (distanceFront < 13.0 ) {
            speed = SpeedLow;
        } else if (distanceFront < 20.0) {
            speed = SpeedMid;
        } else {
            speed = SpeedHigh;
        }

        if (speed == GearPark) Car.stop();
        else  Car.goForward(speed);

        break;
    

     case LA_bump:
        while ((distanceFront < 20) && (distanceBack<10)) {
            Car.goBackward(SpeedMid);
        } 
        while (distanceFront > 30){
            Car.turnLeft(SpeedMid);
        }
        LA_State = LA_roll;
        break;

     default:
        LA_State = LA_SMStart;
        break;
  }
  } // Transitions



float getDistanceTraveled(){
    int localCount = counter;
    counter = 0;
    return wheelRadius * 2 * pi * localCount / 20;
}

void updateLocation(){
    float distance = getDistanceTraveled();

    if (Car.myGear() == 1) {
        x = x + distance*cos(theta);
        y = y + distance *sin(theta);
        d = sqrt(x*x + y*y);
        // if ((x == 0)&&(y==0)){
        //     theta = 0;
        // }else {
        //     theta = atan(y/x);
        // }
        
    } else if (Car.myGear() == Backward) {
        x = x + distance*cos(theta + pi);
        y = y + distance *sin(theta + pi);
        d = sqrt(x*x + y*y);
        // if ((x == 0)&&(y==0)){
        //     theta = 0;
        // }else {
        //     theta = atan(y/x);
        // }
    } else if (Car.myGear() == Right) {
        theta = theta - distance/turnRadius;
        if (theta<(-2*pi)){
            theta = theta  + 2*pi;
        }
        
    } else if (Car.myGear() == Left) {
        theta = theta + distance/turnRadius;
        if (theta>(2*pi)){
            theta = theta - 2*pi;
        } 
    }
}

void updateCounter(){
    if ((IRsensor.read() == 0) && (preV == 0 )){
        counter ++; 
        preV = 1;
    } else if ((IRsensor.read()!= 0) && (preV > 0 )){
        preV = 0 ;
    }

}


float getInputFront(){
    // trigger pulse
    trigger_front = 1;
    wait_us(10);
    trigger_front = 0;

    while (echo_front != 1); // wait for echo to go high
    echoDuration.start(); // start timer
    while (echo_front == 1);
    echoDuration.stop(); // stop timer
    
     float distance =  ((float)duration_cast<microseconds>(echoDuration.elapsed_time()).count()/58.0);
    // print distance in cm (switch to divison by 148 for inches)
    echoDuration.reset(); // need to reset timer (doesn't happen automatically)
    return distance;

} 

float getInputBack(){
    
    // trigger pulse
    trigger_back = 1;
    wait_us(10);
    trigger_back = 0;

    while (echo_back != 1); // wait for echo to go high
    echoDuration.start(); // start timer
    while (echo_back == 1);
    echoDuration.stop(); // stop timer
    
    float distance =  ((float)duration_cast<microseconds>(echoDuration.elapsed_time()).count()/58.0);
    // print distance in cm (switch to divison by 148 for inches)
    echoDuration.reset(); // need to reset timer (doesn't happen automatically)
    return distance;

} 

void distanceADC(){
    distanceFront = getInputFront();
    distanceBack = getInputBack();
}


//Display the song name on the LCD and the RGB LEDs
void update_distance_thread(){
    while(1){
        
        distanceFront = getInputFront();
        distanceBack = getInputBack();
       ThisThread::sleep_for(500ms);
    }
}


int main()
{
    printf("This is the bare metal blinky example running on Mbed OS %d.%d.%d.\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);


    // t1.start(update_distance_thread);
    // LA_State = LA_SMStart; // Indicates initial call


    sampleDistance.attach(&distanceADC,50ms);
    sampleCounter.attach(&updateCounter,5ms);
    sampleOdometry.attach(&updateLocation,50ms);

    char action = 'Z';
    char buff;
    int desiredDistance = 0;
    int state = 1;
    bool inAction = false;
    bool inRead = true;
    speed = SpeedLow;
    float thetaNow = 0;
    while (true)
    {   
        
        // thread_sleep_for(WAIT_TIME_MS);
        // TickFct_Latch();
        thread_sleep_for(100);
        if(inRead){
            led1 = !led1;
            // printf("Action : \n");
            while((action !='A' && action !='B' && action !='C' && action != 'D')){
                action = getCharacter(true);
            }
            // printf("distance or direction : \n");
            buff = getCharacter(true);
            inAction = false;
            while (buff != '#') {
                desiredDistance = desiredDistance * 10 + (buff - 0x30);
                buff = getCharacter(true);
                inRead= false;
                inAction = true;
            }
        }
        if(inAction){
            led1 = 1;
            switch (action) {
                case 'A':
                if(d < desiredDistance){
                    Car.goForward(speed);
                    thread_sleep_for(50);
                }else{
                Car.stop();
                inRead= true;
                inAction = false;
                desiredDistance = 0;
                theta = 0;
                action = 'Z';   
                }     
                    break;
                case 'B':
                if(d < desiredDistance){
                    Car.goBackward(speed);
                    thread_sleep_for(50);
                }else{
                Car.stop();
                inRead= true;
                inAction = false; 
                desiredDistance = 0;
                theta = 0;
                action = 'Z';
                }
            
                break;
                case 'C':
                if(theta < (desiredDistance*pi/180)){
                    Car.turnLeft(0.2f);
                    thread_sleep_for(50);
                }else{
                inRead= true;
                inAction = false;
                desiredDistance = 0;
                action = 'Z';
                theta = 0;
                Car.stop();

                } 
                    break;
                case 'D':
                if(theta > ((-1) * desiredDistance * pi/180)){
                    Car.turnRight(0.2f);
                    thread_sleep_for(50);
                }else{
                inRead= true;
                inAction = false; 
                desiredDistance = 0;
                theta = 0 ;
                action = 'Z';
                Car.stop();
                }
                    break;
                default:
                    Car.stop();
            }
        }

        // Car.goBackward(1.0f);
        // printf("distance front : %0.3f \n distance back: %0.3f", distanceFront, distanceBack);
        // if (state == 1) {
        if (Car.myGear() == Forward) {
            if (distanceFront < 20.0) {
                speed = GearPark;
            } else if (distanceFront < 30.0 ) {
                speed = SpeedLow;
            } else if (distanceFront < 40.0) {
                speed = SpeedMid;
            } else {
                speed = SpeedHigh;
            }
        } else if (Car.myGear() == Backward){
            if (distanceBack < 20.0) {
                speed = GearPark;
            } else if (distanceBack < 30.0 ) {
                speed = SpeedLow;
            } else if (distanceBack < 40.0) {
                speed = SpeedMid;
            } else {
                speed = SpeedHigh;
            } 
        }

        //     if (speed == GearPark) Car.stop();
        //     else  Car.goForward(speed);
        // }

    // if (state ==0 ) {
    //     while ((distanceFront < 20) && (distanceBack<10)) {
    //         Car.goBackward(SpeedMid);
    //     } 
    //     while (distanceFront > 30){
    //         Car.turnLeft(SpeedMid);
    //     }
    //     state = 1;
    // }
    // for (int i = 1; i<6; i++){
    //         while(d < i * 20){
    //             Car.goForward(0.2f);
    //             thread_sleep_for(100);
    //             // printf("distance travelled : %0.3f", x);
    //         }
    //         Car.stop();
    //         thread_sleep_for(2000);
    //     }        
    // // Car.goForward(0.2f);
    // thread_sleep_for(500);
    // for (int i = 1; i<4; i++){
    //         while(theta < i * (pi/2)){
    //             Car.turnLeft(0.2f);
    //             thread_sleep_for(100);
    //             printf("theta : %0.3f", theta);
    //         }
    //         Car.stop();
    //         thread_sleep_for(2000);
    //     }        
    }
}
