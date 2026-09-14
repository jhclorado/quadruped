#ifndef SEQUENCES_H
#define SEQUENCES_H
// #define DAVIN
// #define PIOLO
#pragma once
#include "legs.h"
#define STANDING_OFFSET 0
extern Quadruped robot;
extern Controller controller;


const int LIFT_FRONT_LEFT_FIBULA = 45;
const int LIFT_FRONT_RIGHT_FIBULA = 135;
const int LIFT_BACK_LEFT_FIBULA = 135;
const int LIFT_BACK_RIGHT_FIBULA = 45;



const int DOWN_FRONT_LEFT_FIBULA = 0 + STANDING_OFFSET;
const int DOWN_FRONT_RIGHT_FIBULA = 180 - STANDING_OFFSET;
const int DOWN_BACK_LEFT_FIBULA = 180 - STANDING_OFFSET;
const int DOWN_BACK_RIGHT_FIBULA = 0 + STANDING_OFFSET;


const int UP_FRONT_LEFT_FIBULA = 180;
const int UP_FRONT_RIGHT_FIBULA = 0;
const int UP_BACK_LEFT_FIBULA = 0;
const int UP_BACK_RIGHT_FIBULA = 180;

const int POINT_FIBULA = 90;


const int CROSS_FRONT_LEFT_FEMUR = 45;
const int CROSS_FRONT_RIGHT_FEMUR = 135;
const int CROSS_BACK_LEFT_FEMUR = 135;
const int CROSS_BACK_RIGHT_FEMUR = 45;
// #ifdef PIOLO
// const int FORWARD_FRONT_LEFT_FEMUR = 135;
// const int FORWARD_FRONT_RIGHT_FEMUR = 135;
// const int FORWARD_BACK_LEFT_FEMUR = 45;
// const int FORWARD_BACK_RIGHT_FEMUR = 135;

// const int BACKWARD_FRONT_LEFT_FEMUR = 45;
// const int BACKWARD_FRONT_RIGHT_FEMUR = 45;
// const int BACKWARD_BACK_LEFT_FEMUR = 135;
// const int BACKWARD_BACK_RIGHT_FEMUR = 45;
// #endif
// #ifdef DAVIN
const int FORWARD_FRONT_LEFT_FEMUR = 45;
const int FORWARD_FRONT_RIGHT_FEMUR = 135;
const int FORWARD_BACK_LEFT_FEMUR = 45;
const int FORWARD_BACK_RIGHT_FEMUR = 135;

const int BACKWARD_FRONT_LEFT_FEMUR = 135;
const int BACKWARD_FRONT_RIGHT_FEMUR = 45;
const int BACKWARD_BACK_LEFT_FEMUR = 135;
const int BACKWARD_BACK_RIGHT_FEMUR = 45;
// #endif

const int CENTER_FRONT_LEFT_FEMUR = 90;
const int CENTER_FRONT_RIGHT_FEMUR = 90;
const int CENTER_BACK_LEFT_FEMUR = 90;
const int CENTER_BACK_RIGHT_FEMUR = 90;


const int CENTER_FEMUR = 90;






/*

@fileinfo


creating descrete movements
for left fibulas, 180 is up, and 0 is down
for right fibulas, 0 is up, and 180 is down


descrete values: 0, 30, 45, 60, 90, 120, 150, 180
movement for left fibulas {
setTargetAngle (x) {
  switch (x) {
    case 1: targetAngle = 0;
    case 2: targetAngle = 30;
    case 3: targetAngle = 45;
    case 4: targetAngle = 60;
    case 5: targetAngle = 90;
    case 6: targetAngle = 120;
    case 7: targetAngle = 150;
    case 8: targetAngle = 180;}
  }
}

movement for right fibulas {
setTargetAngle (x) {
switch (x) {
  case 1: targetAngle = 180;
  case 2: targetAngle = 150;
  case 3: targetAngle = 120;
  case 4: targetAngle = 90;
  case 5: targetAngle = 60;
  case 6: targetAngle = 45;
  case 7: targetAngle = 30;
  case 8: targetAngle = 0;}
}
}
for left femurs, 0 is down and 180 is up
for right femurs, 180 is down and 0 is up

movements for left femurs {
setTargetAngle (x) {
switch (x) {
  case 1: targetAngle = 0;
  case 2: targetAngle = 30;
  case 3: targetAngle = 45;
  case 4: targetAngle = 60;
  case 5: targetAngle = 90;
  case 6: targetAngle = 120;
  case 7: targetAngle = 150;
  case 8: targetAngle = 180;}
}
}
movements for right femurs {
setTargetAngle (x) {
switch (x) {
  case 1: targetAngle = 180;
  case 2: targetAngle = 150;
  case 3: targetAngle = 120;
  case 4: targetAngle = 90;
  case 5: targetAngle = 60;
  case 6: targetAngle = 45;
  case 7: targetAngle = 30;
  case 8: targetAngle = 0;}
}
}

*/



