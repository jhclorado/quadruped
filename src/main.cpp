#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "legs.h"
#include "comm.h"
#include "espnow.h"
#include "sequences.h"


void gait();
void ready();

// ================= ESP-NOW =================
volatile char command = 0;
volatile bool commandReady = false;

// callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  if (len >= 1) {
    command = incomingData[0];
    commandReady = true;
  }
}


bool isInterrupted() {
  if (commandReady) {
    commandReady = false;
    return true;
  }
  return false;
}

void handleCommand(char key) {
  Serial.println("Received command: " + String(key));
  switch (key) {
    case 'x':
    case 'X':
      ready();
      applyJointAngles();
      move();

    case 'q':
    case 'Q':
      initialPosition();
      Serial.println("Stand");
      break;

    case 'w':
    case 'W':
      gait();
      Serial.println("Walk");
      break;

  //   case ' ':
  //     jump();
  //     Serial.println("Jump");
  //     break;
  }
}




void startAutoWalk() {
  delay(2000);
  Serial.println("Starting uncontrolled test walk...");
  autoWalk = true;
  autoWalkLastStep = millis();
  autoWalkPhase = 0;
}

void runAutoWalkStep() {
  if (!autoWalk) {
    return;
  }

  unsigned long now = millis();
  if (now - autoWalkLastStep < AUTO_WALK_STEP_MS) {
    return;
  }
  autoWalkLastStep = now;

  switch (autoWalkPhase) {
    case 0:
      robot.frontLeft.setTarget(130, 40);
      robot.backRight.setTarget(130, 40);
      robot.frontRight.setTarget(50, 80);
      robot.backLeft.setTarget(50, 80);
      break;

    case 1:
      robot.frontLeft.setTarget(100, 60);
      robot.backRight.setTarget(100, 60);
      robot.frontRight.setTarget(80, 60);
      robot.backLeft.setTarget(80, 60);
      break;

    case 2:
      robot.frontLeft.setTarget(70, 80);
      robot.backRight.setTarget(70, 80);
      robot.frontRight.setTarget(110, 40);
      robot.backLeft.setTarget(110, 40);
      break;

    case 3:
      robot.frontLeft.setTarget(90, 60);
      robot.backRight.setTarget(90, 60);
      robot.frontRight.setTarget(90, 60);
      robot.backLeft.setTarget(90, 60);
      break;
  }

  autoWalkPhase = (autoWalkPhase + 1) % 4;
}

