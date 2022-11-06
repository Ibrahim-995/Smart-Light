#include<ESP8266WiFi.h>

String header;
const int output = D2;
String outputState = "ON";
const char* ssid = "Ibrahim's network";
const char* password = "password995";

WiFiServer server(80);

void setup() {
  Serial.begin(9600);
  pinMode(output, OUTPUT);
  digitalWrite(output, LOW);
  Serial.println("Starting IoT Smart Bulb...");
  delay(500);
  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(ssid, password);
  WiFi.hostname("Smart-Bulb");
  
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.print("Connected to Wi-Fi network: ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  
  if(client) {
    Serial.println("New client request..");
    String currentLine = "";
    
    while(client.connected()) {
      if(client.available()) {
        char c = client.read();
        Serial.write(c);
        header += c;
        
        if(c == '\n') {
          if(currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            if(header.indexOf("GET /D2/off") >= 0) {
              Serial.println("Bulb is OFF");
              outputState = "OFF";
              digitalWrite(output, HIGH);
            }
            else if(header.indexOf("GET /D2/on") >= 0) {
              Serial.println("Bulb is ON");
              outputState = "ON";
              digitalWrite(output, LOW);
            }

            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<style>html {font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button {background-color: #FF0000; border: none; color: black; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #00FF00;}</style></head>");
            client.println("<body><h1>IoT Smart Light</h1>");
            client.println("<p>Current Light State: " + outputState + "</p>");

            if(outputState == "ON") {
              client.println("<p><a href=\"/D2/off\"><button class=\"button\">TURN OFF</button></a></p>");
            }
            else {
              client.println("<p><a href=\"/D2/on\"><button class=\"button button2\">TURN ON</button></a></p>");
            }

            client.println("</body></html>");
            client.println();

            break;
          }
          else {
            currentLine = "";
          }
        }
        else if(c != '\r') {
          currentLine += c;
        }
      }
    }

    header = "";
    client.stop();
    Serial.println("Client disconnected..");
    Serial.println("");
  }
}
