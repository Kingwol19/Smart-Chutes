#include <Arduino.h>
#include <ESP32Servo.h>
#include <ArduinoJson.h>

Servo sortGate;
#define SERVO_PIN 18

void setup() {
  Serial.begin(115200);  // Comm with Pi/ThinkPad
  Serial1.begin(115200, SERIAL_8N1, 1, 2); // RX from WROVER (Pins 1/2)
  
  sortGate.attach(SERVO_PIN);
  sortGate.write(90); // Home Position
}

void loop() {
  if (Serial1.available()) {
    StaticJsonDocument<300> doc;
    DeserializationError error = deserializeJson(doc, Serial1);

    if (!error) {
      float weight = doc["weight"];
      int dist = doc["dist"];
      bool wet = doc["moisture"];

      // THE DETERMINISTIC HANDSHAKE
      // If we get an external command from Pi/Cerebras
      if (Serial.available()) {
        String ai_guess = Serial.readStringUntil('\n');
        
        // Purity Shield Logic
        if (ai_guess == "PAPER" && weight < 25.0 && !wet) {
          sortGate.write(135); // Clean Paper Sort
          delay(2000);
          sortGate.write(90);
        } else if (weight > 50.0 || wet) {
          Serial.println("ALERT: CONTAMINATION DETECTED. Routing to Complex.");
          sortGate.write(90); // Default drop
        }
      }
    }
  }
}