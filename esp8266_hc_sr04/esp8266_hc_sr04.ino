#include <ESP8266WiFi.h>

const char *ssid = "<SSID>";
const char *password = "<PASSWORD>";

// Defining pin numbers
const int triggerPin = 2;
const int echoPulsePin = 0;

long duration;
int distance;

WiFiServer server(80);

void connect_to_wifi();
void run_server();

void setup() {
  Serial.begin(9600);
  delay(10);
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPulsePin, INPUT);

  connect_to_wifi();
  run_server();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connect_to_wifi();
    run_server();
  } else {
    handleClient();
  }
}

void handleClient() {
    // Check if a client has connected
    WiFiClient client = server.available();
    if (!client) {
      return;
    }
    // Wait for the client to send data
    Serial.println("new client");
    while (!client.available()) {
      delay(1);
    }

    String request = client.readStringUntil('\r');
    Serial.println(request);
    client.flush();


    // Clear the trigger pin state
    digitalWrite(triggerPin, LOW);
    delayMicroseconds(2);
    
    // Set the triggerPin HIGH for 10 microseconds
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(triggerPin, LOW);
    
    // Read echo pulse pin (returns the sound wave travel time in microseconds)
    duration = pulseIn(echoPulsePin, HIGH);
    distance = duration * 0.034 / 2;
    
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("");
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
    client.print("Distance meter with ultrasonic HC-SR04 sensor.");
    client.println("<br><br>");
    
    client.print("distance: ");
    client.print(distance);

    client.println("</html>");

    delay(1);
    Serial.println("Client disconnected\n");
 }


void connect_to_wifi() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void run_server() {
  server.begin();
  Serial.println("Server started");

  // Print the IP
  Serial.print("URL : ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}
