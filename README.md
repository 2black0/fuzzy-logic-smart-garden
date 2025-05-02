# 🌱 Fuzzy Logic Smart Garden 🌿

An intelligent garden automation system based on **fuzzy logic** using **ESP8266 (NodeMCU)**. This system adjusts water pump speed based on soil moisture and air temperature using linguistic fuzzy rules. Real-time monitoring and control is available via **Blynk** and an **LCD display**.

Developed by [2black0](mailto:2black0@gmail.com) – 2021

---

## 📦 Features

- 🌡️ **Temperature Monitoring** – DHT11 sensor
- 💧 **Soil Moisture Reading** – Analog soil sensor
- 🚿 **Pump Control** – Fuzzy logic adjusts speed (PWM)
- 📲 **Remote Monitoring** – Blynk mobile app integration
- 📟 **LCD Feedback** – 16x2 I2C display for real-time status
- 🤖 **Motor Reversal Logic** – Basic IN1/IN2 setup for optional motor control

---

## 🧠 Fuzzy Logic System

### **Inputs**

| Name        | Linguistic Terms          | Range         |
|-------------|---------------------------|---------------|
| Temperature | Cold, Warm, Hot           | 0 – 40 °C     |
| Soil Moist. | Dry, Normal, Wet          | 0 – 100 %     |

### **Output**

| Name  | Linguistic Terms                     | Range       |
|-------|--------------------------------------|-------------|
| Pump  | Very Slow, Slow, Average, Fast, Very Fast | 0 – 100 % |

### **Rule Base**

| IF Soil Moisture | AND Temperature | THEN Pump Speed |
|------------------|------------------|------------------|
| Dry              | Cold             | Average          |
| Dry              | Warm             | Fast             |
| Dry              | Hot              | Very Fast        |
| Normal           | Cold             | Slow             |
| Normal           | Warm             | Average          |
| Normal           | Hot              | Fast             |
| Wet              | Cold             | Very Slow        |
| Wet              | Warm             | Slow             |
| Wet              | Hot              | Average          |

---

## 🛠️ Hardware Requirements

| Component             | Description                                |
|------------------------|--------------------------------------------|
| ESP8266 NodeMCU        | Main microcontroller with Wi-Fi            |
| DHT11 Sensor           | For temperature & humidity                 |
| Analog Soil Sensor     | For soil moisture detection                |
| Water Pump             | Controlled by PWM signal                   |
| I2C LCD 16x2           | For displaying temperature, moisture, etc. |
| Motor Driver (optional)| Connected to `IN1` & `IN2` for expansion   |
| Power Supply           | For ESP + peripherals (recommend 5V 2A)    |

---

## ⚙️ Wiring Guide

| Component         | ESP8266 Pin | Description                      |
|------------------|-------------|----------------------------------|
| Soil Sensor       | A0          | Analog input for moisture        |
| DHT11 Data        | D4 (GPIO2)  | Digital input                    |
| LCD I2C           | SDA/SCL     | A4 / A5 equivalent               |
| Pump Control      | D5 (GPIO14) | PWM output                       |
| Motor IN1         | D6 (GPIO12) | Motor direction control (optional) |
| Motor IN2         | D7 (GPIO13) | Motor direction control (optional) |

---

## 📲 Blynk Configuration

- Register on [https://blynk.io](https://blynk.io) and create a new project
- Add 3 **Value Displays**:
  - V0 → Temperature
  - V1 → Soil Moisture
  - V2 → Pump Speed (%)
- Replace `auth`, `ssid`, and `pass` in `main.ino` with your credentials:

```cpp
char auth[] = "YourAuthToken";
char ssid[] = "YourNetworkName";
char pass[] = "YourPassword";
```

---

## 🔁 System Workflow

1. Read temperature from **DHT11**
2. Read moisture from **soil sensor**
3. Fuzzify both inputs into **linguistic terms**
4. Apply fuzzy rules to determine **pump speed**
5. **Defuzzify** output to get crisp PWM value
6. Control water pump via **analogWrite()**
7. Send values to **LCD** and **Blynk app**

---

## 📟 LCD Display Format

```
t:25.3C h:72.1%
w:64.0%
```

* `t:` – temperature in Celsius
* `h:` – soil humidity (interpreted)
* `w:` – pump speed in percent

---

## 🧪 Sample Serial Output

```
Temperature: 28.0 °C
Soil Moisture: 37.0 %
Water Pump: 60.0 %
```

---

## 📦 Libraries Required

Install via Library Manager or manually:

* `Adafruit Unified Sensor`
* `DHT sensor library`
* `LiquidCrystal_I2C`
* `Blynk`
* `Fuzzy`
* `ESP8266WiFi`

---

## 🧠 Notes

* Soil sensor mapping might need adjustment based on sensor type
* Calibration may be required for soil range and fuzzy set boundaries
* This project is extensible with weather forecast APIs or solar-powered supply

---

## 📜 License

This project is licensed under the [MIT License](LICENSE).

---

## 👨‍💻 Author

**Ardy Seto Priambodo (2black0)**
🚀 IoT, AI, and Robotics enthusiast
🌐 [http://robot-terbang.web.id](http://robot-terbang.web.id)
📫 Contact: [2black0@gmail.com](mailto:2black0@gmail.com)