// Initialize PCA9685 PWM controller
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Quadruped with 8 servos (2 per limb)
// Servo indices: FL_femur=0, FL_fibula=1, FR_femur=2, FR_fibula=3, 
//                BL_femur=4, BL_fibula=5, BR_femur=6, BR_fibula=7



// Manual control settings
constexpr int JOINT_STEP = 5;  // degrees per keypress
constexpr int MIN_ANGLE = 0;
constexpr int MAX_ANGLE = 180;

struct JointAngles {
  int fl_femur = 45;
  int fr_femur = 135;
  int bl_femur = 45;
  int br_femur = 135;
  
  int fl_fibula = 180;
  int fr_fibula = 0;
  int bl_fibula = 180;
  int br_fibula = 0;
} joints;


bool autoWalk = false;
unsigned long autoWalkLastStep = 0;
const unsigned long AUTO_WALK_STEP_MS = 500;
int autoWalkPhase = 0;

void applyJointAngles() {
  robot.frontLeft.setTarget(joints.fl_femur, joints.fl_fibula);
  robot.frontRight.setTarget(joints.fr_femur, joints.fr_fibula);
  robot.backLeft.setTarget(joints.bl_femur, joints.bl_fibula);
  robot.backRight.setTarget(joints.br_femur, joints.br_fibula);
}



void move() {
     while (
       (robot.frontLeft.fibula.currentAngle != joints.fl_fibula) ||
       (robot.backLeft.fibula.currentAngle != joints.bl_fibula) ||
       (robot.backRight.fibula.currentAngle != joints.br_fibula) ||
       (robot.frontRight.fibula.currentAngle != joints.fr_fibula) ||
       (robot.frontLeft.femur.currentAngle != joints.fl_femur) ||
       (robot.backLeft.femur.currentAngle != joints.bl_femur) ||
       (robot.backRight.femur.currentAngle != joints.br_femur) ||
       (robot.frontRight.femur.currentAngle != joints.fr_femur)
     ) {
       robot.update();
       delay(5);
     }
}


void ready() {
    joints.fl_femur = FORWARD_FRONT_LEFT_FEMUR;;
    joints.fr_femur = FORWARD_FRONT_RIGHT_FEMUR;
    joints.bl_femur = BACKWARD_BACK_LEFT_FEMUR;
    joints.br_femur = BACKWARD_BACK_RIGHT_FEMUR;

    joints.fl_fibula = 0;
    joints.fr_fibula = 180;
    joints.bl_fibula = 180;
    joints.br_fibula = 0;
}


void fibulaStand() {
    joints.fl_fibula = 180;
    joints.fr_fibula = 0;
    joints.bl_fibula = 180;
    joints.br_fibula = 0;
}

void calibrate() {
  joints.fl_femur = 90;
  joints.fr_femur = 90;
  joints.bl_femur = 90;
  joints.br_femur = 90;
  applyJointAngles();
  move();

  joints.fl_fibula = POINT_FIBULA - 45;
  joints.fr_fibula = POINT_FIBULA + 45;
  joints.bl_fibula = POINT_FIBULA + 45;
  joints.br_fibula = POINT_FIBULA - 45;
  applyJointAngles();
  move();


  joints.fl_femur = FORWARD_FRONT_LEFT_FEMUR;
  joints.fr_femur = FORWARD_FRONT_RIGHT_FEMUR;
  joints.bl_femur = BACKWARD_BACK_LEFT_FEMUR;
  joints.br_femur = BACKWARD_BACK_RIGHT_FEMUR;
  applyJointAngles();
  move();

  joints.fl_fibula = 0;
  joints.fr_fibula = 180;
  joints.bl_fibula = 180;
  joints.br_fibula = 0;
  applyJointAngles();
  move();
}

