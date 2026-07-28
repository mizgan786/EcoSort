#include <Arduino.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <WebServer.h>

// ================= WIFI SETTINGS =================
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""
#define WIFI_CHANNEL 6

WebServer server(80);

// ================= PIN MAPPING =================
#define IR_PIN 26      // IR Sensor for dry/wet detection
#define METAL_BTN 27   // Inductive sensor / Metal button
#define BUZZER 25      // Notification buzzer
#define MOISTURE 34    // Moisture sensor (Analog)
#define SERVO_PIN 13   // Flap Servo
#define LED 2          // Status LED

#define IN1 19         // Stepper Motor Coil 1
#define IN2 18         // Stepper Motor Coil 2
#define IN3 5          // Stepper Motor Coil 3
#define IN4 17         // Stepper Motor Coil 4

// ================= ACTUATOR OBJECTS =================
Servo flap;
const int FLAP_CLOSED = 30;
const int FLAP_OPEN   = 120;

int stepPins[4] = {IN1, IN2, IN3, IN4};
int seq[8][4] = {
  {1,0,0,0},{1,1,0,0},{0,1,0,0},{0,1,1,0},
  {0,0,1,0},{0,0,1,1},{0,0,0,1},{1,0,0,1}
};

int stepIndex = 0;
int stepsLeft = 0;
unsigned long lastStep = 0;
const int stepDelay = 3; 

// ================= SYSTEM STATE =================
enum WasteType { DRY = 0, WET = 1, METAL = 2 };
int currentBin = DRY;
String lastWaste = "NONE";

enum State {IDLE, BUZZ, ROTATE, OPEN, DROP, CLOSE};
State state = IDLE;
unsigned long stateTime = 0;
int lastIR = LOW;

// ================= HELPER FUNCTIONS =================
String binName(int b) {
  if (b == DRY) return "DRY";
  if (b == WET) return "WET";
  return "METAL";
}

void stepMotor() {
  for (int i = 0; i < 4; i++)
    digitalWrite(stepPins[i], seq[stepIndex][i]);
  stepIndex = (stepIndex + 1) % 8;
}

// ================= WEB SERVER LOGIC =================
void sendStatus() {
  String json = "{";
  json += "\"waste\":\"" + lastWaste + "\",";
  json += "\"bin\":\"" + binName(currentBin) + "\",";
  json += "}";
  server.send(200, "application/json", json);
}

void handleRoot() {
  String html = "<h1>EcoSort IoT Dashboard</h1><p>Status: Running</p>";
  server.send(200, "text/html", html);
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);
  delay(2000); // Wait for Serial Monitor to catch up
  
  Serial.println("\n==============================");
  Serial.println("   ECOSORT SYSTEM STARTING    ");
  Serial.println("==============================");

  // Initialize Pins
  pinMode(IR_PIN, INPUT);
  pinMode(METAL_BTN, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);
  pinMode(LED, OUTPUT);
  for (int i = 0; i < 4; i++) pinMode(stepPins[i], OUTPUT);

  // Initialize Servo
  flap.setPeriodHertz(50);
  flap.attach(SERVO_PIN, 500, 2400);
  flap.write(FLAP_CLOSED);

  // WiFi Connection with Timeout
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);

  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED && timeout < 20) {
    delay(500);
    Serial.print(".");
    timeout++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected! IP: " + WiFi.localIP().toString());
  } else {
    Serial.println("\nWiFi Timeout - Starting in Offline Mode.");
  }

  server.on("/", handleRoot);
  server.on("/status", sendStatus);
  server.begin();

  Serial.println("System Ready - Waiting for Waste...");
}

// ================= MAIN LOOP =================
void loop() {
  server.handleClient();
  int ir = digitalRead(IR_PIN);

  if (state == IDLE) {
    digitalWrite(LED, HIGH);

    WasteType targetBin;
    bool detected = false;

    // 1. Metal Detection (Highest Priority)
    if (digitalRead(METAL_BTN) == LOW) {
      targetBin = METAL;
      lastWaste = "METAL";
      Serial.println("Waste Detected: METAL");
      digitalWrite(BUZZER, HIGH);
      state = BUZZ;
      stateTime = millis();
      detected = true;
    } 
    // 2. Dry/Wet Detection
    else if (ir == HIGH && lastIR == LOW) {
      int moistureValue = analogRead(MOISTURE);
      if (moistureValue > 1500) { // Adjust threshold based on sensor
        targetBin = WET;
        lastWaste = "WET";
      } else {
        targetBin = DRY;
        lastWaste = "DRY";
      }
      Serial.println("Waste Detected: " + lastWaste);
      detected = true;
    }

    if (detected) {
      int diff = (targetBin - currentBin + 3) % 3;
      int angle = diff * 120;
      stepsLeft = 4096 * angle / 360; // Assuming 28BYJ-48 Stepper
      currentBin = targetBin;
      if (state != BUZZ) state = ROTATE;
    }
  }

  // --- STATE MACHINE ---
  if (state == BUZZ && millis() - stateTime > 500) {
    digitalWrite(BUZZER, LOW);
    state = ROTATE;
  }

  if (state == ROTATE) {
    digitalWrite(LED, LOW);
    if (stepsLeft > 0 && millis() - lastStep > stepDelay) {
      lastStep = millis();
      stepMotor();
      stepsLeft--;
    } else if (stepsLeft == 0) {
      flap.write(FLAP_OPEN);
      state = OPEN;
      stateTime = millis();
    }
  }

  if (state == OPEN && millis() - stateTime > 1000) {
    state = DROP;
    stateTime = millis();
  }

  if (state == DROP && millis() - stateTime > 1200) {
    flap.write(FLAP_CLOSED);
    state = CLOSE;
    stateTime = millis();
  }

  if (state == CLOSE && millis() - stateTime > 1000) {
    state = IDLE;
  }

  lastIR = ir;
}