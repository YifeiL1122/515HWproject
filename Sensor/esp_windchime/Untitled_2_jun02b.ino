#include "arduino_secrets.h"
/*
  ESP32-S3 + Arduino IoT Cloud - 风速优先 + 手势控制（修复版）

  修复问题：
  - 手势调整基于当前档位，而不是基础档位
  - 加档/减档操作更直观
*/
#ifndef ARDUINO_SECRETS_H
#define ARDUINO_SECRETS_H
#define SECRET_SSID          
#define SECRET_OPTIONAL_PASS 
#define SECRET_DEVICE_KEY    "UWDbon4zO3OBH2OptNbmdLkTW"
#endif

#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>
#include "thingProperties.h"

// —— 硬件引脚 —— 
const int pinA             = 18;
const int pinB             = 12;
const int electromagnetPin = 2;

// —— PWM 参数 —— 
const int pwmFreq       = 20000;
const int pwmResolution = 8;
const int pwmChannelA   = 0;
const int pwmChannelB   = 1;

// —— 控制状态 —— 
int baseLevel        = 0;        // 风速决定的基础档位
int currentLevel     = 0;        // 实际档位
bool waitingForGesture = false;  // 是否在等待手势状态（收到石头后）

// —— 电磁铁 & 马达 循环 —— 
bool magToggleActive = false;
unsigned long magToggleTime = 0;
bool magIsOn = false;
const unsigned long magOnDur = 1000;
const unsigned long magOffDur = 1000;

bool motorToggleActive = false;
unsigned long motorToggleTime = 0;
int motorState = 0;

// —— 函数声明 —— 
void motorForward();
void motorReverse();
void motorStop();
void updateDeviceState();

// —— Cloud 回调函数 —— 

void onWindspeedChange() {
  // 计算新的基础档位
  int newBaseLevel;
  if (windspeed <= 1.0) newBaseLevel = 0;
  else if (windspeed <= 8.0) newBaseLevel = 1;
  else if (windspeed <= 18.0) newBaseLevel = 2;
  else newBaseLevel = 3;
  
  // 只有基础档位变化时才处理
  if (newBaseLevel != baseLevel) {
    baseLevel = newBaseLevel;
    
    Serial.print("[风速] 更新: ");
    Serial.print(windspeed);
    Serial.print(" → 新基础档位: ");
    Serial.println(baseLevel);
    
    // 风速变化时立即应用基础档位
    currentLevel = baseLevel;
    outputStatus = currentLevel;
    
    // 重置手势状态
    waitingForGesture = false;
    
    updateDeviceState();
  }
}

void onGestureChange() {
  Serial.print("[手势] 收到: ");
  Serial.println(gesture);
  
  // 忽略无效手势
  if (gesture == -1) return;
  
  // 风速为0时忽略所有手势
  if (baseLevel == 0) {
    Serial.println("  → 风速为0，忽略手势");
    return;
  }
  
  // 石头手势处理
  if (gesture == 1) {
    waitingForGesture = true;
    Serial.println("  → 石头: 等待下一个手势");
  }
  // 在等待手势状态下处理其他手势
  else if (waitingForGesture) {
    // 剪刀手势 - 加档（基于当前档位）
    if (gesture == 2) {
      currentLevel = min(currentLevel + 1, 3);
      waitingForGesture = false;
      Serial.print("  → 剪刀: 加档 → ");
      Serial.println(currentLevel);
    }
    // 布手势 - 减档（基于当前档位）
    else if (gesture == 0) {
      currentLevel = max(currentLevel - 1, 0);
      waitingForGesture = false;
      Serial.print("  → 布: 减档 → ");
      Serial.println(currentLevel);
    }
    // 再次石头 - 重置等待状态
    else if (gesture == 1) {
      waitingForGesture = true;
      Serial.println("  → 石头: 重新等待下一个手势");
    }
    
    // 更新状态
    outputStatus = currentLevel;
    updateDeviceState();
  }
}

