#include <HTTP_Method.h>
#include <Uri.h>
#include <WebServer.h>
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <mdns.h>
#include <WiFi.h>

const char* kSsid = "IOT_Devices";
const char* kPassword = "TurnMe0n!";

#if defined(ESP8266)
ESP8266WebServer server(80);
#undef HOSTNAME
#define HOSTNAME "IRBlaster"
#endif  // ESP8266
#if defined(ESP32)
WebServer server(80);
#undef HOSTNAME
#define HOSTNAME "IRBlaster"
#endif  // ESP32

const uint16_t kIrLed = 4;  // ESP GPIO pin to use. Recommended: 4 (D2).

IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.

void handleRoot() {
  server.send(200, "text/html",
              "<html>" \
                "<head><title>" HOSTNAME " </title>" \
                "<meta http-equiv=\"Content-Type\" " \
                    "content=\"text/html;charset=utf-8\">" \
                "</head>" \
                "<body>" \
                  "<h1>Control All IR Devices with " HOSTNAME ".</h1>" \
                  "<p><a href=\"ir?code=33464415\">OUT1 IN1</a></p>" \
                  "<p><a href=\"ir?code=33448095\">OUT1 IN2</a></p>" \
                  "<p><a href=\"ir?code=33460335\">OUT1 IN3</a></p>" \
                  "<p><a href=\"ir?code=33444015\">OUT1 IN4</a></p>" \
                  "<p><a href=\"ir?code=33465435\">OUT2 IN1</a></p>" \
                  "<p><a href=\"ir?code=33449115\">OUT2 IN2</a></p>" \
                  "<p><a href=\"ir?code=33481755\">OUT2 IN3</a></p>" \
                  "<p><a href=\"ir?code=33428715\">OUT2 IN4</a></p>" \
                "</body>" \
              "</html>");
}

void handleIr() {
  for (uint8_t i = 0; i < server.args(); i++) {
    if (server.argName(i) == "code") {
      uint32_t code = strtoul(server.arg(i).c_str(), NULL, 10);
#if SEND_NEC
      irsend.sendNEC(code, 32);
#endif  // SEND_NEC
    }
  }
  handleRoot();
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++)
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  server.send(404, "text/plain", message);
}

void setup(void) {
  irsend.begin();

  Serial.begin(115200);
  WiFi.begin(kSsid, kPassword);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(kSsid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP().toString());

  server.on("/", handleRoot);
  server.on("/ir", handleIr);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}