void pushup () {

  // const int DOWN_FRONT_LEFT_FIBULA = 0;
  // const int DOWN_FRONT_RIGHT_FIBULA = 180;
  // const int DOWN_BACK_LEFT_FIBULA = 180;
  // const int DOWN_BACK_RIGHT_FIBULA = 0;
  joints.fl_fibula = POINT_FIBULA - 45;
  joints.fr_fibula = POINT_FIBULA + 45;
  joints.bl_fibula = POINT_FIBULA + 45;
  joints.br_fibula = POINT_FIBULA - 45;
  applyJointAngles(); move();
  joints.fl_fibula = DOWN_FRONT_LEFT_FIBULA;
  joints.fr_fibula = DOWN_FRONT_RIGHT_FIBULA;
  joints.bl_fibula = DOWN_BACK_LEFT_FIBULA;
  joints.br_fibula = DOWN_BACK_RIGHT_FIBULA;
  applyJointAngles(); move();
}


void kembot() {
  joints.bl_femur = 90; 
  joints.fl_femur = 90; 
  joints.br_femur = 90; 
  joints.fr_femur = 90; 
  applyJointAngles(); move();
  delay(200);
  joints.fl_fibula = POINT_FIBULA - 45;
  joints.bl_fibula = POINT_FIBULA + 45;
  applyJointAngles(); move();
  delay(500);
  joints.fl_fibula = DOWN_FRONT_LEFT_FIBULA;
  joints.bl_fibula = DOWN_BACK_LEFT_FIBULA;
  applyJointAngles(); move();

  joints.br_fibula = POINT_FIBULA - 45;
  joints.fr_fibula = POINT_FIBULA + 45;
  applyJointAngles(); move();
  delay(500);

  joints.br_fibula = DOWN_BACK_RIGHT_FIBULA;
  joints.fr_fibula = DOWN_FRONT_RIGHT_FIBULA;
  applyJointAngles(); move();
}
void dog() {
  joints.bl_fibula = LIFT_BACK_LEFT_FIBULA;
  joints.br_fibula = LIFT_BACK_RIGHT_FIBULA;
  applyJointAngles(); move();
}

void chaos() {
  dog();
  delay(200);
  joints.bl_fibula = DOWN_BACK_LEFT_FIBULA;
  joints.br_fibula = DOWN_BACK_RIGHT_FIBULA;
  applyJointAngles(); move();
  delay(200);
  applyJointAngles(); move();
  dog();
  delay(200);
  joints.bl_fibula = DOWN_BACK_LEFT_FIBULA;
  joints.br_fibula = DOWN_BACK_RIGHT_FIBULA;
  applyJointAngles(); move();
  delay(200);
  dog();
  delay(200);
  joints.bl_fibula = DOWN_BACK_LEFT_FIBULA;
  joints.br_fibula = DOWN_BACK_RIGHT_FIBULA;
  applyJointAngles(); move();
  delay(200);
  
}
void dogbot() {
  dog();
  delay(500);
  joints.fl_femur = CENTER_FRONT_LEFT_FEMUR;
  applyJointAngles(); move();
  delay(500);
  joints.fl_femur = FORWARD_FRONT_LEFT_FEMUR;
  joints.fr_femur = CENTER_FRONT_RIGHT_FEMUR;
  applyJointAngles(); move();
  delay(500);
  joints.fr_femur = FORWARD_FRONT_RIGHT_FEMUR;
  applyJointAngles(); move();


}
void greet() {
  dog();
  delay(500);
  joints.fr_fibula = UP_FRONT_RIGHT_FIBULA;
  applyJointAngles(); move();
  delay(500);
  joints.fr_fibula = DOWN_FRONT_RIGHT_FIBULA;
  applyJointAngles(); move();
  delay(500);
  joints.fl_fibula = UP_FRONT_LEFT_FIBULA;
  applyJointAngles(); move();
  delay(500);
  joints.fl_fibula = DOWN_FRONT_LEFT_FIBULA;
  applyJointAngles(); move();
}


