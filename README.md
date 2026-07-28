# ♻️ EcoSort

An **IoT-enabled Smart Waste Segregation System** that automatically classifies waste into **Dry**, **Wet**, and **Metal** categories using multiple sensors and an ESP32 based control system.

The system integrates an **IR sensor**, **inductive proximity sensor**, **moisture sensor**, **servo motor**, **stepper motor**, and **ESP32 Wi-Fi connectivity** to automate the waste segregation process and provide real-time monitoring through a web dashboard.

---

## 🚀 Features

- ♻️ Automatic Dry, Wet, and Metal Waste Classification
- 💧 Moisture-Based Waste Detection
- 🔩 Metal Detection using Inductive Sensor
- 🔄 Automatic Bin Rotation using Stepper Motor
- 🚪 Servo-Controlled Disposal Flap
- 🌐 ESP32 Web Dashboard for Live Monitoring
- 📡 Built-in Wi-Fi Connectivity
- 🔔 Buzzer Notification on Waste Detection
- 💡 LED Status Indicator
- ⚡ Real-Time Embedded System using ESP32

---

## 🛠 Hardware Components

- ESP32 Development Board
- 28BYJ-48 Stepper Motor
- ULN2003 Stepper Driver
- SG90 Servo Motor
- IR Sensor
- Inductive Proximity Sensor
- Capacitive Moisture Sensor
- Active Buzzer
- LED
- Breadboard & Jumper Wires
- External Power Supply

---

## 📚 Software & Libraries

- Arduino IDE
- ESP32 Board Package
- ESP32Servo
- WiFi Library
- WebServer Library
- C++

---


---

## ⚙️ How It Works

1. The system continuously monitors for incoming waste using the **IR sensor**.
2. If the **inductive sensor** detects metal, the waste is classified as **Metal**.
3. Otherwise, the **moisture sensor** determines whether the waste is **Wet** or **Dry**.
4. The **stepper motor** rotates the platform to align the correct bin.
5. The **servo motor** opens the flap, allowing the waste to fall into the selected bin.
6. The flap closes automatically after disposal.
7. The current waste type and active bin can be monitored through the ESP32 web interface.

---

## 🔄 Waste Classification Logic

| Sensor Condition | Waste Type |
|------------------|------------|
| Metal Sensor Triggered | Metal Waste |
| Moisture Above Threshold | Wet Waste |
| Moisture Below Threshold | Dry Waste |

---

---

## 🏗 System Workflow

```
                Waste Detected
                       │
                       ▼
             Is Metal Detected?
              ┌────────┴────────┐
             Yes               No
              │                 │
              ▼                 ▼
      Select Metal Bin     Read Moisture Sensor
                                │
                  ┌─────────────┴─────────────┐
                  ▼                           ▼
             Wet Waste                  Dry Waste
                  │                           │
                  └─────────────┬─────────────┘
                                ▼
                     Rotate Stepper Motor
                                │
                                ▼
                      Open Servo Flap
                                │
                                ▼
                        Dispose Waste
                                │
                                ▼
                       Close Servo Flap
                                │
                                ▼
                       Return to Idle State
```

---


## 🚀 Future Enhancements

- 📱 Mobile Application
- ☁️ Firebase Cloud Integration
- 📊 Waste Collection Analytics
- 📈 Fill-Level Monitoring
- 📩 SMS/Email Notifications
- 🤖 AI-Based Image Waste Classification
- 📡 MQTT & Blynk Support
- 🌞 Solar-Powered Operation

---


## ⭐ Support

If you found this project useful, consider giving it a **⭐ Star** on GitHub!
