#ifndef LEGS_H
#define LEGS_H

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define SERVOMIN 170
#define SERVOMAX 650


// Safe femur range: avoid body collision at 0 and 180 degrees
constexpr int MIN_FEMUR_ANGLE = 40;
constexpr int MAX_FEMUR_ANGLE = 135;

// External PWM driver reference (defined in main.cpp)
extern Adafruit_PWMServoDriver pwm;

// Convert angle (0-180) to pulse width
int angleToPulse(int angle) {
  return map(angle, 0, 180, SERVOMIN, SERVOMAX);
}

inline int discreteAngle(int step) {
  static const int values[] = {0, 30, 45, 60, 90, 120, 135, 150, 180};
  constexpr int valueCount = sizeof(values) / sizeof(values[0]);

  if (step < 0) step = 0;
  if (step >= valueCount) step = valueCount - 1;

  Serial.println("discreteAngle: step=" + String(step) + ", angle=" + String(values[step]));
  return values[step];
}

inline int leftFemurTarget(int step) {
  // Left femurs: step 0 -> 180 degrees
  return discreteAngle(8 - step);
}

inline int rightFemurTarget(int step) {
  // Right femurs: step 0 -> 0 degrees
  return discreteAngle(step);
}

inline int leftFibulaTarget(int step) {
  // Left fibulas: step 0 -> 180 degrees, step 8 -> 0 degrees
  return discreteAngle(8 - step);
}

inline int rightFibulaTarget(int step) {
  // Right fibulas: step 0 -> first discrete angle
  return discreteAngle(step);
}

class Joint {
public:
  int servoIndex;
  int currentAngle = 0;
  int targetAngle = 0;
  unsigned long lastUpdate = 0;
  int stepDelay = 3;

  Joint(int index) {
    servoIndex = index;
  }

  virtual void setTarget(int angle) {
    targetAngle = angle;
  }

  void update() {
    unsigned long now = millis();

    if (now - lastUpdate >= stepDelay) {
      lastUpdate = now;

      if (currentAngle < targetAngle) {
        currentAngle++;
        pwm.setPWM(servoIndex, 0, angleToPulse(currentAngle));
      } 
      else if (currentAngle > targetAngle) {
        currentAngle--;
        pwm.setPWM(servoIndex, 0, angleToPulse(currentAngle));
      }
    }
  }
};


class Femur : public Joint {
public:
  Femur(int pin) : Joint(pin) {}

  void setTarget(int angle) override {
    if (angle < MIN_FEMUR_ANGLE) {
      angle = MIN_FEMUR_ANGLE;
    } else if (angle > MAX_FEMUR_ANGLE) {
      angle = MAX_FEMUR_ANGLE;
    }
    Joint::setTarget(angle);
  }
};

class Fibula : public Joint {
public:
  Fibula(int pin) : Joint(pin) {}
};


class Limb {
public:
  Femur femur;
  Fibula fibula;

  Limb(int femurPin, int fibulaPin)
    : femur(femurPin), fibula(fibulaPin) {}

  void setTarget(int femurAngle, int fibulaAngle) {
    femur.setTarget(femurAngle);
    fibula.setTarget(fibulaAngle);
  }

  void update() {
    femur.update();
    fibula.update();
  }
};

class Quadruped {
public:
  Limb frontLeft;
  Limb frontRight;
  Limb backLeft;
  Limb backRight;

  Quadruped(
    int fl_femur, int fl_fibula,
    int fr_femur, int fr_fibula,
    int bl_femur, int bl_fibula,
    int br_femur, int br_fibula
  )
    : frontLeft(fl_femur, fl_fibula),
      frontRight(fr_femur, fr_fibula),
      backLeft(bl_femur, bl_fibula),
      backRight(br_femur, br_fibula) {}

  void update() {
    frontLeft.update();
    frontRight.update();
    backLeft.update();
    backRight.update();
  }
};

#endif