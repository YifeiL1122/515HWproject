# ESP32-C3 Rock-Paper-Scissors Windless Chime Controller

## Introduction
This project implements a real-time Rock-Paper-Scissors hand-gesture recognition system on an ESP32-C3. A camera captures each frame, an external SceneCraft AI V2 module infers the gesture, and ESP-NOW transmits the result to a second ESP-based controller (“ESPtalk”). Upon receiving the gesture code, EspTalk drives a stepper motor to strike a wind chime: single strike for Rock, double strike for Paper, and no action for Scissors. This setup demonstrates on-device AI inference, peer-to-peer wireless communication, and low-latency motor control without a router.

---

## Hardware Requirements
- **ESP32-C3 Development Board** (camera host + AI interface)
- **OV2640 Camera Module** (mounted on ESP32-C3)
- **SceneCraft AI V2 Inference Module** (pre-flashed with a Rock-Paper-Scissors model)
- **Second ESP-Based Board (“ESPtalk”)** for motor control (e.g., ESP32 Dev Module)
- **Stepper Motor** (1.8°/step or similar)
- **Stepper Driver Board** (e.g., A4988 or DRV8825)
- **Power Supplies:**
  - ESP32 boards: 5 V USB or regulated 3.3 V (camera and AI module can draw from the 3.3 V pin if current allows)
  - Stepper motor driver: 12 V (or voltage matching your motor)
- **Connecting Wires and Breadboard**

---

## Wiring and Pin Configuration

### ESP32-C3 ↔ OV2640 Camera
- **XCLK** → GPIO 8 (output 20 MHz clock)
- **SIOD (SDA)** → GPIO 14 (I²C data)
- **SIOC (SCL)** → GPIO 15 (I²C clock)
- **D0–D7 (Camera data lines)** → GPIO 2, 3, 4, 5, 6, 7, 9, 10
- **VSYNC, HREF, PCLK** → Any free GPIOs; update these assignments in code to match your wiring.
- **PWDN (Power down)** → GPIO 0 (or tie LOW if unused)
- **RESET** → GPIO 1 (or leave unconnected if not required)
- **3.3 V, GND** → Camera VCC and GND

### ESP32-C3 ↔ SceneCraft AI V2
- **UART TX (AI V2)** → **ESP32-C3 RX (e.g., GPIO 17)**
- **UART RX (AI V2)** → **ESP32-C3 TX (e.g., GPIO 18)**
- **3.3 V, GND** → AI V2 VCC and GND

### ESP32-C3 ↔ EspTalk (ESP-NOW Peer)
No physical wires—ESP-NOW uses Wi-Fi channel 1 (both boards must lock channel 1 in software).

### EspTalk ↔ Stepper Driver
- **DIR Pin (Driver)** → **EspTalk GPIO 16**
- **STEP Pin (Driver)** → **EspTalk GPIO 17**
- **ENABLE Pin (Driver, if required)** → Tie to GND or to an EspTalk GPIO configured LOW
- **Motor Power (12 V)** → Driver VIN (motor supply)
- **Driver GND** → EspTalk GND (common ground)

---

## Software Requirements
- **Arduino IDE 2.x** (or PlatformIO in VS Code)
- **Espressif ESP32 Board Package** (supports ESP32-C3)
- **Seeed SSCMA Library** (for SceneCraft AI V2; install via Library Manager or from Seeed GitHub)
- **AccelStepper Library** (for stepper control on EspTalk)
- **esp_now.h**, **WiFi.h**, **esp_wifi.h** (included with ESP32 board package)

---

## Installation and Flashing

1. **Clone or Download Repository**

   Place the folders `gesture_sender` and `motor_receiver` inside your Arduino sketch directory (or PlatformIO project workspace).

2. **ESP32-C3 “gesture_sender” Sketch**

   1. Open `gesture_sender/gesture_sender.ino` in Arduino IDE.
   2. Verify that:
      ```cpp
      static const char* LABELS[] = { "Paper", "Rock", "Scissors" };
      ```
      matches the AI model’s class order.
   3. Update `peerInfo.peer_addr` to the MAC address of your EspTalk board, for example:
      ```cpp
      uint8_t broadcastAddress[] = {0xD8, 0x3B, 0xDA, 0x77, 0x81, 0x60};
      memcpy(peerInfo.peer_addr, broadcastAddress, 6);
      ```
   4. Ensure the Wi-Fi channel lock is in `setup()`:
      ```cpp
      WiFi.mode(WIFI_STA);
      esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
      ```
   5. Select **Board → ESP32C3 Dev Module**, choose the correct COM port, and click **Upload**.
   6. Confirm the console shows no errors and indicates successful AI module initialization.

