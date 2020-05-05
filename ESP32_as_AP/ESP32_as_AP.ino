#include <WiFi.h>
// You can customize the SSID name and change the password
const char* ssid = " ESP32-Access-Point ";
const char* password = "123456789";
// Set web server port number to 80
WiFiServer server(80);
// Variable to store the HTTP request
String header;
// Motor 1
int motor1Pin1 = 27;
int motor1Pin2 = 26;
int enable1Pin = 14;
// Motor 2
int motor2Pin1 = 33;
int motor2Pin2 = 25;
int enable2Pin = 32;
// Setting PWM properties
const int freq = 30000;
const int pwmChannel = 0;
const int resolution = 8;
int dutyCycle = 0;
// Decode HTTP GET value
String valueString = "0";
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
// Connect to Wi-Fi network with SSID and password
Serial.print("Setting AP (Access Point)...");
// Remove the password parameter, if you want the AP (Access Point) to be open
WiFi.softAP(ssid,password);
IPAddress IP = WiFi.softAPIP();
Serial.print("AP IP address: ");
Serial.println(IP);
server.begin();
}
void loop(){
WiFiClient client = server.available();
if (client) {
Serial.println("New Client.");
String currentLine = "";
while (client.connected()) {
if (client.available()) {
char c = client.read();
header += c;
if (c == '\n') {
if (currentLine.length() == 0) {
client.println("HTTP/1.1 200 OK");
client.println("Content-type:text/html");
client.println("Connection: close");
client.println();
// turns the GPIOs on and off
if (header.indexOf("GET /forward") >= 0) {
Serial.println("Forward");
digitalWrite(motor1Pin1, LOW);
digitalWrite(motor1Pin2, HIGH);
digitalWrite(motor2Pin1, LOW);
digitalWrite(motor2Pin2, HIGH);
} else if (header.indexOf("GET /left") >= 0) {
  Serial.println("Left");
digitalWrite(motor1Pin1, LOW);
digitalWrite(motor1Pin2, LOW);
digitalWrite(motor2Pin1, LOW);
digitalWrite(motor2Pin2, HIGH);
} else if (header.indexOf("GET /stop") >= 0) {
Serial.println("Stop");
digitalWrite(motor1Pin1, LOW);
digitalWrite(motor1Pin2, LOW);
digitalWrite(motor2Pin1, LOW);
digitalWrite(motor2Pin2, LOW);
} else if (header.indexOf("GET /right") >= 0) {
Serial.println("Right");
digitalWrite(motor1Pin1, LOW);
digitalWrite(motor1Pin2, HIGH);
digitalWrite(motor2Pin1, LOW);
digitalWrite(motor2Pin2, LOW);
} else if (header.indexOf("GET /reverse") >= 0) {
Serial.println("Reverse");
digitalWrite(motor1Pin1, HIGH);
digitalWrite(motor1Pin2, LOW);
digitalWrite(motor2Pin1, HIGH);
digitalWrite(motor2Pin2, LOW);
}
// Display the HTML web page
client.println("<!DOCTYPE HTML><html>");
client.println("<head><metaname=\"viewport\"content=\"width=device-width,initial-scale=1\">");
client.println("<link rel=\"icon\" href=\"data:,\">");
// CSS to style the on/off buttons
// Feel free to change the background-color and font-size attributes to fit your preferences
client.println("<style>html { font-family: Helvetica; display:inline-block; margin: 0px auto; text-align: center;}");
client.println(".button { -webkit-user-select: none; -moz-userselect:none; -ms-user-select: none; user-select: none; background-color:#4CAF50;");
client.println("border: none; color: white; padding: 12px 28px;text-decoration: none; font-size: 26px; margin: 1px; cursor: pointer;}");
client.println(".button2 {background-color: #555555;}</style></head>");
// Web Page
client.println("<p><ahref=\"/forward\"><buttonclass=\"button\" onclick=\"moveForward()\">FORWARD</button></a></p>");
client.println("<divstyle=\"clear:both;\"><p><ahref=\"/left\"><buttonclass=\"button\" onclick=\"moveLeft()\">LEFT</button></a>");
client.println("<ahref=\"/stop\"><buttonclass=\"buttonbutton2\" onclick=\"stopRobot()\">STOP</button></a>");
client.println("<ahref=\"/right\"><button class=\"button\"onclick=\"moveRight()\">RIGHT</button></a></p></div>");
client.println("<p><ahref=\"/reverse\"><button class=\"button\"onclick=\"moveReverse()\">REVERSE</button></a></p>");
client.println("<input type=\"range\" min=\"0\" max=\"100\" step=\"25\" id=\"motorSlider\" onchange=\"motorSpeed(this.value)\"value=\"" + valueString + "\"/>");
client.println("<script> function motorSpeed(pos) { ");
client.println("var xhr = new XMLHttpRequest();");
client.println("xhr.open('GET', \"/?value=\" + pos + \"&\",true);");
client.println("xhr.send(); } </script>");
client.println("</html>");
//Request example: GET /?value=100& HTTP/1.1 - sets PWM duty cycle to 100
if(header.indexOf("GET /?value=")>=0) {
pos1 = header.indexOf('=');
pos2 = header.indexOf('&');
valueString = header.substring(pos1+1, pos2);
//Set motor speed value
if (valueString == "0") {
ledcWrite(pwmChannel, 0);
digitalWrite(motor1Pin1, LOW);
digitalWrite(motor1Pin2, LOW);
digitalWrite(motor2Pin1, LOW);
digitalWrite(motor2Pin2, LOW);
}
else {
dutyCycle = map(valueString.toInt(), 25, 100, 200, 255);
ledcWrite(pwmChannel, dutyCycle);
Serial.println(valueString);
}
}
client.println();
break;
} else {
currentLine = "";
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
}
