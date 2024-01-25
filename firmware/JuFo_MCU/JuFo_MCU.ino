
#include <ESP8266WiFi.h> 
const char* ssid = "";
const char* pass = "";
const char* host = "";
const uint16_t port = 80;

/*
#define A0 54
#define A1 55
#define A2 56
#define A3 57
#define A4 58
#define A5 59
#define A6 60
#define A7 61
#define A8 62
#define A9 63
#define A10 64
#define A11 65
#define A12 66
#define A13 67
#define A14 68
#define A15 69
*/
#include <SoftwareSerial.h>
// #include <Servo.h> // uses Timer1
#include "ServoTimer2.h"
//#include <math.h>
// Jan 11 2024 start
// TODO better data transfer
// light
// light sensor
// will mark new lines with +
// Store data to transfer all together, max size 32 KB, program uses 11KB

// Uart interface sometiemes not working
SoftwareSerial SerialCom(13, 12);  // RX, TX
// SoftwareSerial Bluetooth(8, 10);  // RX, TX

// analog interface very unprecise
const int analogPin = A0;

// PWM interface = can get stuck
const int pwmPin = 10;

// to check if BT is connected
// int bluetoothStatePin = 11? // TODO
const int TotalVentanas = 4;
int VentanaPin[TotalVentanas] = { 2, 4, 5, 7 };
//freepins 3 6 8  11
//usedpins 0 1 2 4 5 9 10 12 13 A0
bool VentanaState[TotalVentanas] = { false, false, false, false };
/*
 * if(ventana1state){
 *  cout<< "ventana abierta!;
 * }
 */


ServoTimer2 myservo;  // create servo object to control a servo
// Servo myservo;  // create servo object to control a servo

bool motorAllowed = true;  // variable to read the value from the analog pin
int current_angle = 42;
unsigned long last_used;


float interval = 0.2;

int co2;

int threshold = 100;  // in %

/*
   * 8.05 - 8.18 EMPIEZA COLE
   * 
   * 8.18 - 9.50 1+2 HORA
   * 
   * 9.54 - 10.06 PAUSA 1
   * 
   * 10.06 - 10.12 VUELTA
   * 
   * 10.12 - 11.45 3+4 HORA
   * 
   * 11.45 - 12.05 PAUSA 2
   * 
   * 12.05 - 2.20 ULTIMAS HORAS
   * 
   *
   * 
   */
int ppmhora[9] = { 900, 800, 600, 850, 800, 600, 800 };
//dummy button  abrir con motor
//slider threshold +-10%
char SerialChar;
int ControlInt;                                // multi-purpose int to read from serial
bool AllowedSource[3] = { true, true, true };  // analog, PWM, Uart

// START NEW CODE
//freepins 8  11
//usedpins 0 1 2 3 4 5 6 9 10 12 13 A0
const int TotalLights = 2;  // ejemplo con ia de que si aprende que una nunca se enciende da igual la situación (tmb como input se puede usar "otras leds encendidas")
int LightPin[TotalLights] = { 3, 6 };
bool LightState[TotalLights] = { false, false };

const int TotalLightSensors = 2;  // ejemplo con ia de que si aprende que una nunc se enciende )(tmb como input se puede usar "otras leds encendidas")
int LightSensorPin[TotalLightSensors] = { A1, A2 };
int LightSensorState[TotalLightSensors] = {}; // will have to multiply by 1000 to be albe to precise


bool lightAllowed = true;  // variable to read the value from the analog pin

int motionSensorPin = 8;
int motionSensorState = LOW;
long int lastMotion = millis();

void setup() {
  Serial.begin(9600);  // esta cambiado !!!
  Serial.println("Machine Initialised");
  /* WIFI WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  delay(500);
  Serial.print("WiFi is ");
  Serial.println(WiFi.status() == WL_CONNECTED); */
  myservo.attach(9);
  last_used = millis();
  for (current_angle = 1750; current_angle > 1200; current_angle -= 1) {
    myservo.write(current_angle);
    delay(2);
  }
  for (current_angle = 1200; current_angle < 1750; current_angle += 1) {
    myservo.write(current_angle);
    delay(2);
  }
  for (current_angle = 1750; current_angle > 1200; current_angle -= 1) {
    myservo.write(current_angle);
    delay(2);
  }
  for (int i = 0; i < TotalVentanas; i++) {
    pinMode(VentanaPin[i], INPUT_PULLUP);
  }

  SerialCom.begin(9600);
  // Serial.begin(9600);
  pinMode(pwmPin, INPUT_PULLUP);

  // START NEW CODE
  for (int i = 0; i < TotalLights; i++) {
    pinMode(LightPin[i], OUTPUT);
    digitalWrite(LightPin[i], HIGH);
  }
  for (int i = 0; i < TotalLightSensors; i++) {
    pinMode(LightSensorPin[i], INPUT);
  }
  // TEST IF WORKS
  delay(500);
  for (int i = 0; i < TotalLights; i++) {
    digitalWrite(LightPin[i], LOW);
  }
  delay(1000);
  digitalWrite(LightPin[0], analogRead(LightSensorPin[0]) > 0.5 ? LOW : HIGH);
  delay(1000);
  digitalWrite(LightPin[0], analogRead(LightSensorPin[0]) > 0.5 ? LOW : HIGH);

  pinMode(motionSensorPin, INPUT);
  

  //delay(180000); // preheat the CO2 sensor for 3 minutes
  delay(1000);
}