void onOutputStatusChange() {
  Serial.print("[手动] 设置档位: ");
  Serial.println(outputStatus);
  
  // 手动覆盖档位
  int forced = constrain(outputStatus, 0, 3);
  currentLevel = forced;
  baseLevel = forced;  // 同时更新基础档位
  
  // 重置手势状态
  waitingForGesture = false;
  
  outputStatus = currentLevel; // 确认同步
  updateDeviceState();
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // 初始化 GPIO
  pinMode(electromagnetPin, OUTPUT);
  digitalWrite(electromagnetPin, LOW);

  // 初始化 PWM
  ledcSetup(pwmChannelA, pwmFreq, pwmResolution);
  ledcAttachPin(pinA, pwmChannelA);
  ledcSetup(pwmChannelB, pwmFreq, pwmResolution);
  ledcAttachPin(pinB, pwmChannelB);

  // 初始化 Cloud
  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  while (!ArduinoCloud.connected()) {
    ArduinoCloud.update();
    delay(500);
  }
  Serial.println(">>> Cloud Connected");

  // 初始状态
  baseLevel = 0;
  currentLevel = 0;
  outputStatus = 0;
  waitingForGesture = false;
  motorStop();
  digitalWrite(electromagnetPin, LOW);
}

void loop() {
  ArduinoCloud.update();
  unsigned long nowMs = millis();

  // 停机状态处理
  if (currentLevel == 0) {
    if (motorToggleActive || magToggleActive) {
      motorStop();
      digitalWrite(electromagnetPin, LOW);
      magToggleActive = false;
      motorToggleActive = false;
    }
    return;
  }

  // 电磁铁循环
  if (magToggleActive) {
    if (magIsOn) {
      if (nowMs - magToggleTime >= magOnDur) {
        magIsOn = false;
        magToggleTime = nowMs;
        digitalWrite(electromagnetPin, LOW);
      }
    } else {
      if (nowMs - magToggleTime >= magOffDur) {
        magIsOn = true;
        magToggleTime = nowMs;
        digitalWrite(electromagnetPin, HIGH);
      }
    }
  }

  // 马达循环
  if (motorToggleActive) {
    switch (currentLevel) {
      case 1:  // 3s 前进 → 5s 停止
        if (motorState == 0) {
          if (nowMs - motorToggleTime >= 3000) {
            motorState = 1;
            motorToggleTime = nowMs;
            motorStop();
          }
        } else {
          if (nowMs - motorToggleTime >= 5000) {
            motorState = 0;
            motorToggleTime = nowMs;
            motorForward();
          }
        }
        break;

      case 2:  // 3s 前进 → 1s 停止
        if (motorState == 0) {
          if (nowMs - motorToggleTime >= 3000) {
            motorState = 1;
            motorToggleTime = nowMs;
            motorStop();
          }
        } else {
          if (nowMs - motorToggleTime >= 1000) {
            motorState = 0;
            motorToggleTime = nowMs;
            motorForward();
          }
        }
        break;

      case 3:  // 3s 前进 → 3s 反转
        if (motorState == 0) {
          if (nowMs - motorToggleTime >= 3000) {
            motorState = 1;
            motorToggleTime = nowMs;
            motorReverse();
          }
        } else {
          if (nowMs - motorToggleTime >= 3000) {
            motorState = 0;
            motorToggleTime = nowMs;
            motorForward();
          }
        }
        break;
    }
  }

  delay(10);
}

////////////////////////////
// 马达控制函数
////////////////////////////
void motorForward() {
  ledcWrite(pwmChannelA, 225);
  ledcWrite(pwmChannelB, 0);
}

void motorReverse() {
  ledcWrite(pwmChannelA, 0);
  ledcWrite(pwmChannelB, 225);
}

void motorStop() {
  ledcWrite(pwmChannelA, 0);
  ledcWrite(pwmChannelB, 0);
}

void updateDeviceState() {
  Serial.print("档位更新: ");
  Serial.println(currentLevel);
  
  switch (currentLevel) {
    case 0:
      motorStop();
      digitalWrite(electromagnetPin, LOW);
      magToggleActive = false;
      motorToggleActive = false;
      break;

    case 1:
    case 2:
    case 3:
      if (!motorToggleActive) {
        motorToggleActive = true;
        motorToggleTime = millis();
        motorState = 0;
        motorForward();
      }
      if (!magToggleActive) {
        magToggleActive = true;
        magToggleTime = millis();
        magIsOn = false;
      }
      break;
  }
}