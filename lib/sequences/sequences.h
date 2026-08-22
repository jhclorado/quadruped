#ifndef SEQUENCES_H
#define SEQUENCES_H
#include "legs.h"


const int LIFT_FRONT_LEFT_FIBULA = 45;
const int LIFT_FRONT_RIGHT_FIBULA = 135;
const int LIFT_BACK_LEFT_FIBULA = 135;
const int LIFT_BACK_RIGHT_FIBULA = 45;

const int DOWN_FRONT_LEFT_FIBULA = 0;
const int DOWN_FRONT_RIGHT_FIBULA = 180;
const int DOWN_BACK_LEFT_FIBULA = 180;
const int DOWN_BACK_RIGHT_FIBULA = 0;


const int UP_FRONT_LEFT_FIBULA = 180;
const int UP_FRONT_RIGHT_FIBULA = 0;
const int UP_BACK_LEFT_FIBULA = 0;
const int UP_BACK_RIGHT_FIBULA = 180;

const int POINT_FIBULA = 90;




const int CROSS_FRONT_LEFT_FEMUR = 45;
const int CROSS_FRONT_RIGHT_FEMUR = 135;
const int CROSS_BACK_LEFT_FEMUR = 135;
const int CROSS_BACK_RIGHT_FEMUR = 45;

const int FORWARD_FRONT_LEFT_FEMUR = 45;
const int FORWARD_FRONT_RIGHT_FEMUR = 135;
const int FORWARD_BACK_LEFT_FEMUR = 45;
const int FORWARD_BACK_RIGHT_FEMUR = 135;

const int BACKWARD_FRONT_LEFT_FEMUR = 135;
const int BACKWARD_FRONT_RIGHT_FEMUR = 45;
const int BACKWARD_BACK_LEFT_FEMUR = 135;
const int BACKWARD_BACK_RIGHT_FEMUR = 45;

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
Quadruped robot(0, 1, 2, 3, 4, 5, 6, 7);
Controller controller(&robot);

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
     }
}


void ready() {
    joints.fl_femur = 45;;
    joints.fr_femur = 135;
    joints.bl_femur = 135;
    joints.br_femur = 45;

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

void gait() {
    // BL && FR
    joints.bl_fibula = LIFT_BACK_LEFT_FIBULA;
    joints.fr_fibula = LIFT_FRONT_RIGHT_FIBULA;


    joints.bl_femur = FORWARD_BACK_LEFT_FEMUR;
    joints.fr_femur = FORWARD_FRONT_RIGHT_FEMUR;

    applyJointAngles();
    move();

    // BL && FR
    joints.bl_fibula = DOWN_BACK_LEFT_FIBULA;
    joints.fr_fibula = DOWN_FRONT_RIGHT_FIBULA;
    applyJointAngles();
    move();

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

    joints.br_fibula = DOWN_BACK_RIGHT_FIBULA;
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

    joints.fl_femur = BACKWARD_FRONT_LEFT_FEMUR;
    joints.br_femur = BACKWARD_BACK_RIGHT_FEMUR;
    joints.bl_femur = FORWARD_BACK_LEFT_FEMUR;
    joints.bl_fibula = LIFT_BACK_LEFT_FIBULA;
    applyJointAngles();
    move();

    joints.fl_femur = FORWARD_FRONT_LEFT_FEMUR;
    joints.fl_fibula = LIFT_FRONT_LEFT_FIBULA;
    
    joints.bl_fibula = DOWN_BACK_LEFT_FIBULA;
    applyJointAngles();
    move();
    
    joints.fl_fibula = DOWN_FRONT_LEFT_FIBULA;
    applyJointAngles();
    move();

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
void walk() {
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
  
}


void splitPosition() {
  ready();

  joints.fl_fibula = 90;
  joints.fr_fibula = 90;
  joints.bl_fibula = 90;
  joints.br_fibula = 90;
  
}

#endif  // SEQUENCES_H