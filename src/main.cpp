#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "legs.h"
#include "comm.h"
#include "espnow.h"
#include "sequences.h"

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
        break;
      }
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
      pushup();
      Serial.println("Pushup");
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
}

void setup() {
  Serial.begin(115200);
  delay(500);

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

  robot.update();
}