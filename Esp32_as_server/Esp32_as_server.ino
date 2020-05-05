// Loading of the Wi-Fi library
#include <WiFi.h>
// Replace with your network credentials
// In this case I have used my hotspot phone Wi-Fi
const char* ssid = "Charost";
const char* password = "pepito1992";
// Set web server port number to 80
WiFiServer server(80);
// Variable to store the HTTP request
String header;
//creation of  variables for the motor input pins and enable pins
// Motor 1
int motor1Pin1 = 27;
int motor1Pin2 = 26;
int enable1Pin = 14;
// Motor 2
int motor2Pin1 = 33;
int motor2Pin2 = 25;
int enable2Pin = 32;
// Setting PWM properties like frequency, the PWM channel, resolution, and duty cycle
const int freq = 30000;
const int pwmChannel = 0;
const int resolution = 8;
int dutyCycle = 0;
// Decode HTTP GET value
String valueString = String(5);
int pos1 = 0;
int pos2 = 0;
void setup() {
Serial.begin(115200);
// Set the Motor pins as outputs
pinMode(motor1Pin1, OUTPUT);
pinMode(motor1Pin2, OUTPUT);
pinMode(motor2Pin1, OUTPUT);
pinMode(motor2Pin2, OUTPUT);
// Configure PWM channel functionalitites
ledcSetup(pwmChannel, freq, resolution);
// Attach the PWM channel 0 to the enable pins which are the GPIOs to be controlled
ledcAttachPin(enable1Pin, pwmChannel);
ledcAttachPin(enable2Pin, pwmChannel);
// Produce a PWM signal to both enable pins with a duty cycle 0
ledcWrite(pwmChannel, dutyCycle);
// Connection to Wi-Fi network with SSID and password
Serial.print("Connecting to ");
Serial.println(ssid);
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
//Print local IP address and start web server
Serial.println("");
Serial.println("WiFi connected.");
Serial.println("IP address: ");
Serial.println(WiFi.localIP());
server.begin();
}
// the ESP32 is always listening for incoming clients, and when a request is
//received we save the incoming data.
void loop(){
WiFiClient client = server.available();
if (client) {
Serial.println("New Client.");
String currentLine = "";
while (client.connected()) {
if (client.available()) {
    char c = client.read();
    Serial.write(c);
    header += c;
    if (c == '\n') {
      if (currentLine.length() == 0) {
        client.println("HTTP/1.1 200 OK");
        client.println("Content-type:text/html");
        client.println("Connection: close");
        client.println();

// Controls the motor pins according to the button pressed
        if (header.indexOf("GET /forward") >= 0) {
          Serial.println("Forward");
          digitalWrite(motor1Pin1, LOW);
          digitalWrite(motor1Pin2, HIGH);
          digitalWrite(motor2Pin1, LOW);
          digitalWrite(motor2Pin2, HIGH);
          } 
        else if (header.indexOf("GET /left") >= 0) {
          Serial.println("Left");
          digitalWrite(motor1Pin1, LOW);
          digitalWrite(motor1Pin2, LOW);
          digitalWrite(motor2Pin1, LOW);
          digitalWrite(motor2Pin2, HIGH);
          } 
       else if (header.indexOf("GET /stop") >= 0) {
          Serial.println("Stop");
          digitalWrite(motor1Pin1, LOW);
          digitalWrite(motor1Pin2, LOW);
          digitalWrite(motor2Pin1, LOW);
          digitalWrite(motor2Pin2, LOW);
          } 
       else if (header.indexOf("GET /right") >= 0) {
          Serial.println("Right");
          digitalWrite(motor1Pin1, LOW);
          digitalWrite(motor1Pin2, HIGH);
          digitalWrite(motor2Pin1, LOW);
          digitalWrite(motor2Pin2, LOW);
          } 
       else if (header.indexOf("GET /reverse") >= 0) {
          Serial.println("Reverse");
          digitalWrite(motor1Pin1, HIGH);
          digitalWrite(motor1Pin2, LOW);
          digitalWrite(motor2Pin1, HIGH);
          digitalWrite(motor2Pin2, LOW);
         }
// Display  of the HTML web page
client.println("<!DOCTYPE HTML><html>");
client.println("<head><meta name=\"viewport\"content=\"width=device-width, initial-scale=1\">");
client.println("<link rel=\"icon\" href=\"data:,\">");
// CSS to style the buttons
client.println("<style>html { font-family: Helvetica; display:inline-block; margin: 0px auto; text-align: center;}");
client.println(".button { -webkit-user-select: none; -moz-userselect:none; -ms-user-select: none; user-select: none; background-color:#4CAF50;");
client.println("border: none; color: white; padding: 12px 28px;text-decoration: none; font-size: 26px; margin: 1px; cursor: pointer;}");
client.println(".button2 {background-color:#555555;}</style>");
client.println("<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script></head>");

// Web Page
client.println("<p><button class=\"button\"onclick=\"moveForward()\">FORWARD</button></p>");
client.println("<div style=\"clear: both;\"><p><button class=\"button\" onclick=\"moveLeft()\">LEFT </button>");
client.println("<button class=\"button button2\"onclick=\"stopRobot()\">STOP</button>");
client.println("<button class=\"button\"onclick=\"moveRight()\">RIGHT</button></p></div>");
client.println("<p><button class=\"button\"onclick=\"moveReverse()\">REVERSE</button></p>");
client.println("<p>Motor Speed: <span id=\"motorSpeed\"></span></p>");
client.println("<input type=\"range\" min=\"0\" max=\"100\"step=\"25\"id=\"motorSlider\" onchange=\"motorSpeed(this.value)\"value=\"" + valueString + "\"/>");
client.println("<script>$.ajaxSetup({timeout:1000});");
client.println("function moveForward() { $.get(\"/forward\");{Connection: close};}");
client.println("function moveLeft() { $.get(\"/left\");{Connection: close};}");
client.println("function stopRobot() {$.get(\"/stop\");{Connection: close};}");
client.println("function moveRight() { $.get(\"/right\");{Connection: close};}");
client.println("function moveReverse() { $.get(\"/reverse\");{Connection: close};}");
client.println("var slider = document.getElementById(\"motorSlider\");");
client.println("var motorP = document.getElementById(\"motorSpeed\"); motorP.innerHTML = slider.value;");
client.println("slider.oninput = function() { slider.value = this.value; motorP.innerHTML = this.value; }");
client.println("function motorSpeed(pos) { $.get(\"/?value=\" +pos + \"&\"); {Connection: close};}</script>");
client.println("</html>");

//In this case, we set the duty cycle to start at 200 
    }
      else {
    dutyCycle = map(valueString.toInt(), 25, 100, 200, 255);
    ledcWrite(pwmChannel, dutyCycle);
    Serial.println(valueString);
       }
}
// The HTTP response ends with another blank line
client.println();
// Break out of the while loop
break;
} 
 else { // if you got a newline, then clear currentLine
    currentLine = "";
   }
  }
} else if (c != '\r') {
    currentLine += c;
    }
}
}
// Clear the header variable
header = "";
// Close the connection
client.stop();
Serial.println("Client disconnected.");
Serial.println("");
}
