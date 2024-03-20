// DIP 5,6 ON
// DIP 7 ON for upload, then OFF
// use TXD0
// I think 1,2 ON for AT mode
// WIFI here


#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <AutoConnect.h>
#include <WiFiClientSecure.h>

const char* ssid = "VGurbani";
const char* pass = "JuFo2024";
const char* host = "";
const uint16_t port = 80;

bool useHotSpot = false;
IPAddress myIP;
ESP8266WebServer server(80);
WiFiClientSecure client;


// MDNSResponder mdns;
void handle_NotFound(){
  // no clue what they sent
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
}
void alertNewPerson(String discordMsg) {
  HTTPClient http;
  
  http.begin(client, "REDACTED_DISCORD_WEBHOOK");
  http.addHeader("Content-Type", "application/json");

  String payload = "{\"username\": \"YOUR HOME\", \"content\": \""+discordMsg+ "\", \"tts\":true }";
  
  int httpResponseCode = http.POST(payload);
  
  if (httpResponseCode > 0) {
    // Serial2.print("Webhook message sent. Response code: ");
    // Serial2.println(httpResponseCode);
    if (httpResponseCode == HTTP_CODE_OK || httpResponseCode == HTTP_CODE_MOVED_PERMANENTLY) { // Todo OK
      // String payload = https.getString();
      // Serial.print("[HTTP] Response: ");
      // Serial.println(payload);
    } 
  } else {
    // Serial2.print("Error sending webhook message. Error code: ");
    // Serial2.println(httpResponseCode);
    // Serial2.println(https.errorToString(httpResponseCode).c_str());
  }
  http.end();
}
void setup() {
  

  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  client.setInsecure();
  if(!useHotSpot) {
    WiFi.mode(WIFI_STA);
    // esp_wifi_set_ps(WIFI_PS_NONE);
    WiFi.begin(ssid, pass);
    delay(500);  
    // Serial.print("WiFi is ");
    // Serial.println(WiFi.status() == WL_CONNECTED); 
    while(WiFi.status() != WL_CONNECTED) {
      // Serial.print(".");
      delay(80);
    }
    myIP = WiFi.localIP();
  } else {
    WiFi.softAP("JuFo Victor Gurbani", "LichtKontrolle"); 
    myIP = WiFi.softAPIP();
  }
  alertNewPerson("House is turning on... ");
  if (MDNS.begin("varfield"/*/, WiFi.localIP()/*/)) {
    // Serial.print("MDNS responder started for ");
    // Serial.println(myIP);
  }
  server.onNotFound(handle_NotFound);

  server.on("/", [](){
    // Serial.print("omg");
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/html", R"rawliteral(
      <head>
      <meta name="viewport" content="width=device-width, initial-scale=1" />
      </head>
      <body>
    <img src="https://www.jugend-forscht.de/fileadmin/assets/img/logo_jugend-forscht_wir-foerdern-talente.svg"><h1>Victor Gurbani</h1>
    <a href='/get'>GET info</a>
    </body>
    )rawliteral");
    delay(10);
  });
  server.on("/get", [](){
    // server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    // server.sendHeader("Transfer-Encoding", "chunked");
    server.sendHeader("Access-Control-Allow-Origin", "*");
    // server.sendHeader("Content-Type", "text/json");
    // server.send(200);

    Serial.print("j");
    int limitTime = 0;
    bool getWorked = true;
    while (!Serial.available()) {
      limitTime++;
      delay(20);
      if(limitTime > 250){
        getWorked = false;
      }
    }
    if(!getWorked) {
      server.send(200, "", "{error}");
    } else {
      delay(150); // wait for finish buffering
      String webPage = ""; // TODO ver pq se come el primer byte
      // server.streamFile(Serial);
      // Stream data from Serial to client
      // while (Serial.available()) {
      //   char c = Serial.read();
      //   server.sendContent(String(c));
      // }
      // server.sendContent(""); 
      while (Serial.available()) { // TODO antes era while
        char inChar = Serial.read();
        // String webp = "";
        // webp += inChar;
        // server.send(200, "", webp);
        // webPage += Serial.read();
        webPage += Serial.readStringUntil('}');
      }
      
      server.send(200, "text/json", '{' + webPage + '}');
    }
    delay(10);
  });
  server.on("/send", [](){
    server.sendHeader("Access-Control-Allow-Origin", "*");
    String message = "OK";
    if (server.arg("cmd")== ""){     //Parameter not found

      message = "Command Argument not found (use cmd)";

    } else {
      Serial.print(server.arg("cmd"));
    }
    
      
    server.send(200, "text/plain", message);
    
    delay(10);
  });
  
  server.begin();
  // Serial.println("HTTP server started");

  MDNS.addService("http", "tcp", 80);


  alertNewPerson("House is ON ");
}

void loop() {
  MDNS.update();
  server.handleClient();

  // alert Dsc
  if(Serial.available()) {
    // delay(800); // wait to recieve and check if not server
    // String msg = "";  
    // while (Serial.available()) {
    //   // char inChar = Serial.read();
    //   // char inChar = "a";
    //   // msg += inChar;
    //   msg = "";
    //   if(msg == "alarm") {
    //     alertNewPerson("Intruder Detected!");
    //   }
    // }
    if(Serial.read() == 'a') {
      alertNewPerson("Intruder Detected!");
    }
    
  }
}


  

/* void httpReq() {
  // Send and store data
  HTTPClient http;
  
  http.begin("VictorMBP.local");
  http.addHeader("Content-Type", "application/json");
  
  int httpResponseCode = http.POST(JSONdata);
  
  if (httpResponseCode > 0) {
    //Serial.print("Data message sent. Response code: ");
    // Serial.println(httpResponseCode);
  } else {
    Serial.print("Error sending data message. Error code: ");
    Serial.println(httpResponseCode);
  }
  
  http.end();
} */

/* void alertNewPerson() {
  HTTPClient http;
  
  http.begin(webhookURL);
  http.addHeader("Content-Type", "application/json");

  String payload = "{\"username\": \"test\", \"content\": \"hello\"}";
  
  int httpResponseCode = http.POST(payload);
  
  if (httpResponseCode > 0) {
    // Serial.print("Webhook message sent. Response code: ");
    Serial.println(httpResponseCode);
  } else {
    // Serial.print("Error sending webhook message. Error code: ");
    Serial.println(httpResponseCode);
  }
  
  http.end();
}
*/ 
// END NEW CODE


/* Quellen:
 *  
 *  https://diyi0t.com/mh-z14a-co2-meter-arduino-esp8266-esp32/
 *  
 *  
 */


// App open window on notification, threshold and everything, fixed PWM inteerface
