#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include "secrets.h"

#define trigPin D4 // Blue LED
#define echoPin D1
#define homePin D0 // Red LED

// Imported from secrets.h, you will need to define your own for your setup!
extern const char* ssid;
extern const char* password;
extern String server;

// State variable to make sure we don't keep sending HTTP requests
bool previouslyInRange = false;

// Function prototypes
void output_distance();
int ping();

// the setup function runs once when you press reset or power the board
void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(homePin, OUTPUT);

  // Trig pin is blue LED, "home" pin is red,
  // logic levels are inverted
  digitalWrite(trigPin, LOW);
  digitalWrite(homePin, HIGH);
  
  Serial.begin(9600);

  // Connect to WiFi!
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Waiting for someone to come home...");
}

// the loop function runs over and over again forever
void loop() {
  int distance = ping();
  // If your sensor is not registering,
  // check your output by uncommenting this line
  // outputDistance(distance);

  // Update our "is person home" knowledge
  bool shouldUpdate = false;
  if (distance < 5 && distance > 0 && !previouslyInRange) {
    previouslyInRange = true;
    shouldUpdate = true;
  } else if ((distance >= 5 || distance <= 0) && previouslyInRange) {
    previouslyInRange = false;
    shouldUpdate = true;
  }

  // If our state has changed, send an update to the server!
  if (shouldUpdate && WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    String serverPath = server + (previouslyInRange ? "/nowHome" : "/leftHome");

    http.begin(client, serverPath.c_str());

    int httpResponse = http.GET();
    if (httpResponse > 0) {
      Serial.print("HTTP Response: ");
      Serial.println(httpResponse);
      String payload = http.getString();
      Serial.println(payload);

      if (previouslyInRange) {
        digitalWrite(homePin, LOW);
      } else {
        digitalWrite(homePin, HIGH);
      }
    } else {
      Serial.print("HTTP Error! code: ");
      Serial.println(httpResponse);
    }
    
    // Free resources
    http.end();
  }

  // Wait for a second before re-scanning for people
  delay(1000);
}

// Print distance from sensor nicely
void outputDistance(int distance) {
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
}

// Return distance from sensor in cm
int ping() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  return (int)(duration * 0.034 / 2);
}
