
/*

  Este programa escribe en una bbase de datos myqsl a través de una solicitud http / por php

  Realizado por huecat.es

  HUECAT INGENIERIA ESPAÑA
  WHATSAPP (+34) 911 080 949

*/

#ifdef ESP32
#include <WiFi.h>
#include <HTTPClient.h>
#else
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#endif

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

////////////////////////////////////////////////// Definición de pines
const int srEcho = D3;
const int srTrigger = D5;
const int dis1Sensor = D6;
const int LedRele = D7; //Relay


////////////////////////////////////////////////// WiFi
bool resetWiFiCredential = false;
const char* serverName = "http://supermega.tech/iot/http/index.php";
String apiKeyValue = "74r43rjdewowri3rjrfks987hdjfup8";
int chipId;

// Deposito // HC-SRC04
int alturaRecipiente = 16;
int cm = 0;
int contenido = 0;
int contenidoant = 0;
int valAnt = 0;


void setup() {

  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode (LedRele, OUTPUT); // para cambiar por rele
  digitalWrite (LedRele, HIGH); //para cambiar por rele

  startLCD();

  chipId = system_get_chip_id();

  Serial.print ("chipId");  Serial.println (chipId);

  chipId = chipId;

  startLCD();

  Serial.print ("\n starting \n CHIP ID: "); Serial.println(chipId);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  //reset saved settings
  if (resetWiFiCredential) wifiManager.resetSettings();


  //wifiManager.autoConnect("PRY001-ID:"+chipId);

  char *wifiName = "PRY001 ";

  //  char *wifiName2= chipId;

  //char * strcat(char * wifiName, char * chipId)

  //wifiManager.autoConnect();
  wifiManager.autoConnect(wifiName);



  //if you get here you have connected to the WiFi
  Serial.println("Conectado a Wi-Fi)");

  delay (1000);

  updateLevel();
  updateServerState("Restarted");

}

unsigned long currentMillis, lastMillis;

void loop() {

  currentMillis = millis();

  if (currentMillis > lastMillis + 60000)
  {
    updateLevel();
    updateServerState("ok");
    lastMillis = currentMillis;
  }

}

int horasUso = 648;
int liquidLevel = 18;


void updateServerState(String sendMessage)
{
  unsigned long millisStart = millis();

  Serial.print("updateServerState -> ");

  if (WiFi.status() == WL_CONNECTED) {

    HTTPClient http;

    http.begin(serverName);

    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String httpRequestData = "api_key=" + apiKeyValue + "&id_chip=" + chipId + "&nivel=" + liquidLevel + "&mensaje=" + sendMessage + "&horas_uso=" + horasUso;

    // Ejemplo de como quedaría
    // http://localhost/iot/http/?api_key=74r43rjdewowri3rjrfks987hdjfup8&id_chip=9087369&nivel=10&mensaje=ok&horas_uso=443





    Serial.print("httpRequestData ->  "); Serial.println(httpRequestData);

    int httpResponseCode = http.POST(httpRequestData);

    // Request with a content type: text/plain
    //  http.addHeader("Content-Type", "text/plain");
    //  int httpResponseCode = http.POST("Hello World");

    // If you need an HTTP request with a content type: application/json, use the following:
    //http.addHeader("Content-Type", "application/json");
    //int httpResponseCode = http.POST("{\"value1\":\"19\",\"value2\":\"67\",\"value3\":\"78\"}");

    if (httpResponseCode > 0) {
      Serial.print("Response: ");  Serial.print(httpResponseCode);
    }
    else {
      Serial.print("Error: "); Serial.print(httpResponseCode);
    }

    http.end(); //Libera recursos
  }
  else {
    Serial.println("WiFi desconectado");
  }

  unsigned long elapsed = millis() - millisStart;

  Serial.print (" time: "); Serial.print(elapsed); Serial.println ("ms");

}

void startLCD()
{
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("PRY001");
  lcd.setCursor(0, 1);
  lcd.print("Comenzando... ");
}

void updateLevel()
{
  // HC-SR04
  cm = 0.01723 * leerDistanciaUltrasonica(srTrigger, srEcho); //mide distancia en cm del ping

  if (valAnt != cm && cm <= alturaRecipiente) {
    valAnt = cm;
    contenido = 100 - ((cm * 100) / alturaRecipiente);
    Serial.print(cm);
    Serial.print(" cm, ");
    Serial.print(contenido);
    Serial.print(" %");
    Serial.println();
    if (contenido != contenidoant) {
      presentar(contenido);
      contenidoant = contenido;
    }
  }
}


long leerDistanciaUltrasonica (int triggerPin, int echoPin) {
  pinMode(triggerPin, OUTPUT);    // limpiamos el trigger
  digitalWrite(triggerPin, LOW);  // para genera un pulso limpio lo ponermos a LOW
  delayMicroseconds(2);       // 2 microsegundos
  digitalWrite(triggerPin, HIGH); // Ponemos el trigger en HIGH durante 10 microsegundos
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);      // lee el pin de Echo y devuelve duracion del trayecto
  return pulseIn(echoPin, HIGH);  // medimos el tiempo entre pulsos
}


void presentar(int capacidad) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print ("InfoLev");
  lcd.setCursor(9, 0);
  lcd.print (chipId);
  lcd.setCursor(0, 1);
  lcd.print ("Capacidad : ");
  lcd.print (capacidad);
  lcd.print (" %");
}
