#ifndef SMARTCAR_H
#define SMARTCAR_H

class SmartCar {
private:
  int speed_;
  // 0 = park; 1 = forward; -1 = backward; 2 = right; 3 = left
  int gear_;
  

public:
  SmartCar();
  void goForward(float speed);
  void goBackward(float speed);
  void goDiagonalRight(float speed);
  void goDiagonalLeft(float speed);
  void turnRight(float speed);
  void turnLeft(float speed);
  void slideLeft(float speed);
  void slideRight(float speed);
  float smoothAcc(float desired_vel);
  void stop();
  int mySpeed();
  int myGear();

  

  
};

#endif // SMARTCAR_H