void gait() {

  // 1ST STEP
  // BL && FR
  joints.bl_fibula = LIFT_BACK_LEFT_FIBULA;
  joints.fr_fibula = LIFT_FRONT_RIGHT_FIBULA;


  joints.bl_femur = FORWARD_BACK_LEFT_FEMUR;
  joints.fr_femur = FORWARD_FRONT_RIGHT_FEMUR;

  applyJointAngles();
  move();

  // SECOND STEP
  // BL && FR
  joints.bl_fibula = DOWN_BACK_LEFT_FIBULA;
  joints.fr_fibula = DOWN_FRONT_RIGHT_FIBULA;
  applyJointAngles();
  move();
  // THIRD STEP
  // BL && FR
  joints.bl_femur = BACKWARD_BACK_LEFT_FEMUR;
  joints.fr_femur = BACKWARD_FRONT_RIGHT_FEMUR;

  // BR && FL
  joints.br_fibula = LIFT_BACK_RIGHT_FIBULA;
  joints.fl_fibula = LIFT_FRONT_LEFT_FIBULA;
  joints.br_femur = FORWARD_BACK_RIGHT_FEMUR;
  // joints.fl_femur = BACKWARD_FRONT_LEFT_FEMUR;

  applyJointAngles();
  move();

  joints.br_fibula = DOWN_BACK_RIGHT_FIBULA; // now with other side foot
  joints.fl_fibula = DOWN_FRONT_LEFT_FIBULA;
  applyJointAngles();
  move();

  joints.fr_femur = FORWARD_FRONT_RIGHT_FEMUR;
  joints.fr_fibula = LIFT_FRONT_RIGHT_FIBULA;
  applyJointAngles();
  move();

  joints.fr_fibula = DOWN_FRONT_RIGHT_FIBULA;
  applyJointAngles();
  move();
  joints.fl_fibula = LIFT_FRONT_LEFT_FIBULA;
  joints.br_fibula = LIFT_BACK_RIGHT_FIBULA;
  applyJointAngles();
  move();
  joints.fl_fibula = DOWN_FRONT_LEFT_FIBULA;
  joints.br_fibula = DOWN_BACK_RIGHT_FIBULA;
  applyJointAngles();
  move();



  joints.fl_femur = BACKWARD_FRONT_LEFT_FEMUR;
  joints.br_femur = BACKWARD_BACK_RIGHT_FEMUR;
  joints.bl_femur = FORWARD_BACK_LEFT_FEMUR;
  joints.bl_fibula = LIFT_BACK_LEFT_FIBULA;
  applyJointAngles();
  move();

  joints.bl_fibula = DOWN_BACK_LEFT_FIBULA;
  applyJointAngles();
  move();
  joints.fl_fibula = LIFT_FRONT_LEFT_FIBULA;
  joints.fl_femur = FORWARD_FRONT_LEFT_FEMUR;
  applyJointAngles();
  move();
  joints.fl_fibula = DOWN_FRONT_LEFT_FIBULA;
  applyJointAngles();
  move();


  // joints.fl_femur = FORWARD_FRONT_LEFT_FEMUR;
  // joints.fl_fibula = LIFT_FRONT_LEFT_FIBULA;
  
  // joints.bl_fibula = DOWN_BACK_LEFT_FIBULA;
  // joints.fr_fibula = LIFT_FRONT_RIGHT_FIBULA;
  // applyJointAngles();
  // move();
  
  // joints.fl_fibula = DOWN_FRONT_LEFT_FIBULA;
  // joints.fr_fibula = DOWN_FRONT_RIGHT_FIBULA;
  // applyJointAngles();
  // move();


    // BR && FL
    // joints.fl_fibula = 135;
    // joints.br_fibula = 45;

    /*
    joints.fl_femur = FORWARD_FRONT_LEFT_FEMUR;
    joints.fl_fibula = LIFT_FRONT_LEFT_FIBULA;
    applyJointAngles();
    move();
    joints.fl_fibula = DOWN_FRONT_LEFT_FIBULA;
    applyJointAngles();
    move();
    */


}

