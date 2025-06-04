# 515HWproject

## Overview
This repository contains the implementation of a gesture recognition system using ESP32 microcontrollers and machine learning. The project includes:
- ESP32-based sensor nodes for data collection
- Machine learning models for gesture recognition
- Enclosure designs for the hardware components

---

## Repository Structure
```
515HWproject/
├── README.md
├── Sensor/
│   ├── esp_outside/     # ESP32 code for outdoor sensor node
│   ├── esp_camera/      # ESP32 code for camera module
│   └── esp_windchime/   # ESP32 code for wind chime sensor
├── gestureML/
│   ├── Dataset/         # Training and test datasets
│   ├── MLtrain/         # Machine learning training scripts
│   ├── ModelAssistant/  # Model training assistance tools
│   ├── Reciever/        # Receiver implementation
│   ├── Sender/          # Sender implementation
│   └── saved_models/    # Trained ML models
└── Enclosure/           # Hardware enclosure designs
```

---

## Prerequisites
- **Hardware**: 
  - ESP32 microcontrollers
  - Sensors (as specified in each ESP32 module)
  - USB cables for programming
- **Software**:
  - Python 3.7+
  - ESP-IDF or Arduino IDE
  - Required Python libraries (listed in `gestureML/requirements.txt`)
  - Git

---

## Installation

1. **Clone the repository**  
   ```bash
   git clone https://github.com/YifeiL1122/515HWproject.git
   cd 515HWproject
   ```

2. **Set up Python environment**  
   ```bash
   cd gestureML
   python -m venv venv
   source venv/bin/activate      # macOS/Linux
   venv\Scripts\activate         # Windows
   pip install -r requirements.txt
   ```

3. **ESP32 Setup**
   - Install ESP-IDF or Arduino IDE
   - Configure ESP32 development environment
   - Install required ESP32 libraries

---

## Project Components

### 1. Sensor Nodes (ESP32)
Located in the `Sensor/` directory:
- **esp_outside**: Outdoor sensor node implementation
- **esp_camera**: Camera module implementation
- **esp_windchime**: Wind chime sensor implementation

### 2. Machine Learning Components
Located in the `gestureML/` directory:
- **Dataset**: Contains training and test data
- **MLtrain**: Scripts for training gesture recognition models
- **ModelAssistant**: Tools for model training and evaluation
- **Reciever/Sender**: Implementation of data transmission
- **saved_models**: Pre-trained models for gesture recognition

### 3. Enclosure
Located in the `Enclosure/` directory:
- Design files for housing the ESP32 modules and sensors

---

## Usage

### Training the Model
1. Navigate to the gestureML directory:
   ```bash
   cd gestureML
   ```

2. Prepare your dataset in the `Dataset/` directory

3. Train the model:
   ```bash
   cd MLtrain
   python train_model.py
   ```

### Running the System
1. Upload the appropriate ESP32 code to each module
2. Start the receiver:
   ```bash
   cd gestureML/Reciever
   python receiver.py
   ```
3. The system will begin processing sensor data and recognizing gestures

---

## Contributing
Please read the contribution guidelines before submitting pull requests.

---

## License
This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.

---

## Acknowledgments
- ESP32 development team
- Machine learning library authors
- Project team members and contributors
