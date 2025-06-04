#include <Seeed_Arduino_SSCMA.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>        // ← 加这个，用来设频道

// 模型推理相关
SSCMA AI;
static const char* LABELS[] = { "Paper", "Rock", "Scissors" };

// 发送结构体
typedef struct struct_message {
    char  a[32];
    int   b;
    float c;
    bool  d;
} struct_message;
static struct_message myData;
esp_now_peer_info_t peerInfo;

// 发送状态回调
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("ESP-NOW Send Status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void setup() {
    Serial.begin(115200);
    AI.begin();

    // 1) STA 模式 + 固定频道（要跟接收端保持一致）
    WiFi.mode(WIFI_STA);
    esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);

    // 2) 初始化 ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
    esp_now_register_send_cb(OnDataSent);

    // 3) 配置对端 MAC & 频道
    uint8_t broadcastAddress[] = {0xD8,0x3B,0xDA,0x77,0x81,0x60};
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 1;      // ← 这里也要改成 1
    peerInfo.encrypt = false;
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
        return;
    }
}

void loop() {
    if (!AI.invoke()) {
        auto &boxes = AI.boxes();
        if (!boxes.empty()) {
            int best = 0;
            for (int i = 1; i < boxes.size(); i++)
                if (boxes[i].score > boxes[best].score) best = i;

            int   cls   = boxes[best].target;
            float score = boxes[best].score;
            strncpy(myData.a, LABELS[cls], sizeof(myData.a)-1);
            myData.a[sizeof(myData.a)-1] = '\0';
            myData.b = cls;
            myData.c = score;
            myData.d = false;

            Serial.print("Detected: "); Serial.print(myData.a);
            Serial.print(" (idx="); Serial.print(myData.b);
            Serial.print(", conf="); Serial.print(myData.c, 2);
            Serial.println(")");

            esp_err_t result = esp_now_send(peerInfo.peer_addr, (uint8_t *)&myData, sizeof(myData));
            if (result != ESP_OK) Serial.println("Error sending ESP-NOW data");
        }
    }
    delay(200);
}
