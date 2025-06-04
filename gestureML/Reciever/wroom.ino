#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>       // 用来设置信道
#include "driver/ledc.h"

// —— 跟发送端一致的 struct —— 
typedef struct struct_message {
    char  a[32];
    int   b;
    float c;
    bool  d;
} struct_message;
static struct_message myData;

// —— motor & PWM 配置（LEDC） —— 
const int pinA = 17, pinB = 18;
const ledc_mode_t      speed_mode = LEDC_LOW_SPEED_MODE;
const ledc_timer_t     timer_num   = LEDC_TIMER_0;
const ledc_channel_t   channelA    = LEDC_CHANNEL_0;
const ledc_channel_t   channelB    = LEDC_CHANNEL_1;
const ledc_timer_bit_t bit_num     = LEDC_TIMER_8_BIT;
const int              freq        = 20000;

bool    unlocked = false;
uint8_t speed    = 0;
const uint8_t step = 20;

// —— 回调：保持你原来的打印格式 —— 
void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: "); Serial.println(len);
  Serial.print("Char: ");          Serial.println(myData.a);
  Serial.print("Int: ");           Serial.println(myData.b);
  Serial.print("Float: ");         Serial.println(myData.c, 3);
  Serial.print("Bool: ");          Serial.println(myData.d);
  Serial.println();

  if (!unlocked && strcmp(myData.a, "Rock") == 0) {
    unlocked = true;
    speed = 128;  // 中速启动
  }
  else if (unlocked) {
    if (strcmp(myData.a, "Paper") == 0) {
      speed = (speed > step ? speed - step : 0);
    } else if (strcmp(myData.a, "Rock") == 0) {
      speed = (speed + step <= 255 ? speed + step : 255);
    }
  }

  // 更新 PWM
  ledc_set_duty(speed_mode, channelA, speed);
  ledc_update_duty(speed_mode, channelA);
  ledc_set_duty(speed_mode, channelB, 0);
  ledc_update_duty(speed_mode, channelB);

  Serial.printf("Motor %s, speed=%d\n\n", unlocked?"running":"stopped", speed);
}

void setup() {
  Serial.begin(115200);
  // 1) 强制切到频道 1
  WiFi.mode(WIFI_STA);
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);

  // 2) 配置 LEDC
  ledc_timer_config_t tcfg = {
    .speed_mode      = speed_mode,
    .duty_resolution = bit_num,
    .timer_num       = timer_num,
    .freq_hz         = freq,
    .clk_cfg         = LEDC_AUTO_CLK
  };
  ledc_timer_config(&tcfg);
  ledc_channel_config_t ccfg = {
    .gpio_num   = pinA,
    .speed_mode = speed_mode,
    .channel    = channelA,
    .intr_type  = LEDC_INTR_DISABLE,
    .timer_sel  = timer_num,
    .duty       = 0,
    .hpoint     = 0
  };
  ledc_channel_config(&ccfg);
  ccfg.channel  = channelB;
  ccfg.gpio_num = pinB;
  ledc_channel_config(&ccfg);

  // 3) 初始化 ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // 回调里已经处理完所有事了
  delay(100);
}
