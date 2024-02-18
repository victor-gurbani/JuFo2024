// DIP 5,6 ON
// DIP 7 ON for upload, then OFF
// use TXD0
// I think 1,2 ON for AT mode
// WIFI here
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <AutoConnect.h>

const char* ssid = "Garfield";
const char* pass = "19792008";
const char* host = "";
const uint16_t port = 80;

// const bool 

ESP8266WebServer server(80);
MDNSResponder mdns;
void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}
void setup() {

  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  delay(500);
  Serial.print("WiFi is ");
  Serial.println(WiFi.status() == WL_CONNECTED); 
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  if (mdns.begin("esp8266"/*/, WiFi.localIP()/*/)) {
    Serial.print("MDNS responder started for ");
    Serial.println(WiFi.localIP());
  }
  server.onNotFound(handle_NotFound);

  server.on("/", [](){
    
    server.send(200, "text/html", "Works!");
  });
  server.on("/get", [](){
    Serial.println("j");
    while (!Serial.available()) {
    ;
    }
    String webPage = "<p>asdf</p>";
    while (Serial.available()) {
     
      char inChar = Serial.read();
      webPage += inChar;
    }
    
    server.send(200, "text/html", webPage);
    delay(500);
  });
  
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
  /*
   *
   * 
   * g = get
   *  a = analog
   *  p = PWM
   *  u = UART
   *  l = lights as JSON array{[]}
   *  m = time since last motion in ms
   * s = set
   *  a = analog
   *  p = PWM
   *  u = UART
   *  m = motor (automatic Window opening)
   *    1 = ON (use)
   *    0 = OFF (do not use)
   * m = set motor
   *   1 = ON (use)
   *   0 = OFF (do not use)
   * l = set light
   *  n = lightPin
   *   1 = ON
   *   0 = OFF
   * a = allowed sources
   * t = threshold
   *  [1-99] = number (suggested max 40-60%) // 50 is normal
   *  
   * w = windows
   *  [0-TotalWindows] = window id 
   *    returns 0 or 1
   *    
   * e = errorcodes
   * 
   *  ERROR CODES:
   *  1 - Missing Info
   *  2 - Wrong Info
   * 
  if (Serial.available()) {
    SerialChar = Serial.read();
    // Serial.println("--------------------");
    // Serial.println(SerialChar);
    switch (SerialChar) {
      case 'h':
        Serial.println("help menu");
        break;
      case 'g':  // get
        if (Serial.available()) {
          SerialChar = Serial.read();
          // Serial.println(SerialChar);
          if (SerialChar == 'a') {
            Serial.println(ppm_analog);
          } else if (SerialChar == 'p') {
            Serial.println(ppm_PWM);
          } else if (SerialChar == 'u') {
            Serial.println(ppm_uart);
          } else if (SerialChar == 'l') {
            Serial.println("{ " + toJSON(LightSensorState, TotalLightSensors) + " }");
          } else if (SerialChar == 'm') {
            Serial.println(millis() - lastMotion);
          } else {
            Serial.println("ERROR (Code: 2)");
          }
        } else {
          Serial.println("ERROR (Code: 1)");
        }
        break;
      case 's':  // set
        if (Serial.available()) {
          SerialChar = Serial.read();
          //Serial.println(SerialChar);
          if (Serial.available()) {
            ControlInt = Serial.read() - 48;
            ControlInt = (bool)ControlInt;
            // Serial.println(ControlInt);
          } else {
            Serial.println("ERROR (Code: 1)");
          }
          if (SerialChar == 'a') {
            AllowedSource[0] = ControlInt;
          } else if (SerialChar == 'p') {
            AllowedSource[1] = ControlInt;
          } else if (SerialChar == 'u') {
            AllowedSource[2] = ControlInt;
          } else if (SerialChar == 'm') {
            motorAllowed = ControlInt;
          } else {
            Serial.println("ERROR (Code: 2)");
            break;
          }
          SerialJSON("OK");
        } else {
          Serial.println("ERROR (Code: 1)");
        }
        break;
      case 'm':  // use servo/motor 1 open:
                 //      if (Serial.available()) {
                 //           ControlInt = Serial.read() - 48;
                 //           ControlInt = (bool)ControlInt;
                 //           Serial.println(ControlInt);
                 //           motorAllowed = ControlInt;
                 //           Serial.println("OK");
                 //         } else {
                 //           Serial.println("ERROR (Code: 1)");
                 //         }
        if (Serial.available()) {
          ControlInt = Serial.read() - 48;
          ControlInt = (bool)ControlInt;
          // Serial.println(ControlInt);
          if (ControlInt) {
            if (current_angle != 1200) {
              for (current_angle = 1750; current_angle > 1200; current_angle -= 1) {
                myservo.write(current_angle);
                delay(2);
              }
            }
          } else {
            if (current_angle != 1750) {
              for (current_angle = 1200; current_angle < 1750; current_angle += 1) {
                myservo.write(current_angle);
                delay(2);
              }
            }
          }
          last_used = millis();
          SerialJSON("OK");
        } else {
          Serial.println("ERROR (Code: 1)");
        }
        break;
      case 'l':
        if (Serial.available()) {
          int ChosenLight = (int)Serial.read();
          if (Serial.available()) {
            ControlInt = Serial.read() - 48;
            ControlInt = (bool)ControlInt;
            // Serial.println(ControlInt);
            if(ChosenLight < TotalLights) {
              digitalWrite(ChosenLight, ControlInt ? HIGH : LOW);            
            } else {
              Serial.println("ERROR (Code: 2)");
            }
            SerialJSON("OK");
          } else {
            Serial.println("ERROR (Code: 1)");
          }
        } else {
          Serial.println("ERROR (Code: 1)");
        }
        break;
      case 'a':
        Serial.print("Analog ");
        Serial.println(AllowedSource[0]);
        Serial.print("PWM ");
        Serial.println(AllowedSource[1]);
        Serial.print("UART ");
        Serial.println(AllowedSource[2]);
        break;
      case 't':  // threshold between 40-60% (d = default)
        if (!Serial.available()) {
          Serial.println("ERROR (Code: 1)");
        } else {
          SerialChar = Serial.read();
          if (SerialChar == 'd') {
            threshold = 100;  // in %
          } else {
            int n1 = (int)(SerialChar - 48);
            int n2;
            if (Serial.available()) {
              SerialChar = Serial.read();  //warning NL and CR (ASCII 10,13)
              n2 = (int)(SerialChar - 48);
            } else {
              Serial.println("ERROR (Code: 1)");
              break;
            }

            threshold = (n1 * 10 + n2) * 2;  // los numeros son * 2 pq 50 son 100
          }
        }
        //Serial.println(threshold);
        break;
      case 'w':
        if (Serial.available())
          ControlInt = (int)Serial.read();
        else {
          Serial.println("ERROR (Code: 1)");
          break;
        }
        if (ControlInt <= TotalVentanas)
          Serial.println(VentanaState[ControlInt]);
        else
          Serial.println("ERROR (Code: 2)");
        break;
      case 'e':
        Serial.println("1 - Missing Info\n2 - Wrong Info");
        break;
      case 'j':
        SerialJSON("");
        break;
      default:  // anything else
        // Serial.println("ERROR WTF");
        break;
    }
    // Serial.println("--------------------");
  }
}

void httpReq() {
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
} 

void alertNewPerson() {
  HTTPClient http;
  
  http.begin(webhookURL);
  http.addHeader("Content-Type", "application/json");

  String payload = "{\"username\": \"test\", \"content\": \"hello\"}";
  
  int httpResponseCode = http.POST(payload);
  
  if (httpResponseCode > 0) {
    Serial.print("Webhook message sent. Response code: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.print("Error sending webhook message. Error code: ");
    Serial.println(httpResponseCode);
  }
  
  http.end();
}

// END NEW CODE


/* Quellen:
 *  
 *  https://diyi0t.com/mh-z14a-co2-meter-arduino-esp8266-esp32/
 *  
 *  
 */


// App open window on notification, threshold and everything, fixed PWM inteerface
