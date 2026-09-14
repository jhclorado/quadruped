#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "legs.h"
#include "comm.h"
#include "espnow.h"
#include "sequences.h"
#include "oleddd.h"

Quadruped robot(0, 1, 2, 3, 4, 5, 6, 7);
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

  // Ignore duplicate packets that arrive too close together
  if (newCmd == lastCommand && (now - lastCommandTime) < 250) {
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

void handleCommand(char key) {
  Serial.println("Received command: " + String(key));

  switch (key) {
    case 'x':
    case 'X':
      standUp();
      showIdleAnimation(120, 1);
      break;

    case 'q':
    case 'Q':
      initialPosition();
      Serial.println("Stand");
      break;

    case 'w':
    case 'W':
      Serial.println(robot.idle);
      if (robot.idle) {
        robot.idle = false;
        if (reversed) {
          standUp();
          reversed = false;
        }
        Serial.println(robot.idle);
        walk();
        showAngryAnimation(120, 1);
        Serial.println("Walk");
      }
      break;

    case 's':
    case 'S':
      Serial.println(robot.idle);
      if (robot.idle) {
        if (!reversed) {
          standUp();
          reversed = true;
        }
        Serial.println("Reverse Walk");
        robot.idle = false;
        Serial.println(robot.idle);
        reverseWalk();
        showSadAnimation(120, 1);
        break;
      }
      break;
    
    case 'a':
    case 'A':
      standUp();
      turnLeft();
      Serial.println("Turn Left");
      showHappyAnimation(120, 1);
      break;
    case 'd':
    case 'D':
      standUp();
      turnRight();
      Serial.println("Turn Right");
      showHappyAnimation(120, 1);
      break;

    case '1':
      pushup();
      Serial.println("Pushup");
      showHappyAnimation(120, 1);
      break;

    case 'c':
    case 'C':
      calibrate();
      break;

    case '2':
      kembot();
      showHappyAnimation(120, 1);
      break;

    case '3':
      greet();
      showHappyAnimation(120, 1);
      break;

    case '4':
      dogbot();
      showHappyAnimation(120, 1);
      break;

    case '5':
      chaos();
      showHappyAnimation(120, 1);
      break;

    default:
      break;
  }
}

void setup() {

  Serial.begin(115200);
  delay(500);
  oled_init();
  delay(500);
  showSleepAnimation(120, 1);
  pwm.begin();
  pwm.setPWMFreq(60);

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