int ppm_uart = 0;
int ppm_PWM = 0;
int ppm_analog = 0;

void loop() {
  // Serial.println("true1");

  for (int i = 0; i < TotalVentanas; i++) {
    VentanaState[i] = digitalRead(VentanaPin[i]);
  }

  // START NEW CODE
  for (int i = 0; i < TotalLightSensors; i++) {
    LightSensorState[i] = analogRead(LightSensorPin[i])*1000;
  }
  motionSensorState = digitalRead(motionSensorPin);
  if(motionSensorState && millis() - lastMotion > 20000 /*&& (!digitalRead(bluetoothStatePin))*/) {
    // saliste de casa y llevas 20 segundos fuera sin movimiento y no hay nadie cerca(conectado)
    // TODO connect HC05 state pin
    
    alertNewPerson(); // envia push a dispositivos y ya verán si hay alguien en casa
  }
  lastMotion = motionSensorState ? millis() : lastMotion;

  // END NEW CODE
  //  Serial.print(VentanaState[0]);
  //  Serial.println("<-- Ventana 1");
  //  Serial.print(VentanaState[1]);
  //  Serial.println("<-- Ventana 2");
  //  Serial.print(VentanaState[2]);
  //  Serial.println("<-- Ventana 3");
  //

  if (AllowedSource[2]) {
    //  Serial.println("reading uart...");
    ppm_uart = gas_concentration_uart();
  } else {
    ppm_uart = 0;
  }

  //  Serial.println("reading pwm..."); //not working
  if (AllowedSource[1]) {
    ppm_PWM = gas_concentration_PWM();
  } else {
    ppm_PWM = 0;
  }

  if (AllowedSource[0]) {
    //  Serial.println("reading analog..."); //not accurate
    ppm_analog = get_analog();
  } else {
    ppm_analog = 0;
  }

  /* Print Serial Info
  //  Serial.print(ppm_uart);
  //  Serial.println(" <-- UART");
  //  Serial.print(ppm_PWM);
  //  Serial.println(" <-- PWM");
  //  Serial.print(ppm_analog);
  //  Serial.println(" <-- Analog");
  //  Serial.println("--------------------");*/
  int number_of_closed = 0;
  for (int i = 0; i < TotalVentanas; i++) {
    if (VentanaState[i] == 0) {
      number_of_closed++;
    }
  }

  // automatic opening
  // Serial.println(TotalVentanas / 2);
  int main_ppm = (ppm_PWM != 0) ? ppm_PWM : ppm_uart;

  if (main_ppm > /*ppmhora[hora]*/ 1000 * threshold / 100) {  // cambiar entre (desde app) 80% y 120% (40-60*2)
    //Serial.println(current_angle);
    if (number_of_closed >= TotalVentanas / 2 && millis() - last_used > 3000 && current_angle != 1200) {
      SerialJSON("Open Windows");
      if (motorAllowed) {
        for (current_angle = 1750; current_angle > 1200; current_angle -= 1) {
          myservo.write(current_angle);
          delay(5);
        }
        // current_angle = 90;
        last_used = millis();
      }
      //abrir ventanas
    }
  } else {
    if (/*number_of_closed < TotalVentanas / 2 && */ millis() - last_used > 3000 && current_angle != 1750) {
      SerialJSON("Close Windows");
      if (motorAllowed) {
        for (current_angle = 1200; current_angle < 1750; current_angle += 1) {
          myservo.write(current_angle);
          delay(5);
        }
        //current_angle = 0;
        last_used = millis();
      }
    }
  }

  // START NEW CODE
  if(lightAllowed) {
    float illuminationAverage;
    {  // new scope for variables to get discarded faster
      int tempSum = 0;
      // Sum up all the values in the array
      for (int i = 0; i < TotalLightSensors; i++) {
        tempSum += LightSensorState[i];
      }
      // STATIC cast convierte como (float)var
      illuminationAverage = static_cast<float>(tempSum) / TotalLightSensors;
    }
    // ( millis() - lastMotion < 5000 && illuminationAverage < 0.5 ) // motion in the last 5 seconds and no existing light
    for (int i = 0; i < TotalLights; i++) {
      digitalWrite(LightPin[i], ( millis() - lastMotion < 5000 && illuminationAverage < 0.5 ) ? HIGH : LOW);
    }
  }
  // auto change lights based on light sensor
  // END NEW CODE
  delay(interval * 1000);
  /*
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
   */
  if (Serial.available()) {
    SerialChar = Serial.read();
    // Serial.println("--------------------");
    // Serial.println(SerialChar);
    switch (SerialChar) {
      case 'h':
        Serial.println("g = get\na = analog\np = PWM\nu = UART\ns = set\na = analog\np = PWM\nu = UART\nm = motor (automatic Window opening)\n   1 = ON (use)\n   0 = OFF (do not use)\nm = set motor\n   1 = ON (use)\n   0 = OFF (do not use)\na = allowed sources\nt = threshold\n   [1-99] = number (suggested max 40-60%) // 50 is normal\n\nw = windows\n   [0-TotalWindows] = window id\n     returns 0 or 1\n\ne = errorcodes\n\n  ERROR CODES:\n  1 - Missing Info\n  2 - Wrong Info");
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

void SerialJSON(String message) {
  String concatV = toJSONbool(VentanaState, TotalVentanas);  //necessary outside the switch and in this function
  String concatL = toJSONbool(LightState, TotalLights);
  String concatLS = toJSON(LightSensorState, TotalLightSensors);
  String JSONdata = 
  (
    "{\"ppm_a\": " + String(ppm_analog) 
  + ",\"ppm_u\": " + String(ppm_uart) 
  + ",\"ppm_p\": " + String(ppm_PWM) 
  + ",\"windows\": " + concatV 
  + ",\"threshold\": " + threshold 
  + ",\"servoangle\": " + current_angle 
  + ",\"motorAllowed\": " + motorAllowed 
  + ",\"millis\": " + millis() 
  + ",\"lights\": " + concatL
  + ",\"lightSensors\": " + concatLS
  + ",\"lightAllowed\": " + lightAllowed 
  + ",\"lastMotionDelta\": " + (millis() - lastMotion) 
  + ",\"message\": \"" + message + "\"}"
  );
  Serial.println(JSONdata);

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
// gas reading

int gas_concentration_uart() {
  byte addArray[] = { 0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79 };
  char dataValue[9];
  SerialCom.write(addArray, 9);  // mueve el servo?!?
  SerialCom.readBytes(dataValue, 9);
  int resHigh = (int)dataValue[2];
  int resLow = (int)dataValue[3];
  int ppm_uart = (resHigh * 256) + resLow;

  return ppm_uart;
}

int get_analog() {
  float v = analogRead(analogPin) * 5.0 / 1023.0;
  int gas_concentration = int((v) * (5000 / 2));

  return gas_concentration;
}

int gas_concentration_PWM() {
  int imbored = 0;
  while (digitalRead(pwmPin) == LOW) {
    //    imbored++;
    //    delay(1);
    //    if(imbored > 5000){
    //      return 0;
    //    }
  }
  long t0 = millis();
  while (digitalRead(pwmPin) == HIGH) {
    //    imbored++;
    //    delay(1);
    //    if(imbored > 5000){
    //      return 0;
    //    }
  }
  long t1 = millis();
  while (digitalRead(pwmPin) == LOW) {
    //    imbored++;
    //    delay(1);
    //    if(imbored > 5000){
    //      return 0;
    //    }
  }
  long t2 = millis();
  long tH = t1 - t0;
  long tL = t2 - t1;
  long ppm = 5000L * (tH - 2) / (tH + tL - 4);
  while (digitalRead(pwmPin) == HIGH) {
    //    imbored++;
    //    delay(1);
    //    if(imbored > 5000){
    //      return 0;
    //    }
  }
  delay(10);

  return int(ppm);
}

// START NEW CODE
String toJSON(int arr[], int arrSize) {
  String JSONresult = "[";
  for (int i = 0; i < arrSize-1; i++) {
    JSONresult += String(arr[i]) + ",";
  }
  JSONresult += String(arr[arrSize - 1]);
  JSONresult += "]";
  return JSONresult;
}

String toJSONbool(bool arr[], int arrSize) {
  String JSONresult = "[";
  for (int i = 0; i < arrSize-1; i++) {
    JSONresult += String(arr[i]) + ",";
  }
  JSONresult += String(arr[arrSize - 1]);
  JSONresult += "]";
  return JSONresult;
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