void walks() {
    ready();
    applyJointAngles();
    // left foot
    joints.bl_fibula = 135; // point 45 degree upward (z axis)
    joints.bl_femur = 45; // point upward 135 degree ccw (x axis)
    applyJointAngles();
    joints.bl_fibula = 180; // point down
    applyJointAngles();

    // left arm
    joints.fl_fibula = 135; // point 45 degree upward (z axis)
    joints.fl_femur = 45; // point upward 135 degree ccw (x axis)

    // right foot
    joints.br_fibula = 45; // point 45 degree upward (z axis)
    joints.fr_femur = 135; 

    // left foot 
    joints.bl_fibula = 180; // point down
    joints.bl_femur = 135; // push
    applyJointAngles();
    fibulaStand();

    
    joints.fl_femur = leftFemurTarget(8);
    joints.fr_femur = rightFemurTarget(8);
    joints.bl_femur = rightFemurTarget(0);
    joints.br_femur = leftFemurTarget(0);
    applyJointAngles();

    joints.fl_femur = leftFemurTarget(0);
    joints.fr_femur = rightFemurTarget(0);
    joints.bl_femur = rightFemurTarget(0);
    joints.br_femur = leftFemurTarget(0);
    applyJointAngles();

}


void initialPosition() {
  ready();
  // applyJointAngles();
  // move();
  
}


void splitPosition() {
  ready();

  joints.fl_fibula = 90;
  joints.fr_fibula = 90;
  joints.bl_fibula = 90;
  joints.br_fibula = 90;
  
}



/* 

void walk () {
  
  // step 1: lift hands
  joints.fl_fibula = 45;
  joints.br_fibula = 135;
  applyJointAngles();
  while ((robot.frontLeft.fibula.currentAngle != joints.fl_fibula) || (robot.backRight.fibula.currentAngle != joints.br_fibula)) {
    robot.update();
    if (isInterrupted()) return;
  }

  // step 2 move femurs

  joints.fl_femur = 180; 
  joints.br_femur = 90; 

  applyJointAngles();
  while ((robot.frontLeft.femur.currentAngle != joints.fl_femur) ||
         (robot.backRight.femur.currentAngle != joints.br_femur) ) {
    robot.update();
    if (isInterrupted()) return;
  }
  
  // step 3: move other legs forward
  joints.bl_femur = 0;
  joints.bl_fibula = 45;
  joints.fl_fibula = 90;
  applyJointAngles();
  while (robot.backLeft.femur.currentAngle != joints.bl_femur || robot.backLeft.fibula.currentAngle != joints.bl_fibula) {
    robot.update();
    if (isInterrupted()) return;
  }

  // 4
  joints.fl_fibula = 90;
  joints.br_fibula = 180;
  applyJointAngles();
  while ((robot.frontLeft.fibula.currentAngle != joints.fl_fibula) || (robot.backRight.fibula.currentAngle != joints.br_fibula)) {
    robot.update();
    if (isInterrupted()) return;
  }

  
  // 5
  standUp();

  // 6
  joints.fr_fibula = 45;
  joints.bl_fibula = 45;
  applyJointAngles();
  while ((robot.frontRight.fibula.currentAngle != joints.fr_fibula) || (robot.backLeft.fibula.currentAngle != joints.bl_fibula)) {
    robot.update();
    if (isInterrupted()) return;
  }
  // 7

  joints.fr_femur = 0; 
  joints.bl_femur = 90; 

  applyJointAngles();
  while ((robot.frontRight.femur.currentAngle != joints.fr_femur) ||
         (robot.backLeft.femur.currentAngle != joints.bl_femur) ) {
    robot.update();
    if (isInterrupted()) return;
  }
  // 8 

  joints.br_femur = 180;
  joints.br_fibula = 135;
  joints.fr_fibula = 0;
  applyJointAngles();
  while (robot.backRight.femur.currentAngle != joints.br_femur || robot.backRight.fibula.currentAngle != joints.br_fibula) {
    robot.update();
    if (isInterrupted()) return;
  }
  // 9
  joints.fr_fibula = 0;
  joints.bl_fibula = 0;
  applyJointAngles();
  while ((robot.frontRight.fibula.currentAngle != joints.fr_fibula) || (robot.backLeft.fibula.currentAngle != joints.bl_fibula)) {
    robot.update();
    if (isInterrupted()) return;
  }

  

  standUp();
  

}


*/
void standUp() {
  joints.fl_fibula = DOWN_FRONT_LEFT_FIBULA;
  joints.fr_fibula = DOWN_FRONT_RIGHT_FIBULA;
  joints.bl_fibula = DOWN_BACK_LEFT_FIBULA;
  joints.br_fibula = DOWN_BACK_RIGHT_FIBULA;
  joints.fl_femur = FORWARD_FRONT_LEFT_FEMUR;
  joints.fr_femur = FORWARD_FRONT_RIGHT_FEMUR;
  joints.bl_femur = BACKWARD_BACK_LEFT_FEMUR;
  joints.br_femur = BACKWARD_BACK_RIGHT_FEMUR;
  applyJointAngles(); move();
}

