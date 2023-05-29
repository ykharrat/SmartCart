#include "SmartCar.h"
#include "mbed.h"


DigitalOut motorBackLeftF(PA_11);
DigitalOut motorBackLeftB(PA_12);
PwmOut speedPWMBackLeft(ARDUINO_UNO_D3);
PwmOut speedPWMBackRight(ARDUINO_UNO_D5);
DigitalOut motorBackRightB(ARDUINO_UNO_D2);
DigitalOut motorBackRightF(ARDUINO_UNO_D4);
PwmOut speedPWMFrontLeft(ARDUINO_UNO_D6);
DigitalOut motorFrontLeftB(ARDUINO_UNO_D7);
DigitalOut motorFrontLeftF(ARDUINO_UNO_D8);
PwmOut speedPWMFrontRight(ARDUINO_UNO_D9);
DigitalOut motorFrontRightF(ARDUINO_UNO_D10);
DigitalOut motorFrontRightB(ARDUINO_UNO_D11);

SmartCar::SmartCar() : speed_(0) {}

const float max_change = 0.05f;

float absolute(float difference){
    if (difference<0) return -1 * difference;
    else return difference; 
}

float SmartCar::smoothAcc(float desired_vel){
    // float difference = desired_vel - speed_;
    // if (absolute(difference) < max_change){
    //     return desired_vel;
    // } else {
    //     if (difference < 0){
    //         desired_vel = speed_ - max_change;
    //     } else {
    //         desired_vel = speed_ + max_change;
    //     }
    // return desired_vel;
    // }
    return desired_vel;
}

void SmartCar::goForward(float speed) {
        motorFrontLeftF = 1;
        motorFrontLeftB = 0;
        speed = smoothAcc(speed);
        speedPWMFrontLeft.write(speed);
        motorFrontRightF = 1;
        motorFrontRightB = 0;
        speedPWMFrontRight.write(speed);
        motorBackLeftF = 1;
        motorBackLeftB = 0;
        speedPWMBackLeft.write(speed);
        motorBackRightF = 1;
        motorBackRightB = 0;
        speedPWMBackRight.write(speed);
        gear_ = 1;
        speed_ = speed;
}

void SmartCar::goBackward(float speed) {
        speed = smoothAcc(speed);
        motorFrontLeftF = 0;
        motorFrontLeftB = 1;
        speedPWMFrontLeft.write(speed);
        motorFrontRightF = 0;
        motorFrontRightB = 1;
        speedPWMFrontRight.write(speed);
        motorBackLeftF = 0;
        motorBackLeftB = 1;
        speedPWMBackLeft.write(speed);
        motorBackRightF = 0;
        motorBackRightB = 1;
        speedPWMBackRight.write(speed);
        gear_ = -1;
        speed_ = speed;
}

void SmartCar::turnRight(float speed) {
        speed = smoothAcc(speed);
        motorFrontLeftF = 0;
        motorFrontLeftB = 1;
        speedPWMFrontLeft.write(speed);
        motorFrontRightF = 1;
        motorFrontRightB = 0;
        speedPWMFrontRight.write(speed);
        motorBackLeftF = 0;
        motorBackLeftB = 1;
        speedPWMBackLeft.write(speed);
        motorBackRightF = 1;
        motorBackRightB = 0;
        speedPWMBackRight.write(speed);
        gear_ = 2;
        speed_ = speed;
}

void SmartCar::turnLeft(float speed) {
        speed = smoothAcc(speed);
        motorFrontLeftF = 1;
        motorFrontLeftB = 0;
        speedPWMFrontLeft.write(speed);
        motorFrontRightF = 0;
        motorFrontRightB = 1;
        speedPWMFrontRight.write(speed);
        motorBackLeftF = 1;
        motorBackLeftB = 0;
        speedPWMBackLeft.write(speed);
        motorBackRightF = 0;
        motorBackRightB = 1;
        speedPWMBackRight.write(speed);
        gear_ = 3;
        speed_ = speed;
}

void SmartCar::goDiagonalLeft(float speed) {
        motorFrontLeftF = 0;
        motorFrontLeftB = 0;
        speedPWMFrontLeft.write(speed);
        motorFrontRightF = 1;
        motorFrontRightB = 0;
        speedPWMFrontRight.write(speed);
        motorBackLeftF = 0;
        motorBackLeftB = 0;
        speedPWMBackLeft.write(speed);
        motorBackRightF = 1;
        motorBackRightB = 0;
        speedPWMBackRight.write(speed);
}

void SmartCar::goDiagonalRight(float speed) {
        motorFrontLeftF = 1;
        motorFrontLeftB = 0;
        speedPWMFrontLeft.write(speed);
        motorFrontRightF = 0;
        motorFrontRightB = 0;
        speedPWMFrontRight.write(speed);
        motorBackLeftF = 1;
        motorBackLeftB = 0;
        speedPWMBackLeft.write(speed);
        motorBackRightF = 0;
        motorBackRightB = 0;
        speedPWMBackRight.write(speed);
}



void SmartCar::slideRight(float speed) {
        motorFrontLeftF = 1;
        motorFrontLeftB = 0;
        speedPWMFrontLeft.write(speed);
        motorFrontRightF = 0;
        motorFrontRightB = 1;
        speedPWMFrontRight.write(speed);
        motorBackLeftF = 0;
        motorBackLeftB = 1;
        speedPWMBackLeft.write(speed);
        motorBackRightF = 1;
        motorBackRightB = 0;
        speedPWMBackRight.write(speed);
}

void SmartCar::slideLeft(float speed) {
        motorFrontLeftF = 0;
        motorFrontLeftB = 1;
        speedPWMFrontLeft.write(speed);
        motorFrontRightF = 1;
        motorFrontRightB = 0;
        speedPWMFrontRight.write(speed);
        motorBackLeftF = 1;
        motorBackLeftB = 0;
        speedPWMBackLeft.write(speed);
        motorBackRightF = 0;
        motorBackRightB = 1;
        speedPWMBackRight.write(speed);
}

void SmartCar::stop() {
        motorFrontLeftF = 0;
        motorFrontLeftB = 0;
        motorFrontRightF = 0;
        motorFrontRightB = 0;
        motorBackLeftF = 0;
        motorBackLeftB = 0;
        motorBackRightF = 0;
        motorBackRightB = 0;
        gear_ = 0;
        speed_ = 0;
}


int SmartCar::mySpeed() {
    return speed_;
}

int SmartCar::myGear() {
    return gear_;
}

