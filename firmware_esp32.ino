/*
 * Smart Plant Monitor — ESP32 firmware
 * បញ្ចេញ JSON នៅ http://<ESP32-IP>/data  សម្រាប់ dashboard (index.html)
 *
 * ឧបករណ៍:
 *   - Capacitive soil moisture sensor  → GPIO 34 (analog)
 *   - DHT22 (temp + humidity)          → GPIO 4
 *   - BH1750 (light, I2C)              → SDA 21 / SCL 22
 *   - EC sensor (DFRobot SEN0161)      → GPIO 35 (analog)
 *   - Relay + water pump (optional)    → GPIO 26
 *
 * Libraries (Arduino IDE → Library Manager):
 *   DHT sensor library (Adafruit), Adafruit Unified Sensor,
 *   BH1750 (Christopher Laws)
 *
 * Board: "ESP32 Dev Module"  ·  Baud 115200
 */

#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>
#include <Wire.h>
#include <BH1750.h>

// ---------- WiFi ----------
const char* WIFI_SSID = "Mariang";
const char* WIFI_PASS = "YOUR_WIFI_PASSWORD";

// ---------- Pins ----------
#define PIN_SOIL   34
#define PIN_EC     35
#define PIN_DHT     4
#define PIN_RELAY  26
#define DHT_TYPE   DHT22

// ---------- Calibration (សូមកែតាមការវាស់ពិត) ----------
const int SOIL_DRY = 2600;   // តម្លៃ analogRead ពេលដីស្ងួតស្ងួត (ខ្យល់)
const int SOIL_WET = 1100;   // តម្លៃ analogRead ពេលដីសើមជោគ
const float EC_K   = 1.0;    // មេគុណកែ EC តាមការធៀបស្តង់ដារ

// ---------- Auto watering ----------
const int   WATER_ON_BELOW = 28;   // ស្រោចពេលសំណើម < 28%
const int   WATER_OFF_ABOVE = 55; // ឈប់ពេលសំណើម >= 55%
const unsigned long MAX_PUMP_MS = 60000; // សុវត្ថិភាព: មិនលើស ៦០ វិនាទី
bool pumpOn = false;
unsigned long pumpStart = 0;

DHT dht(PIN_DHT, DHT_TYPE);
BH1750 lightMeter;
WebServer server(80);

struct Reading { float moisture, temp, hum, lux, ec; };
Reading R = {0, 0, 0, 0, 0};

void readSensors() {
  // --- soil moisture (%) ---
  long sum = 0;
  for (int i = 0; i < 8; i++) { sum += analogRead(PIN_SOIL); delay(6); }
  int raw = sum / 8;
  float m = (float)(SOIL_DRY - raw) * 100.0 / (float)(SOIL_DRY - SOIL_WET);
  R.moisture = constrain(m, 0.0, 100.0);

  // --- DHT22 ---
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (!isnan(t)) R.temp = t;
  if (!isnan(h)) R.hum  = h;

  // --- BH1750 ---
  float lux = lightMeter.readLightLevel();
  if (lux >= 0) R.lux = lux;

  // --- EC (mS/cm) with temperature compensation ---
  long esum = 0;
  for (int i = 0; i < 8; i++) { esum += analogRead(PIN_EC); delay(6); }
  float v = (esum / 8.0) / 4095.0 * 3.3;               // volts
  float ecRaw = (133.42 * v*v*v - 255.86 * v*v + 857.39 * v) * 0.5; // µS/cm
  float comp = 1.0 + 0.02 * (R.temp - 25.0);           // temp compensation
  R.ec = (ecRaw / 1000.0) * comp * EC_K;
  if (R.ec < 0) R.ec = 0;
}

void handleData() {
  readSensors();
  String j = "{";
  j += "\"moisture\":" + String(R.moisture, 1) + ",";
  j += "\"temp\":"     + String(R.temp, 1)     + ",";
  j += "\"hum\":"      + String(R.hum, 1)      + ",";
  j += "\"lux\":"      + String(R.lux, 0)      + ",";
  j += "\"ec\":"       + String(R.ec, 2)       + ",";
  j += "\"pump\":"     + String(pumpOn ? 1 : 0);
  j += "}";
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "application/json", j);
}

void handleRoot() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain; charset=utf-8",
    "Smart Plant Monitor OK\n/data  -> JSON\nIP: " + WiFi.localIP().toString());
}

void autoWater() {
  if (!pumpOn && R.moisture < WATER_ON_BELOW) {
    digitalWrite(PIN_RELAY, HIGH); pumpOn = true; pumpStart = millis();
  } else if (pumpOn && (R.moisture >= WATER_OFF_ABOVE || millis() - pumpStart > MAX_PUMP_MS)) {
    digitalWrite(PIN_RELAY, LOW);  pumpOn = false;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_RELAY, OUTPUT); digitalWrite(PIN_RELAY, LOW);
  dht.begin();
  Wire.begin(21, 22);
  lightMeter.begin();

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting WiFi");
  unsigned long t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < 20000) {
    delay(400); Serial.print(".");
  }
  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Connected. Open: http://");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi failed — check SSID/password");
  }

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();
}

void loop() {
  server.handleClient();
  static unsigned long last = 0;
  if (millis() - last > 3000) {   // អានរាល់ ៣ វិនាទី
    last = millis();
    readSensors();
    autoWater();
    Serial.printf("M=%.1f%% T=%.1fC H=%.0f%% L=%.0flux EC=%.2f pump=%d\n",
                  R.moisture, R.temp, R.hum, R.lux, R.ec, pumpOn);
  }
}
