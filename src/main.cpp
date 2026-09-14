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
unsigned long lastAcceptedCommandTime = 0;
char lastAcceptedCommand = 0;

bool isMotionCommandKey(char key) {
  return (key == 'w' || key == 'W' || key == 's' || key == 'S' ||
          key == 'a' || key == 'A' || key == 'd' || key == 'D');
}

// callback when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  if (len < 1) return;

  char newCmd = incomingData[0];
  unsigned long now = millis();

  const bool isMotion = isMotionCommandKey(newCmd);

  // Ignore duplicate packets that arrive too close together.
  // For motion inputs, allow the same key to repeat only after a short cooldown so the
  // robot can keep moving without getting stuck in a tiny input spam loop.
  if (newCmd == lastCommand && (now - lastCommandTime) < (isMotion ? 35 : 250)) {
    return;
  }

  // If a movement is still mid-step, we briefly reject new motion packets so we do not
  // stack repeated walk/turn commands during the illegal window, but still allow continued
  // spamming once that window has passed.
  if (isMotion && newCmd == lastAcceptedCommand && (now - lastAcceptedCommandTime) < 70) {
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
  const unsigned long now = millis();

  if (isMotionCommandKey(key) && key == lastAcceptedCommand && (now - lastAcceptedCommandTime) < 300) {
    Serial.println("Ignoring motion input during cooldown: " + String(key));
    return;
  }

  lastAcceptedCommand = key;
  lastAcceptedCommandTime = now;

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
    case 'k':
    case 'K':
      showSleepAnimation();
      Serial.println("Kill switch: powering down PCA9685 and OLED");
      u8g2.setPowerSave(1);
      pwm.sleep();
      delay(50);
      esp_deep_sleep_start();
      return;


    default:
      break;
  }

  showActionAnimationForKey(key);
}

void setup() {

  Serial.begin(115200);
  delay(500);

  pwm.begin();
  pwm.setPWMFreq(60);

  robot.setWeaponTarget(90);
  standUp();
  delay(1000);
  standUp();
  delay(500);

  oled_init();
  delay(500);
  showSleepAnimation(120, 1);
  

  receive_init();
  esp_now_register_recv_cb(OnDataRecv);

  Serial.println("\n=== BattleBot Ready ===");
  standUp();
  delay(1000);
  standUp();
  delay(500);
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