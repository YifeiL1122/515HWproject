# ESP32-S3-CAM Gesture Recognition

This project implements real-time gesture recognition on ESP32-S3-CAM using ESP-IDF and ESP-DL.

## Prerequisites

1. ESP-IDF v5.0 or later
2. Python 3.7 or later
3. TensorFlow 2.x
4. ESP32-S3-CAM board
5. USB cable for programming

## Setup Instructions

1. Install ESP-IDF:
```bash
git clone -b v5.0 --recursive https://github.com/espressif/esp-idf.git
cd esp-idf
./install.sh
. ./export.sh
```

2. Convert and quantize the model:
```bash
cd load
python convert_model.py
```

3. Build and flash the project:
```bash
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

## Project Structure

- `main/main.c`: Main application code
- `convert_model.py`: Script to convert and quantize the TFLite model
- `CMakeLists.txt`: Build configuration

## Camera Configuration

The camera is configured for:
- Resolution: QVGA (320x240)
- Format: RGB565
- Frame rate: ~10fps

## Model Configuration

- Input size: 96x96x3
- Output: 5 gesture classes
- Quantization: INT8

## Monitoring Results

The recognition results will be printed to the serial monitor. You can view them using:
```bash
idf.py monitor
```

## Troubleshooting

1. If camera initialization fails:
   - Check camera connections
   - Verify power supply
   - Ensure correct pin configuration

2. If model inference fails:
   - Verify model quantization
   - Check input tensor dimensions
   - Ensure sufficient memory

## Notes

- The model is quantized to INT8 for better performance on ESP32-S3
- Camera frames are processed in real-time
- Results are printed to serial monitor 