void attack() {
  robot.setWeaponTarget(60);
  while (robot.weapon.currentAngle != robot.weapon.targetAngle) {
    robot.update();
  }

  delay(120);

  robot.setWeaponTarget(0);
  while (robot.weapon.currentAngle != robot.weapon.targetAngle) {
    robot.update();
  }

  delay(120);

  robot.setWeaponTarget(90);
  while (robot.weapon.currentAngle != robot.weapon.targetAngle) {
    robot.update();
  }
}



void walk() {
  Serial.println(robot.idle);

  // 1 — Lift FL + BR, move FR backward
  joints.fl_fibula = LIFT_FRONT_LEFT_FIBULA;
  joints.br_fibula = LIFT_BACK_RIGHT_FIBULA;
  applyJointAngles();
  move();
  joints.fr_femur  = BACKWARD_FRONT_RIGHT_FEMUR;
  applyJointAngles();
  move();

  // 2 — Swing FL + BR forward
  joints.fl_femur = FORWARD_FRONT_LEFT_FEMUR;
  joints.br_femur = FORWARD_BACK_RIGHT_FEMUR;
  applyJointAngles();
  move();

  // 3 — Lift BL + FR, lower FL + BR
  joints.bl_fibula = LIFT_BACK_LEFT_FIBULA;
  joints.fr_fibula = LIFT_FRONT_RIGHT_FIBULA;
  joints.fl_fibula = DOWN_FRONT_LEFT_FIBULA + 15;
  joints.br_fibula = DOWN_BACK_RIGHT_FIBULA + 35;
  applyJointAngles();
  move();
  // Mirror femur positions
  joints.fl_femur = BACKWARD_FRONT_LEFT_FEMUR;
  joints.br_femur = BACKWARD_BACK_RIGHT_FEMUR;
  joints.bl_femur = FORWARD_BACK_LEFT_FEMUR;
  joints.fr_femur = FORWARD_FRONT_RIGHT_FEMUR;

  applyJointAngles();
  move();

  // 4 — Lower BL + FR, lift FL + BR
  joints.bl_fibula = DOWN_BACK_LEFT_FIBULA - 35;
  joints.fr_fibula = DOWN_FRONT_RIGHT_FIBULA - 15;
  joints.fl_fibula = LIFT_FRONT_LEFT_FIBULA;
  joints.br_fibula = LIFT_BACK_RIGHT_FIBULA;
  applyJointAngles();
  move();
  // Mirror femur positions
  joints.fl_femur = FORWARD_FRONT_LEFT_FEMUR;
  joints.br_femur = FORWARD_BACK_RIGHT_FEMUR;
  joints.bl_femur = BACKWARD_BACK_LEFT_FEMUR;
  joints.fr_femur = BACKWARD_FRONT_RIGHT_FEMUR;

  applyJointAngles();
  move();

  robot.idle = true;
  Serial.println(robot.idle);
}



void turnLeft_() {
  Serial.println(robot.idle);

  // 1 — Lift FL + BR, move FR backward
  joints.fl_fibula = LIFT_FRONT_LEFT_FIBULA;
  joints.br_fibula = LIFT_BACK_RIGHT_FIBULA;
  joints.fr_femur  = BACKWARD_FRONT_RIGHT_FEMUR;
  applyJointAngles();
  move();

  // 2 — Swing FL + BR forward
  joints.fl_femur = FORWARD_FRONT_LEFT_FEMUR;
  joints.br_femur = FORWARD_BACK_RIGHT_FEMUR;
  applyJointAngles();
  move();

  // 3 — Lift BL + FR, lower FL + BR
  joints.bl_fibula = LIFT_BACK_LEFT_FIBULA;
  joints.fr_fibula = LIFT_FRONT_RIGHT_FIBULA;
  joints.fl_fibula = DOWN_FRONT_LEFT_FIBULA;
  joints.br_fibula = DOWN_BACK_RIGHT_FIBULA;

  // Mirror femur positions
  joints.fl_femur = BACKWARD_FRONT_LEFT_FEMUR;
  joints.br_femur = BACKWARD_BACK_RIGHT_FEMUR;
  joints.bl_femur = FORWARD_BACK_LEFT_FEMUR;
  joints.fr_femur = FORWARD_FRONT_RIGHT_FEMUR;

  applyJointAngles();
  move();
  standUp();
  applyJointAngles();
  move();
}




