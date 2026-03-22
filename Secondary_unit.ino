#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <SoftwareSerial.h>

SoftwareSerial esp(2, 3);

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

float x0, y0, z0;
float threshold = 0.5;

bool L1 = false;
bool F1 = false;

int waterPin = A0;
int waterThreshold = 400;

String ssid = "MAIN_SERVER";
String password = "12345678";
String host = "192.168.4.1";

void sendCommand(String cmd, int t) {
  esp.println(cmd);
  delay(t);
}

void sendData(String msg) {
  sendCommand("AT+CIPSTART=\"TCP\",\"" + host + "\",80", 2000);
  sendCommand("AT+CIPSEND=" + String(msg.length()), 1000);
  esp.print(msg);
  delay(1000);
  sendCommand("AT+CIPCLOSE", 500);
}

void setup() {
  Serial.begin(9600);
  esp.begin(115200);

  accel.begin();
  accel.setRange(ADXL345_RANGE_16_G);

  delay(2000);

  sensors_event_t event;
  accel.getEvent(&event);

  x0 = event.acceleration.x;
  y0 = event.acceleration.y;
  z0 = event.acceleration.z;

  sendCommand("AT", 2000);
  sendCommand("AT+CWMODE=1", 1000);
  sendCommand("AT+CWJAP=\"" + ssid + "\",\"" + password + "\"", 5000);
}

void loop() {

  sensors_event_t event;
  accel.getEvent(&event);

  float dx = abs(event.acceleration.x - x0);
  float dy = abs(event.acceleration.y - y0);
  float dz = abs(event.acceleration.z - z0);

  L1 = (dx > threshold || dy > threshold || dz > threshold);

  int waterValue = analogRead(waterPin);
  F1 = (waterValue > waterThreshold);

  String msg = "<";
  msg += (L1 ? "1" : "0");
  msg += (F1 ? "1" : "0");
  msg += ">";

  Serial.print("L1=");
  Serial.print(L1);
  Serial.print(" F1=");
  Serial.print(F1);
  Serial.print(" | Sending=");
  Serial.println(msg);

  sendData(msg);

  delay(2000);
}