void handleSerialInput(char key) {
  bool updated = false;
  
  switch(key) {
    case 'j':
    case 'J':
    case '0':  // Reset to initial position
      initialPosition();
      updated = true;
      Serial.println("Reset to initial position");
      break;
    case 'k':
    case 'K':
    case '9':  // Reset to split position
      gait();
      updated = true;
      Serial.println("Reset to split position");
      break;  
    // Front Left Femur
    case 'q':
    case 'Q':
    case '1':  // Front Left Femur +
      joints.fl_femur = min(joints.fl_femur + JOINT_STEP, MAX_ANGLE);
      updated = true;
      Serial.printf("FL Femur: %d\n", joints.fl_femur);
      break;
    case 'a':
    case 'A':
    case '!':  // Front Left Femur -
      joints.fl_femur = max(joints.fl_femur - JOINT_STEP, MIN_ANGLE);
      updated = true;
      Serial.printf("FL Femur: %d\n", joints.fl_femur);
      break;
    
    // Front Left Fibula
    case 'w':
    case 'W':
    case '2':  // Front Left Fibula +
      joints.fl_fibula = min(joints.fl_fibula + JOINT_STEP, MAX_ANGLE);
      updated = true;
      Serial.printf("FL Fibula: %d\n", joints.fl_fibula);
      break;
    case 's':
    case 'S':
    case '@':  // Front Left Fibula -
      joints.fl_fibula = max(joints.fl_fibula - JOINT_STEP, MIN_ANGLE);
      updated = true;
      Serial.printf("FL Fibula: %d\n", joints.fl_fibula);
      break;
    
    // Front Right Femur
    case 'e':
    case 'E':
    case '3':  // Front Right Femur +
      joints.fr_femur = min(joints.fr_femur + JOINT_STEP, MAX_ANGLE);
      updated = true;
      Serial.printf("FR Femur: %d\n", joints.fr_femur);
      break;
    case 'd':
    case 'D':
    case '#':  // Front Right Femur -
      joints.fr_femur = max(joints.fr_femur - JOINT_STEP, MIN_ANGLE);
      updated = true;
      Serial.printf("FR Femur: %d\n", joints.fr_femur);
      break;
    
    // Front Right Fibula
    case 'r':
    case 'R':
    case '4':  // Front Right Fibula +
      joints.fr_fibula = min(joints.fr_fibula + JOINT_STEP, MAX_ANGLE);
      updated = true;
      Serial.printf("FR Fibula: %d\n", joints.fr_fibula);
      break;
    case 'f':
    case 'F':
    case '$':  // Front Right Fibula -
      joints.fr_fibula = max(joints.fr_fibula - JOINT_STEP, MIN_ANGLE);
      updated = true;
      Serial.printf("FR Fibula: %d\n", joints.fr_fibula);
      break;
    
    // Back Left Femur
    case 'z':
    case 'Z':
    case '5':  // Back Left Femur +
      joints.bl_femur = min(joints.bl_femur + JOINT_STEP, MAX_ANGLE);
      updated = true;
      Serial.printf("BL Femur: %d\n", joints.bl_femur);
      break;
    case 'x':
    case 'X':
    case '%':  // Back Left Femur -
      joints.bl_femur = max(joints.bl_femur - JOINT_STEP, MIN_ANGLE);
      updated = true;
      Serial.printf("BL Femur: %d\n", joints.bl_femur);
      break;
    
    // Back Left Fibula
    case 'c':
    case 'C':
    case '6':  // Back Left Fibula +
      joints.bl_fibula = min(joints.bl_fibula + JOINT_STEP, MAX_ANGLE);
      updated = true;
      Serial.printf("BL Fibula: %d\n", joints.bl_fibula);
      break;
    case 'v':
    case 'V':
    case '^':  // Back Left Fibula -
      joints.bl_fibula = max(joints.bl_fibula - JOINT_STEP, MIN_ANGLE);
      updated = true;
      Serial.printf("BL Fibula: %d\n", joints.bl_fibula);
      break;
    
    // Back Right Femur
    case 't':
    case 'T':
    case '7':  // Back Right Femur +
      joints.br_femur = min(joints.br_femur + JOINT_STEP, MAX_ANGLE);
      updated = true;
      Serial.printf("BR Femur: %d\n", joints.br_femur);
      break;
    case 'g':
    case 'G':
    case '&':  // Back Right Femur -
      joints.br_femur = max(joints.br_femur - JOINT_STEP, MIN_ANGLE);
      updated = true;
      Serial.printf("BR Femur: %d\n", joints.br_femur);
      break;
    
    // Back Right Fibula
    case 'y':
    case 'Y':
    case '8':  // Back Right Fibula +
      joints.br_fibula = min(joints.br_fibula + JOINT_STEP, MAX_ANGLE);
      updated = true;
      Serial.printf("BR Fibula: %d\n", joints.br_fibula);
      break;
    case 'h':
    case 'H':
    case '*':  // Back Right Fibula -
      joints.br_fibula = max(joints.br_fibula - JOINT_STEP, MIN_ANGLE);
      updated = true;
      Serial.printf("BR Fibula: %d\n", joints.br_fibula);
      break;
    
    case '?':  // Help
      Serial.println("\n=== Manual Control ===");
      Serial.println("Each joint has 3 key options:");
      Serial.println("\nFront Left:  q/1(+) a/!(−) femur | w/2(+) s/@(−) fibula");
      Serial.println("Front Right: e/3(+) d/#(−) femur | r/4(+) f/$(−) fibula");
      Serial.println("Back Left:   z/5(+) x/%(−) femur | c/6(+) v/^(−) fibula");
      Serial.println("Back Right:  t/7(+) g/&(−) femur | y/8(+) h/*(−) fibula");
      Serial.println("\n? = Help\n");
      break;
  }

  
  if (updated) {
    applyJointAngles();
  }
}



void setup() {
  Serial.begin(115200);
  delay(500);  // Wait for serial to stabilize
  
  // Initialize PCA9685
  pwm.begin();
  pwm.setPWMFreq(60);
  
  // Initialize joint angles
  ready();
  applyJointAngles();
  
  // Initialize ESP-NOW receiver
  receive_init();
  esp_now_register_recv_cb(OnDataRecv);
  
  Serial.println("\n=== BattleBot Ready ===");
  Serial.println("Type '?' for control help");

  // startAutoWalk();
}

// void loop() {
//   if (autoWalk) {
//     runAutoWalkStep();
//   } else {
//     // Check for serial input
//     if (Serial.available()) {
//       char key = Serial.read();
//       if (key >= 32) {  // Only process printable characters
//         handleSerialInput(key);
//       }
//     }
    
//     // Check for new ESP-NOW messages
//     char msg[250];
//     uint8_t sender[6];
//     if (get_latest_message(msg, sizeof(msg), sender)) {
//       controller.handlePacket((uint8_t*)msg, strlen(msg));
//     }
//   }
  
//   robot.update();
// }


void loop() {

  if (commandReady) {
    commandReady = false;

    if (command >= 32) {
      handleCommand(command);
    }
  }

  robot.update();
}