# 515HWproject

## Overview
This repository contains all code and design files for streaming sensor data and the enclosure design for the 515HWproject. It includes:
- Instructions for running the code
- Code used to stream sensor data
- Enclosure design files

---

## Repository Structure
```
515HWproject/
├── README.md
├── src/
│   ├── stream_sensor.py
│   ├── main.py
│   └── (other Python/Arduino files)
├── enclosure_design/
│   ├── enclosure.step
│   ├── enclosure.svg
│   ├── enclosure.dxf
│   └── (other CAD files)
└── requirements.txt
```

---

## Prerequisites
- **Hardware**: 
  - Microcontroller (e.g., ESP32, Arduino)
  - Sensor module (e.g., MPU6050, PPG, wind sensor)
  - USB cable or wireless uploader
- **Software**:
  - Python 3.7+ (for any Python-based scripts)
  - Arduino IDE 1.8+ (if using Arduino sketches)
  - PlatformIO (optional, for ESP32/ESP8266 development)
  - Git (to clone this repo)
  - Required Python libraries (listed in `requirements.txt`)

---

## Installation

1. **Clone the repository**  
   ```bash
   git clone https://github.com/YifeiL1122/515HWproject.git
   cd 515HWproject
   ```

2. **(Optional) Create a virtual environment**  
   ```bash
   python3 -m venv venv
   source venv/bin/activate      # macOS/Linux
   venv\Scripts\activate         # Windows
   ```

3. **Install Python dependencies**  
   ```bash
   pip install -r requirements.txt
   ```

4. **Prepare Arduino/PlatformIO environment**  
   - Open `src/stream_sensor.py` or the Arduino sketch in the Arduino IDE/PlatformIO.
   - Install any board support packages (e.g., ESP32 core).

---

## How to Run the Code

### 1. Streaming Sensor Data (Python)
   - **File**: `src/stream_sensor.py`
   - **Description**: Streams real-time data from the connected sensor over a serial port (e.g., `/dev/ttyUSB0` or `COM3`).
   - **Usage**:
     ```bash
     cd src
     python stream_sensor.py --port /dev/ttyUSB0 --baud 115200
     ```
     Replace `/dev/ttyUSB0` with your serial port.  
   - **Command-line options**:
     - `--port`: Serial port where the microcontroller is connected
     - `--baud`: Baud rate (default: 115200)
     - `--output` (optional): If specified, save streamed data to a CSV file  
   - **Example**:
     ```bash
     python stream_sensor.py --port COM3 --baud 115200 --output data_log.csv
     ```

### 2. Streaming Sensor Data (Arduino / Microcontroller)
   - **File**: any Arduino sketch in `src/` (e.g., `src/stream_sensor.ino` or similar)
   - **Description**: Reads sensor values (e.g., from MPU6050) and sends them out via Serial or Wi-Fi.
   - **How to upload**:
     1. Open the sketch in Arduino IDE or PlatformIO.
     2. Select the correct board and port.
     3. Click **Upload**.
   - **How to view data**:
     - Open the Serial Monitor at the matching baud rate (e.g., 115200).

---

## Enclosure Design Files
All enclosure design files are stored in `enclosure_design/`. These include:
- **enclosure.step** – STEP file for 3D modeling software
- **enclosure.svg** – 2D vector outline for laser cutting
- **enclosure.dxf** – DXF file compatible with most CAD/CAM tools  
- **README within `enclosure_design/`**: Describes dimensions and assembly steps

**How to use**:
1. Open `enclosure.step` in your preferred CAD software (e.g., Fusion 360, SolidWorks) to view or modify.
2. Use `enclosure.svg` or `enclosure.dxf` to laser-cut the panels.
3. Follow the assembly notes (in `enclosure_design/README.md`) to fit the PCB, sensors, and battery.

---

## Usage Summary

1. **Assemble hardware** according to the circuit diagram in `docs/circuit_diagram.png` (if provided).
2. **Upload microcontroller code**:
   - For Arduino: open `src/stream_sensor.ino`, upload to board.
   - For Micropython/Python: flash the `.py` file to the board.
3. **Run the Python streamer** (if applicable):
   ```bash
   cd src
   python stream_sensor.py --port <your_serial_port> --baud 115200
   ```
4. **Collect data** and verify real-time output in the console or CSV.

---

## Helpful Links
- [Arduino IDE Download](https://www.arduino.cc/en/software)
- [PlatformIO Documentation](https://docs.platformio.org/)
- [Python 3 Downloads](https://www.python.org/downloads/)
- [Fusion 360 (Free for Personal Use)](https://www.autodesk.com/products/fusion-360/personal)

---

## License
This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.

---

## Acknowledgments
- Sensor library authors (e.g., Adafruit for MPU6050)
- Class instructors and TAs for guidance
- Team members who contributed code and CAD designs