void turnRight() {
  const static int TURNING_OFFSET = 20;
  Serial.println(robot.idle);

  // 1 — Lift FL + BR, move FR backward
  // joints.fl_fibula = LIFT_FRONT_LEFT_FIBULA;
  // joints.br_fibula = LIFT_BACK_RIGHT_FIBULA;
  // applyJointAngles();
  // move();

  // 2 — Swing FL + BR forward
  // joints.fl_femur = FORWARD_FRONT_LEFT_FEMUR - TURNING_OFFSET;
  // joints.br_femur = BACKWARD_BACK_RIGHT_FEMUR - TURNING_OFFSET;
  // applyJointAngles();
  // move();
  joints.fr_fibula = LIFT_FRONT_RIGHT_FIBULA;
  joints.bl_fibula = LIFT_BACK_LEFT_FIBULA;
  applyJointAngles(); move();

  // joints.fl_fibula = DOWN_FRONT_LEFT_FIBULA;
  // joints.br_fibula = DOWN_BACK_RIGHT_FIBULA;
  // applyJointAngles();
  // move();

  // joints.fr_fibula = LIFT_FRONT_RIGHT_FIBULA;
  // joints.bl_fibula = LIFT_BACK_LEFT_FIBULA;
  // applyJointAngles();
  // move();
  joints.fl_femur = CENTER_FRONT_LEFT_FEMUR;
  joints.br_femur = CENTER_BACK_RIGHT_FEMUR;
  applyJointAngles();
  move();
  joints.fr_femur = CENTER_FRONT_RIGHT_FEMUR;
  joints.bl_femur = CENTER_BACK_LEFT_FEMUR;
  applyJointAngles();
  move();
  joints.fr_fibula = DOWN_FRONT_RIGHT_FIBULA;
  joints.bl_fibula = DOWN_BACK_LEFT_FIBULA;
  applyJointAngles();
  move();
  joints.fl_fibula = LIFT_FRONT_LEFT_FIBULA;
  joints.br_fibula = LIFT_BACK_RIGHT_FIBULA;
  applyJointAngles();
  move();
  joints.fl_femur = FORWARD_FRONT_LEFT_FEMUR;
  joints.br_femur = BACKWARD_BACK_RIGHT_FEMUR;
  applyJointAngles();
  move();
  standUp();
  robot.idle = true;
  Serial.println(robot.idle);
  // standUp();
}

void reverseWalk() {
  Serial.println(robot.idle);

  // 1 — Lift FL + BR, move FR forward (reverse of the forward step)
  joints.fl_fibula = LIFT_FRONT_LEFT_FIBULA;
  joints.br_fibula = LIFT_BACK_RIGHT_FIBULA;
  joints.fr_femur = FORWARD_FRONT_RIGHT_FEMUR;
  applyJointAngles();
  move();

  // 2 — Swing FL + BR backward
  joints.fl_femur = BACKWARD_FRONT_LEFT_FEMUR;
  joints.br_femur = BACKWARD_BACK_RIGHT_FEMUR;
  applyJointAngles();
  move();

  // 3 — Lift BL + FR, lower FL + BR
  joints.bl_fibula = LIFT_BACK_LEFT_FIBULA;
  joints.fr_fibula = LIFT_FRONT_RIGHT_FIBULA;
  joints.fl_fibula = DOWN_FRONT_LEFT_FIBULA + 15;
  joints.br_fibula = DOWN_BACK_RIGHT_FIBULA + 35;
  applyJointAngles();
  move();

  // Mirror femur positions (reverse of forward walk)
  joints.fl_femur = FORWARD_FRONT_LEFT_FEMUR;
  joints.br_femur = FORWARD_BACK_RIGHT_FEMUR;
  joints.bl_femur = BACKWARD_BACK_LEFT_FEMUR;
  joints.fr_femur = BACKWARD_FRONT_RIGHT_FEMUR;

  applyJointAngles();
  move();

  // 4 — Lower BL + FR, lift FL + BR
  joints.bl_fibula = DOWN_BACK_LEFT_FIBULA - 35;
  joints.fr_fibula = DOWN_FRONT_RIGHT_FIBULA - 15;
  joints.fl_fibula = LIFT_FRONT_LEFT_FIBULA;
  joints.br_fibula = LIFT_BACK_RIGHT_FIBULA;
  applyJointAngles();
  move();

  // Mirror femur positions (reverse of forward walk)
  joints.fl_femur = BACKWARD_FRONT_LEFT_FEMUR;
  joints.br_femur = BACKWARD_BACK_RIGHT_FEMUR;
  joints.bl_femur = FORWARD_BACK_LEFT_FEMUR;
  joints.fr_femur = FORWARD_FRONT_RIGHT_FEMUR;

  applyJointAngles();
  move();

  robot.idle = true;
  Serial.println(robot.idle);
}

