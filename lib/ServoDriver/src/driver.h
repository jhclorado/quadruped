#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#define SERVOMIN 170
#define SERVOMAX 650

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

const int servoCount = 8; // Number of servos you have
const int servoPins[] = {8, 9, 10, 11, 4, 5, 6, 7}; // Define the PCA9685 pins for your servos

int servoAngles[servoCount]; // Array to store current servo angles

void setup()
{
  Serial.begin(9600);
  pwm.begin();
  pwm.setPWMFreq(60);
}

void moveServo(int servoIndex, int targetAngle)
{
  // Smoothly move the servo to the target angle
  for (int i = servoAngles[servoIndex]; i != targetAngle; i += (targetAngle > i) ? 1 : -1)
  {
    servoAngles[servoIndex] = i;
    pwm.setPWM(servoPins[servoIndex], 0, angleToPulse(i));
    delay(5); // Adjust the delay for smoother motion
  }
}


int angleToPulse(int angle)
{
  // Convert angle to servo pulse length
  return map(angle, 0, 180, SERVOMIN, SERVOMAX);
}