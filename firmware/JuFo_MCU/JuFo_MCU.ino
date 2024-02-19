// Upload: DIP 3,4 ON
// use TXD0
// Usage with ESP: DIP 1,2,3,4 ON
// use TXD3

/* WIFI shouldnt be used here, this is the mega
#include <ESP8266WiFi.h> 
const char* ssid = "";
const char* pass = "";
const char* host = "";
const uint16_t port = 80;
*/
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
bool AllowedSource[3] = { false, false, false };  // analog, PWM, Uart TODO: Change this when connecting everything

// START NEW CODE
//freepins 8  11
//usedpins 0 1 2 3 4 5 6 9 10 12 13 A0
const int TotalLights = 4;  // ejemplo con ia de que si aprende que una nunca se enciende da igual la situación (tmb como input se puede usar "otras leds encendidas")
int LightPin[TotalLights] = { 22, 24, 26, 28 };
bool LightState[TotalLights] = { false, false, false, false };
int lightThreshold = 150;
const int TotalLightSensors = 2;  // ejemplo con ia de que si aprende que una nunc se enciende )(tmb como input se puede usar "otras leds encendidas")
int LightSensorPin[TotalLightSensors] = { A1, A2 };
int LightSensorState[TotalLightSensors] = {}; // will have to multiply by 1000 to be albe to precise


bool lightAllowed = true;  // variable to read the value from the analog pin

int motionSensorPin = 8;
int motionSensorState = LOW;
long int lastMotion = millis();
void processCommand(char SerialChar, Stream *selectedSerial);
void setup() {
  // Wifi Connectivity
  Serial3.begin(115200);


  Serial2.begin(9600);  // esta cambiado !!!
  Serial2.println("Machine Initialised");
  /* WIFI WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  delay(500);
  Serial2.print("WiFi is ");
  Serial2.println(WiFi.status() == WL_CONNECTED); */
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
  // Serial2.begin(9600);
  pinMode(pwmPin, INPUT_PULLUP);

  // START NEW CODE
  for (int i = 0; i < TotalLights; i++) {
    pinMode(LightPin[i], OUTPUT);
    digitalWrite(LightPin[i], HIGH);
    LightState[i] = true;
  }
  for (int i = 0; i < TotalLightSensors; i++) {
    pinMode(LightSensorPin[i], INPUT);
  }
  // TEST IF WORKS
  delay(5000);
  for (int i = 0; i < TotalLights; i++) {
    digitalWrite(LightPin[i], LOW);
    LightState[i] = false;
  }
  delay(1000);
  digitalWrite(LightPin[0], analogRead(LightSensorPin[0]) > 500 ? LOW : HIGH);
  LightState[0] = (analogRead(LightSensorPin[0]) > 500) ;
  delay(1000);
  digitalWrite(LightPin[0], analogRead(LightSensorPin[0]) > 500 ? LOW : HIGH);
  LightState[0] = (analogRead(LightSensorPin[0]) > 500 );

  pinMode(motionSensorPin, INPUT);
  

  //delay(180000); // preheat the CO2 sensor for 3 minutes
  delay(1000);
}

// Verificacion de la conectividad entre la Mega y ESP8266
void checkWiFiSerial() {
  if (Serial3.available()) {
    while (Serial3.available()) {
      // Lectura de datos del Serial3
      char inChar = Serial3.read();
       SerialJSON("ESPdataredcieved", &Serial2);
      processCommand(inChar, &Serial3);
      /*
      inString += inChar;
      if (inChar == ']') {
        if (inString.indexOf("[ON]")>0) {
          digitalWrite(PIN_LED, HIGH);
        }
        else if (inString.indexOf("[OFF]")>0) {
          digitalWrite(PIN_LED, LOW);
        }
        else
        {
          Serial2.println("Wrong command");
        }
        inString = "";
      }
      */
    }
  }
}

int ppm_uart = 0;
int ppm_PWM = 0;
int ppm_analog = 0;