// ...existing code...

// ...existing code...

void turnLeft() {
  const static int TURNING_OFFSET = 20;
  Serial.println(robot.idle);

  // 1 — Lift FL + BR, move FR backward
  joints.fl_fibula = LIFT_FRONT_LEFT_FIBULA;
  joints.br_fibula = LIFT_BACK_RIGHT_FIBULA;
  applyJointAngles();
  move();

  // 2 — Center the opposite side
  joints.fr_femur = CENTER_FRONT_RIGHT_FEMUR;
  joints.bl_femur = CENTER_BACK_LEFT_FEMUR;
  applyJointAngles();
  move();

  // 3 — Center the lifted side
  joints.fl_femur = CENTER_FRONT_LEFT_FEMUR;
  joints.br_femur = CENTER_BACK_RIGHT_FEMUR;
  applyJointAngles();
  move();

  // 4 — Put FL + BR down
  joints.fl_fibula = DOWN_FRONT_LEFT_FIBULA;
  joints.br_fibula = DOWN_BACK_RIGHT_FIBULA;
  applyJointAngles();
  move();

  // 5 — Lift FR + BL
  joints.fr_fibula = LIFT_FRONT_RIGHT_FIBULA;
  joints.bl_fibula = LIFT_BACK_LEFT_FIBULA;
  applyJointAngles();
  move();

  // 6 — Shift body for left turn
  joints.fr_femur = FORWARD_FRONT_RIGHT_FEMUR;
  joints.bl_femur = BACKWARD_BACK_LEFT_FEMUR;
  applyJointAngles();
  move();

  standUp();
  robot.idle = true;
  Serial.println(robot.idle);
}

// ...existing code...
// ...existing code...


void potangina() {
  Serial.println(robot.idle);

  // 1 — Lift FL + BR, move FR backward
  joints.fl_fibula = LIFT_FRONT_LEFT_FIBULA;
  joints.br_fibula = LIFT_BACK_RIGHT_FIBULA;
  applyJointAngles();
  move();
  joints.fl_femur  = FORWARD_FRONT_LEFT_FEMUR;
  joints.br_femur  = FORWARD_BACK_RIGHT_FEMUR;
  applyJointAngles();
  move();
  joints.fl_fibula = DOWN_FRONT_LEFT_FIBULA;
  joints.br_fibula = DOWN_BACK_RIGHT_FIBULA;
  applyJointAngles();
  move();
  joints.fr_fibula = LIFT_FRONT_RIGHT_FIBULA;
  joints.bl_fibula = LIFT_BACK_LEFT_FIBULA;
  applyJointAngles();
  move();
  joints.fl_femur  = BACKWARD_FRONT_LEFT_FEMUR;
  joints.br_femur  = BACKWARD_BACK_RIGHT_FEMUR;
  joints.fr_femur  = FORWARD_FRONT_RIGHT_FEMUR;
  joints.bl_femur  = FORWARD_BACK_LEFT_FEMUR;
  applyJointAngles();
  move();
  joints.fr_fibula = DOWN_FRONT_RIGHT_FIBULA;
  joints.bl_fibula = DOWN_BACK_LEFT_FIBULA;
  applyJointAngles();
  move();
  robot.idle = true;
  Serial.println(robot.idle);

}


#endif  // SEQUENCES_H