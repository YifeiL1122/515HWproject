#include <stdio.h>
#include "esp_system.h"
#include "esp_log.h"
#include "esp_camera.h"
#include "dl_detect.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"    // 新增：GPIO 控制

static const char *TAG = "gesture_detection";

// 这部分不变…

// 按板子实际接线改：
static camera_config_t camera_config = {
    .pin_pwdn       = -1,    // 板子没接 pwdn，设 -1
    .pin_reset      = -1,    // 板子没接 reset，设 -1
    .pin_xclk       = 3,     // Arduino 例子里 XCLK_GPIO = 3
    .pin_sscb_sda   = 48,    // SIOD
    .pin_sscb_scl   = 47,    // SIOC
    .pin_d7         = 18,    // Y9
    .pin_d6         = 15,    // Y8
    .pin_d5         = 38,    // Y7
    .pin_d4         = 42,    // Y6
    .pin_d3         = 40,    // Y5
    .pin_d2         = 46,    // Y4
    .pin_d1         = 45,    // Y3
    .pin_d0         = 41,    // Y2
    .pin_vsync      = 1,     // VSYNC
    .pin_href       = 2,     // HREF
    .pin_pclk       = 39,    // PCLK

    .xclk_freq_hz   = 20000000,         // 20MHz
    .ledc_timer     = LEDC_TIMER_0,
    .ledc_channel   = LEDC_CHANNEL_0,

    .pixel_format   = PIXFORMAT_RGB565, // 或者 PIXFORMAT_JPEG，看你需求
    .frame_size     = FRAMESIZE_QVGA,
    .jpeg_quality   = 12,
    .fb_count       = 1,
    .grab_mode      = CAMERA_GRAB_WHEN_EMPTY
};

void app_main(void)
{
    // 先把板子上控制摄像头电源的脚拉低（GPIO8），让摄像头通电
    gpio_reset_pin(GPIO_NUM_8);
    gpio_set_direction(GPIO_NUM_8, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_8, 0);

    ESP_LOGI(TAG, "Initializing camera…");
    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Camera init failed (0x%x), halting here!", err);
        // 出错就挂住，不再往下跑
        while (true) {
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
    ESP_LOGI(TAG, "Camera initialized successfully");

    ESP_LOGI(TAG, "Initializing deep learning model…");
    // if (!dl_detect_init()) { … }

    ESP_LOGI(TAG, "Deep learning model ready");

    while (true) {
        // 你的手势识别主循环
        // dl_detect_run();
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
