#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "legs.h"
#include "comm.h"
#include "espnow.h"
#include "sequences.h"
#include "oleddd.h"

Quadruped robot(0, 1, 2, 3, 4, 5, 6, 7, 8);
Controller controller(&robot);

void gait();
void ready();

bool reversed = false;

// ================= ESP-NOW =================
volatile char command = 0;
volatile bool commandReady = false;

unsigned long lastCommandTime = 0;
char lastCommand = 0;

// callback when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  if (len < 1) return;

  char newCmd = incomingData[0];
  unsigned long now = millis();

  bool isMotionCommand = (newCmd == 'w' || newCmd == 'W' || newCmd == 's' || newCmd == 'S' ||
                         newCmd == 'a' || newCmd == 'A' || newCmd == 'd' || newCmd == 'D');

  // Ignore duplicate packets that arrive too close together, but allow rapid repeats for motion input.
  if (newCmd == lastCommand && (now - lastCommandTime) < (isMotionCommand ? 30 : 250)) {
    return;
  }

  lastCommand = newCmd;
  lastCommandTime = now;

  command = newCmd;
  commandReady = true;
}

bool isInterrupted() {
  if (commandReady) {
    commandReady = false;
    return true;
  }
  return false;
}

void showActionAnimationForKey(char key) {
  switch (key) {
    case 'x':
    case 'X':
      showIdleFace();
      break;

    case 'q':
    case 'Q':
      showIdleFace();
      break;

    case 'w':
    case 'W':
      showTalkHappyFace();
      break;

    case 's':
    case 'S':
      showTalkSadFace();
      break;

    case 'a':
    case 'A':
      showTalkHappyFace();
      break;

    case 'd':
    case 'D':
      showTalkHappyFace();
      break;

    case '1':
      showTalkAngryFace();
      break;

    case '2':
      showTalkHappyFace();
      break;

    case '3':
      showHappyFace();
      break;

    case '4':
      showHappyFace();
      break;

    case '5':
      showAngryFace();
      break;

    default:
      showIdleFace();
      break;
  }
}

void handleCommand(char key) {
  Serial.println("Received command: " + String(key));

  switch (key) {
    case 'x':
    case 'X':
      standUp();
      break;

    case 'q':
    case 'Q':
      initialPosition();
      Serial.println("Stand");
      break;

    case 'w':
    case 'W':
      if (robot.idle) {
        robot.idle = false;
        if (reversed) {
          standUp();
          reversed = false;
        }
      }
      walk();
      Serial.println("Walk");
      break;

    case 's':
    case 'S':
      if (robot.idle) {
        if (!reversed) {
          standUp();
          reversed = true;
        }
      }
      robot.idle = false;
      Serial.println("Reverse Walk");
      reverseWalk();
      break;
    
    case 'a':
    case 'A':
      standUp();
      turnLeft();
      Serial.println("Turn Left");
      break;
    case 'd':
    case 'D':
      standUp();
      turnRight();
      Serial.println("Turn Right");
      break;

    case '1':
      attack();
      Serial.println("Attack");
      break;

    case 'c':
    case 'C':
      calibrate();
      break;

    case '2':
      kembot();
      break;

    case '3':
      greet();
      break;

    case '4':
      dogbot();
      break;

    case '5':
      chaos();
      break;

    default:
      break;
  }

  showActionAnimationForKey(key);
}

void setup() {

  Serial.begin(115200);
  delay(500);
  oled_init();
  delay(500);
  showSleepAnimation(120, 1);
  pwm.begin();
  pwm.setPWMFreq(60);

  robot.setWeaponTarget(90);
  standUp();

  receive_init();
  esp_now_register_recv_cb(OnDataRecv);

  Serial.println("\n=== BattleBot Ready ===");
}

void loop() {
  if (commandReady) {
    char key = command;
    commandReady = false;

    if (key >= 32) {
      handleCommand(key);
    }
  }

  oled_updateAnimation();
  robot.update();
}