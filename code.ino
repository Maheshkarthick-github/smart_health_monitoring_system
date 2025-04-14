#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT22.h>

// Wi-Fi credentials
const char* ssid = "curdrice";
const char* password = "strong password";

// Firebase credentials
const char* firebaseHost = "https://iothealth-834a3-default-rtdb.asia-southeast1.firebasedatabase.app/";
const char* firebaseAuth = "wfmVFv8Tkgk2iv6osjsAxLJ0FNRDc5rWzwggMzwG";

// Pin definitions
#define DHT22_PIN 4
#define MQ135_PIN 34
#define AD8232_PIN 32
#define HEARTBEAT_PIN 33  // New heartbeat sensor pin

// DHT22 object
DHT22 dht(DHT22_PIN);

// Timing
unsigned long previousMillis = 0;
const long interval = 2000; // 2 seconds

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Sensor readings
    int mq135Value = analogRead(MQ135_PIN);
    float temperature = dht.getTemperature();
    float humidity = dht.getHumidity();
    int ecgValue = analogRead(AD8232_PIN);
    int heartbeatValue = analogRead(HEARTBEAT_PIN);

    Serial.println("---- Sensor Readings ----");
    Serial.print("MQ135: "); Serial.println(mq135Value);
    Serial.print("Temp: "); Serial.print(temperature); Serial.println(" °C");
    Serial.print("Humidity: "); Serial.print(humidity); Serial.println(" %");
    Serial.print("ECG: "); Serial.println(ecgValue);
    Serial.print("Heartbeat Sensor: "); Serial.println(heartbeatValue);

    // Send to Firebase
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      String firebasePath = String(firebaseHost) + "/sensorData.json?auth=" + firebaseAuth;

      http.begin(firebasePath);
      http.addHeader("Content-Type", "application/json");

      String json = "{";
      json += "\"temperature\":" + String(temperature, 2) + ",";
      json += "\"humidity\":" + String(humidity, 2) + ",";
      json += "\"mq135\":" + String(mq135Value) + ",";
      json += "\"ecg\":" + String(ecgValue) + ",";
      json += "\"heartbeat\":" + String(heartbeatValue);
      json += "}";

      int httpResponseCode = http.PUT(json);
      Serial.print("Firebase Response Code: ");
      Serial.println(httpResponseCode);
      http.end();
    } else {
      Serial.println("WiFi not connected");
    }

    Serial.println("--------------------------");
  }
}
