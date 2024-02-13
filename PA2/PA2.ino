#include "DHT.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#define BLYNK_TEMPLATE_ID "TMPL6gxu3518x"
#define BLYNK_TEMPLATE_NAME "Maggot"
#define BLYNK_AUTH_TOKEN "x8TWWH1euaoEL8cBkEkjjJC_vvwYTy7E"

DHT dht(4, DHT11);
char ssid[] = "jambire";
char pass[] = "12345678";

#define SOIL_MOISTURE_PIN 34
#define RELAY_PIN 32

BlynkTimer timer;
int relayState = LOW;

void sendNotification(String message) {
  Blynk.virtualWrite(V4, message);
}

BLYNK_WRITE(V3)
{
  relayState = param.asInt();
  digitalWrite(RELAY_PIN, relayState);
  if (relayState == 1) {
    Serial.println("Relay terbuka");
    Blynk.virtualWrite(V0, HIGH);
  } else {
    Serial.println("Relay tertutup");
    Blynk.virtualWrite(V0, LOW);
  }
}

void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);

  Serial.begin(9600);
  dht.begin();

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop() {
  Blynk.run();
  timer.run();

  delay(2000);
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float sm = analogRead(SOIL_MOISTURE_PIN) / 4095.0 * 100.0;

  if (sm > 80) {
    digitalWrite(RELAY_PIN, LOW);
    Serial.println("Relay turned on");
    sendNotification("Kelembaban tanah melebihi batas!");
  } else {
    digitalWrite(RELAY_PIN, HIGH);
    Serial.println("Relay turned off");
  }

  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print("°C ");
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print("% ");
  Serial.print("Soil Moisture: ");
  Serial.print(sm);
  Serial.print("%");
  Serial.println();

  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);
  Blynk.virtualWrite(V2, sm);
}
