#include <Arduino.h>
#include <HX711.h>
#include <NewPing.h>
#include <Adafruit_MPU6050.h>
#include <ArduinoJson.h>

// PIN DEFINITIONS
#define LOADCELL_DOUT_PIN  4
#define LOADCELL_SCK_PIN   5
#define TRIGGER_PIN       10
#define ECHO_PIN          11
#define IR_SENSOR_PIN     13
#define MAX_DISTANCE     200

// SENSOR OBJECTS
HX711 scale;
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
Adafruit_MPU6050 mpu;

void setup() {
  Serial.begin(115200);   // Debug
  Serial1.begin(115200, SERIAL_8N1, 3, 1); // TX to S3 RX (Pins 3/1)

  // 1. Weight Initialization (Sogeking)
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(2280.f); 
  scale.tare();

  // 2. Accelerometer Initialization (MPU6050)
  if (!mpu.begin()) { Serial.println("MPU6050 Failed"); }
  
  pinMode(IR_SENSOR_PIN, INPUT);
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  StaticJsonDocument<200> doc;
  doc["weight"] = scale.get_units(3);
  doc["dist"] = sonar.ping_cm();
  doc["moisture"] = digitalRead(IR_SENSOR_PIN) == LOW; // Low = Detection
  doc["tilt_x"] = a.acceleration.x;
  doc["hb"] = millis(); // Heartbeat

  serializeJson(doc, Serial1);
  Serial1.println();
  delay(100); // 10Hz Verification Rate
}