3. **EspTalk “motor_receiver” Sketch**

   1. Open `motor_receiver/motor_receiver.ino` in Arduino IDE.
   2. Ensure the stepper pins match your wiring:
      ```cpp
      AccelStepper stepper(AccelStepper::DRIVER, 17, 16);
      ```
      (STEP → GPIO 17, DIR → GPIO 16).
   3. Lock the Wi-Fi channel in `setup()`:
      ```cpp
      WiFi.mode(WIFI_STA);
      esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
      ```
   4. Select **Board → ESP32 Dev Module** (or your EspTalk model), choose the correct COM port, and click **Upload**.
   5. Open Serial Monitor at 115200 baud to confirm initialization messages.

---

## Usage

1. **Power Up**

   - Connect **ESP32-C3** to USB or a regulated 5 V source.
   - Connect **EspTalk** to USB or a regulated 5 V source.
   - Power the stepper driver with a 12 V supply (motor supply).
   - Open both Serial Monitors at 115200 baud to confirm:
     - **Sender**: “AI module ready”
     - **Receiver**: “Receiver ready, waiting for gestures”

2. **Demonstration**

   1. Stand **0.5–1.5 m** from the camera so your hand fills most of the frame.
   2. Perform a **Paper** gesture (flat palm).
      - AI V2 infers “Paper” → ID 0.
      - ESP32-C3 prints:
        ```
        Detected: Paper (idx=0, conf=0.97)
        ESP-NOW Send Status: Success
        ```
   3. Perform a **Rock** gesture (closed fist).
      - AI V2 infers “Rock” → ID 1.
      - ESP32-C3 prints:
        ```
        Detected: Rock (idx=1, conf=0.92)
        ESP-NOW Send Status: Success
        ```
   4. Perform a **Scissors** gesture (two extended fingers).
      - AI V2 infers “Scissors” → ID 2.
      - ESP32-C3 prints:
        ```
        Detected: Scissors (idx=2, conf=0.95)
        ESP-NOW Send Status: Success
        ```

3. **Observe the Wind Chime**

   - For **Paper**: Stepper motor strikes **twice**.
   - For **Rock**: Stepper motor strikes **once**.
   - For **Scissors**: Stepper motor remains **silent**.

---

## Troubleshooting

- **“ESP-NOW Send Status: Fail”**
  - Ensure both boards are locked to **channel 1**.
  - Verify the sender’s `peerInfo.peer_addr` exactly matches the receiver’s MAC address.
  - Confirm that `esp_now_add_peer()` returned `ESP_OK`.

- **AI Module Never Returns a Valid Gesture**
  - Confirm SceneCraft AI V2 has the correct Rock-Paper-Scissors model flashed.
  - Check camera initialization messages (`esp_camera_init()` success).
  - Improve lighting or background contrast; poor conditions can cause misclassification.

- **Stepper Motor Hums but Doesn’t Rotate**
  - Verify the stepper driver’s **ENABLE** pin is pulled low (tie to GND if required).
  - Reduce `stepper.setMaxSpeed()` or `stepper.setAcceleration()` values if the motor stalls.
  - Confirm the motor driver’s voltage and current limits match the motor’s requirements.

- **Receiver Never Prints “Gesture received:”**
  - Confirm ESP-NOW callback is registered (`esp_now_register_recv_cb(onDataRecv)`).
  - Ensure both boards share the same Wi-Fi channel and encryption settings.
  - If using unicast, add the sender’s MAC as a peer on the receiver (optional).

---

## License
This repository is licensed under the MIT License.  
- The Rock-Paper-Scissors model was created and trained with Edge Impulse (© EdgeImpulse Inc.).  
- ESP-NOW communication code is adapted from Espressif’s official examples.  
- AccelStepper library is used under its existing open-source license (MIT).

See the `LICENSE` file for full details and acknowledgments.