void loop() {
  // Serial2.println("true1");

  for (int i = 0; i < TotalVentanas; i++) {
    VentanaState[i] = digitalRead(VentanaPin[i]);
  }

  // START NEW CODE
  for (int i = 0; i < TotalLightSensors; i++) {
    LightSensorState[i] = analogRead(LightSensorPin[i]);
  }
  motionSensorState = digitalRead(motionSensorPin);
  if(motionSensorState && millis() - lastMotion > 20000 /*&& (!digitalRead(bluetoothStatePin))*/) {
    // saliste de casa y llevas 20 segundos fuera sin movimiento y no hay nadie cerca(conectado)
    // TODO connect HC05 state pin
    
    // alertNewPerson(); // envia push a dispositivos y ya verán si hay alguien en casa
  }
  lastMotion = motionSensorState ? millis() : lastMotion;

  // END NEW CODE
  //  Serial2.print(VentanaState[0]);
  //  Serial2.println("<-- Ventana 1");
  //  Serial2.print(VentanaState[1]);
  //  Serial2.println("<-- Ventana 2");
  //  Serial2.print(VentanaState[2]);
  //  Serial2.println("<-- Ventana 3");
  //

  if (AllowedSource[2]) {
    //  Serial2.println("reading uart...");
    ppm_uart = gas_concentration_uart();
  } else {
    ppm_uart = 0;
  }

  //  Serial2.println("reading pwm..."); //not working
  if (AllowedSource[1]) {
    ppm_PWM = gas_concentration_PWM();
  } else {
    ppm_PWM = 0;
  }

  if (AllowedSource[0]) {
    //  Serial2.println("reading analog..."); //not accurate
    ppm_analog = get_analog();
  } else {
    ppm_analog = 0;
  }

  /* Print Serial Info
  //  Serial2.print(ppm_uart);
  //  Serial2.println(" <-- UART");
  //  Serial2.print(ppm_PWM);
  //  Serial2.println(" <-- PWM");
  //  Serial2.print(ppm_analog);
  //  Serial2.println(" <-- Analog");
  //  Serial2.println("--------------------");*/
  int number_of_closed = 0;
  for (int i = 0; i < TotalVentanas; i++) {
    if (VentanaState[i] == 0) {
      number_of_closed++;
    }
  }

  // automatic opening
  // Serial2.println(TotalVentanas / 2);
  int main_ppm = (ppm_PWM != 0) ? ppm_PWM : ppm_uart;

  if (main_ppm > /*ppmhora[hora]*/ 1000 * threshold / 100) {  // cambiar entre (desde app) 80% y 120% (40-60*2)
    //Serial2.println(current_angle);
    if (number_of_closed >= TotalVentanas / 2 && millis() - last_used > 3000 && current_angle != 1200) {
      SerialJSON("Open Windows", &Serial2);
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
      SerialJSON("Close Windows", &Serial2);
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
      // TODO change LightSensorState[0] with illuminationAverage
      if ( millis() - lastMotion < 5000 && LightSensorState[0] < lightThreshold ) {
        digitalWrite(LightPin[i], HIGH);
        LightState[i] = true;
      } else {
        digitalWrite(LightPin[i], LOW);
        LightState[i] = false;
      }
      
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
   *  t = threshold 
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
  if (Serial2.available()) {
    SerialChar = Serial2.read();
    processCommand(SerialChar, &Serial2);
    // Serial2.println("--------------------");
    // Serial2.println(SerialChar);
    
    // Serial2.println("--------------------");
  }
  checkWiFiSerial();
}
void processCommand(char SerialChar, Stream *selectedSerial) {
  switch (SerialChar) {
      case 'h':
        selectedSerial->println("g = get\na = analog\np = PWM\nu = UART\ns = set\na = analog\np = PWM\nu = UART\nm = motor (automatic Window opening)\n   1 = ON (use)\n   0 = OFF (do not use)\nm = set motor\n   1 = ON (use)\n   0 = OFF (do not use)\na = allowed sources\nt = threshold\n   [1-99] = number (suggested max 40-60%) // 50 is normal\n\nw = windows\n   [0-TotalWindows] = window id\n     returns 0 or 1\n\ne = errorcodes\n\n  ERROR CODES:\n  1 - Missing Info\n  2 - Wrong Info");
        break;
      case 'g':  // get
        if (selectedSerial->available()) {
          SerialChar = selectedSerial->read();
          // selectedSerial->println(SerialChar);
          if (SerialChar == 'a') {
            selectedSerial->println(ppm_analog);
          } else if (SerialChar == 'p') {
            selectedSerial->println(ppm_PWM);
          } else if (SerialChar == 'u') {
            selectedSerial->println(ppm_uart);
          } else if (SerialChar == 'l') {
            selectedSerial->println("{ " + toJSON(LightSensorState, TotalLightSensors) + " }");
          } else if (SerialChar == 'm') {
            selectedSerial->println(millis() - lastMotion);
          } else {
            selectedSerial->println("ERROR (Code: 2)");
          }
        } else {
          selectedSerial->println("ERROR (Code: 1)");
        }
        break;
      case 's':  // set
        if (selectedSerial->available()) {
          SerialChar = selectedSerial->read();
          //selectedSerial->println(SerialChar);
          if (selectedSerial->available()) {
            ControlInt = selectedSerial->read() - 48;
            ControlInt = (bool)ControlInt;
            // selectedSerial->println(ControlInt);
          } else {
            selectedSerial->println("ERROR (Code: 1)");
          }
          if (SerialChar == 'a') {
            AllowedSource[0] = ControlInt;
          } else if (SerialChar == 'p') {
            AllowedSource[1] = ControlInt;
          } else if (SerialChar == 'u') {
            AllowedSource[2] = ControlInt;
          } else if (SerialChar == 'm') {
            motorAllowed = ControlInt;
          } else if (SerialChar == 'l') {
            lightAllowed = ControlInt;
          } else {
            selectedSerial->println("ERROR (Code: 2)");
            break;
          }
          SerialJSON("OK", &Serial2);
        } else {
          selectedSerial->println("ERROR (Code: 1)");
        }
        break;
      case 'm':  // use servo/motor 1 open:
                 //      if (selectedSerial->available()) {
                 //           ControlInt = selectedSerial->read() - 48;
                 //           ControlInt = (bool)ControlInt;
                 //           selectedSerial->println(ControlInt);
                 //           motorAllowed = ControlInt;
                 //           selectedSerial->println("OK");
                 //         } else {
                 //           selectedSerial->println("ERROR (Code: 1)");
                 //         }
        if (selectedSerial->available()) {
          ControlInt = selectedSerial->read() - 48;
          ControlInt = (bool)ControlInt;
          // selectedSerial->println(ControlInt);
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
          SerialJSON("OK", &Serial2);
        } else {
          selectedSerial->println("ERROR (Code: 1)");
        }
        break;
      case 'l':
        if (selectedSerial->available()) {
          char tempInput = selectedSerial->read();
          if(tempInput = 't') { // threshold 
            if (selectedSerial->available()) {
              lightThreshold = (selectedSerial->read() - 48) * 10;
            } else {
              selectedSerial->println("ERROR (Code: 1)");
            }
            break;
          }
          int ChosenLight = (int)(tempInput - 48);
          if (selectedSerial->available()) {
            ControlInt = selectedSerial->read() - 48;
            ControlInt = (bool)ControlInt;
            // selectedSerial->println(ControlInt);
            if(ChosenLight < TotalLights) {
              digitalWrite( LightPin[ChosenLight], (ControlInt ? HIGH : LOW)); 
              LightState[ChosenLight] = ControlInt;
            } else {
              // selectedSerial->print(ChosenLight);
              selectedSerial->println("ERROR (Code: 2)");
            }
            // SerialJSON("OK");
          } else {
            selectedSerial->println("ERROR (Code: 1)");
          }
        } else {
          selectedSerial->println("ERROR (Code: 1)");
        }
        break;
      case 'a':
        selectedSerial->print("Analog ");
        selectedSerial->println(AllowedSource[0]);
        selectedSerial->print("PWM ");
        selectedSerial->println(AllowedSource[1]);
        selectedSerial->print("UART ");
        selectedSerial->println(AllowedSource[2]);
        break;
      case 't':  // threshold between 40-60% (d = default)
        if (!selectedSerial->available()) {
          selectedSerial->println("ERROR (Code: 1)");
        } else {
          SerialChar = selectedSerial->read();
          if (SerialChar == 'd') {
            threshold = 100;  // in %
          } else {
            int n1 = (int)(SerialChar - 48);
            int n2;
            if (selectedSerial->available()) {
              SerialChar = selectedSerial->read();  //warning NL and CR (ASCII 10,13)
              n2 = (int)(SerialChar - 48);
            } else {
              selectedSerial->println("ERROR (Code: 1)");
              break;
            }

            threshold = (n1 * 10 + n2) * 2;  // los numeros son * 2 pq 50 son 100
          }
        }
        //selectedSerial->println(threshold);
        break;
      case 'w':
        if (selectedSerial->available())
          ControlInt = (int)selectedSerial->read();
        else {
          selectedSerial->println("ERROR (Code: 1)");
          break;
        }
        if (ControlInt <= TotalVentanas)
          selectedSerial->println(VentanaState[ControlInt]);
        else
          selectedSerial->println("ERROR (Code: 2)");
        break;
      case 'e':
        selectedSerial->println("1 - Missing Info\n2 - Wrong Info");
        break;
      case 'j':
        SerialJSON("", selectedSerial);
        break;
      default:  // anything else
        // selectedSerial->println("ERROR WTF");
        break;
    }
}
void SerialJSON(String message, Stream *selectedSerial) {
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
  + ",\"lightThreshold\": " + lightThreshold 
  + ",\"message\": \"" + message + "\"}"
  );
  selectedSerial->print(JSONdata);

  // Send and store data
  /* connect to esp thourgh serial interface and tell it to do smth like
  HTTPClient http;
  
  http.begin("VictorMBP.local");
  http.addHeader("Content-Type", "application/json");
  
  int httpResponseCode = http.POST(JSONdata);
  
  if (httpResponseCode > 0) {
    //Serial.print("Data message sent. Response code: ");
    // Serial2.println(httpResponseCode);
  } else {
    Serial2.print("Error sending data message. Error code: ");
    Serial2.println(httpResponseCode);
  }
  
  http.end();
  */
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
  /* use serial to connet to esp to tell it to 
  HTTPClient http;
  
  http.begin(webhookURL);
  http.addHeader("Content-Type", "application/json");

  String payload = "{\"username\": \"test\", \"content\": \"hello\"}";
  
  int httpResponseCode = http.POST(payload);
  
  if (httpResponseCode > 0) {
    Serial2.print("Webhook message sent. Response code: ");
    Serial2.println(httpResponseCode);
  } else {
    Serial2.print("Error sending webhook message. Error code: ");
    Serial2.println(httpResponseCode);
  }
  
  http.end();
  */
}

// END NEW CODE


/* Quellen:
 *  
 *  https://diyi0t.com/mh-z14a-co2-meter-arduino-esp8266-esp32/
 *  
 *  
 */


// App open window on notification, threshold and everything, fixed PWM inteerface
