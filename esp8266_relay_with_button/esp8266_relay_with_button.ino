#include <ESP8266WiFi.h>

const char *ssid = "<SSID>";
const char *password = "<PWD>";

// defining pin numbers
const int relayPin = 2;

bool currentState = true;

WiFiServer server(80);

void changeState() {
  if (currentState == true) {
    currentState = false;
    digitalWrite(relayPin, HIGH);
    delay(1000);
  } else {
    currentState = true;
    digitalWrite(relayPin, LOW);
    delay(1000);
  }
}

void setup() {
  Serial.begin(9600);
  delay(10);
  pinMode(relayPin, OUTPUT);

  // connecting to WiFi network
  connect_to_wifi();

  // starting the server
  run_server();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connect_to_wifi();
    run_server();

  } else {

    // check if a client has connected
    WiFiClient client = server.available();
    if (!client) {
      if (currentState == false) {
        changeState();
      }

      return;
    }
    // wait until the client sends some data
    Serial.println("new client");
    while (!client.available()) {
      delay(1);
    }

    String request = client.readStringUntil('\r');
    Serial.println(request);
    client.flush();

    if (request.indexOf("/RELAY=OFF") != -1) {
      digitalWrite(relayPin, HIGH);
      currentState = false;
    }

    if (request.indexOf("/RELAY=ON") != -1) {
      digitalWrite(relayPin, LOW);
      currentState = true;
    }

    // return the response
    if ( request.indexOf("/STATUS") != -1) {
      if (currentState) {
        client.print("on");
      } else {
        client.print("off");
      }
    } else {
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println("");
      client.println("<!DOCTYPE HTML>");
      client.println("<html>");
      client.print("Relay is : ");

      if (currentState == true) {
        client.print("On");
      } else {
        client.print("Off");
      }
      client.println("<br><br>");
      client.println("Click <a href=\"/RELAY=ON\">here</a> turn the RELAY on.<br>");
      client.println("Click <a href=\"/RELAY=OFF\">here</a> turn the RELAY off.<br>");
      client.println("</html>");
    }

    delay(1);
    Serial.println("Client disconnected");
    Serial.println("");
  }
}

void connect_to_wifi() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
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
