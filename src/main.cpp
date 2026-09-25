//include tells the compiler to add libaries to the program 

#include <DHTesp.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include "secrets.h" 

// Pin Definitions

const int DHT_PIN = 26;
const int PIR_PIN = 27;
const int LDR_PIN = 34;

const int LED_PIN = 13;
const int RELAY_PIN = 14;
const int SERVO_PIN = 25;


// Create Objects

DHTesp dht;
Servo fanServo;


// System States

bool coolingOn = false;
bool lightOn = false;


// Cooling Thresholds

const float FAN_ON_TEMP = 28.0;
const float FAN_OFF_TEMP = 26.0;

const float FAN_ON_HUMIDITY = 70.0;
const float FAN_OFF_HUMIDITY = 65.0;


// Rule 3 Timer

unsigned long lastMotionTime = 0;

// 5 minutes = 300,000 milliseconds

const unsigned long INACTIVITY_TIME = 300000;


// Light Threshold

const int LIGHT_THRESHOLD = 25;


// Wi-Fi + ThingSpeak
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;
const char* apiKey = THINGSPEAK_WRITE_API_KEY;
const char* server = "https://api.thingspeak.com/update";

unsigned long lastUpload = 0;

// ThingSpeak free tier needs >= 15s between writes
const unsigned long uploadInterval = 20000;


void setup() {

  Serial.begin(115200);

  // Start DHT22
  dht.setup(DHT_PIN, DHTesp::DHT22);

  // Set sensor pins
  pinMode(PIR_PIN, INPUT);
  pinMode(LDR_PIN, INPUT);

  // Set output pins
  pinMode(LED_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);

  // Start outputs OFF
  digitalWrite(LED_PIN, LOW);
  digitalWrite(RELAY_PIN, LOW);

  // Set up servo
  fanServo.attach(SERVO_PIN);
  fanServo.write(0);

  Serial.println("Smart Home System Starting...");

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected. IP address: ");
  Serial.println(WiFi.localIP());
}


void loop() {

  // Read Sensors

  TempAndHumidity data = dht.getTempAndHumidity();

  int motion = digitalRead(PIR_PIN);

  int ldrRaw = analogRead(LDR_PIN);


  // Convert LDR Reading to Percentage

  int lightPercent = map(ldrRaw, 0, 4095, 100, 0);

  lightPercent = constrain(lightPercent, 0, 100);


  // Check for Motion

  if (motion == HIGH) {

    // Reset inactivity timer whenever motion is detected
    lastMotionTime = millis();
  }


  // =====================================================
  // RULE 1 - Automatic Temperature and Humidity Cooling
  // =====================================================

  // Cooling turns ON when:
  // Motion is detected
  // AND
  // Temperature is above 28C OR humidity is above 70%

  if ((data.temperature > FAN_ON_TEMP ||
       data.humidity > FAN_ON_HUMIDITY) &&
       motion == HIGH) {

    coolingOn = true;

    digitalWrite(RELAY_PIN, HIGH);

    fanServo.write(90);
  }


  // Cooling turns OFF when:
  // No motion is detected
  // OR
  // BOTH temperature and humidity are below
  // their OFF thresholds

  else if (motion == LOW ||
          (data.temperature < FAN_OFF_TEMP &&
           data.humidity < FAN_OFF_HUMIDITY)) {

    coolingOn = false;

    // Return servo to OFF position
    fanServo.write(0);

    // Give servo time to return
    delay(500);

    // Turn relay OFF
    digitalWrite(RELAY_PIN, LOW);
  }


  // =====================================================
  // RULE 2 - Smart Lighting
  // =====================================================

  // Motion detected AND room is dark

  if (motion == HIGH && lightPercent < LIGHT_THRESHOLD) {

    lightOn = true;

    digitalWrite(LED_PIN, HIGH);
  }


  // If there is enough natural light,
  // the light is not needed

  else if (lightPercent >= LIGHT_THRESHOLD) {

    lightOn = false;

    digitalWrite(LED_PIN, LOW);
  }


  // =====================================================
  // RULE 3 - Five-Minute Auto Light-Off
  // =====================================================

  // Only check inactivity if there is currently
  // no motion and the light is ON

  if (motion == LOW && lightOn == true) {

    // Calculate how long it has been
    // since motion was last detected

    unsigned long noMotionTime = millis() - lastMotionTime;


    // Turn light OFF after 5 minutes

    if (noMotionTime >= INACTIVITY_TIME) {

      lightOn = false;

      digitalWrite(LED_PIN, LOW);
    }
  }


  // =====================================================
  // Display Sensor Readings
  // =====================================================

  Serial.print("Temperature: ");
  Serial.print(data.temperature);
  Serial.println(" C");


  Serial.print("Humidity: ");
  Serial.print(data.humidity);
  Serial.println(" %");


  Serial.print("Motion: ");

  if (motion == HIGH) {

    Serial.println("Detected");
  }

  else {

    Serial.println("Not Detected");
  }


  Serial.print("LDR Raw Reading: ");
  Serial.println(ldrRaw);


  Serial.print("Light Level: ");
  Serial.print(lightPercent);
  Serial.println(" %");


  // =====================================================
  // Display Cooling Status
  // =====================================================

  Serial.print("Cooling: ");

  if (coolingOn == true) {

    Serial.println("ON");
  }

  else {

    Serial.println("OFF");
  }


  // =====================================================
  // Display Lighting Status
  // =====================================================

  Serial.print("Light: ");

  if (lightOn == true) {

    Serial.println("ON");
  }

  else {

    Serial.println("OFF");
  }


  Serial.println("--------------------");


 // =====================================================
// Upload to ThingSpeak securely (every 20 seconds)
// =====================================================

if (WiFi.status() == WL_CONNECTED && millis() - lastUpload >= uploadInterval) {

  WiFiClientSecure secureClient;
  secureClient.setInsecure();

  HTTPClient http;

  String url = String(server) + "?api_key=" + apiKey +
               "&field1=" + String(data.temperature) +
               "&field2=" + String(data.humidity) +
               "&field3=" + String(motion) +
               "&field4=" + String(lightPercent) +
               "&field5=" + String(coolingOn ? 1 : 0) +
               "&field6=" + String(lightOn ? 1 : 0);

  http.begin(secureClient, url);

  int httpCode = http.GET();

  Serial.print("HTTPS response: ");
  Serial.println(httpCode);

  http.end();

  lastUpload = millis();
}

// Wait 2 second before next reading

delay(2000);
}
