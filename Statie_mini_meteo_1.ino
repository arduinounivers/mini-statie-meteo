#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "DHT.h"
#include <Adafruit_BMP085.h>

#define ssid      "atlas"       // WiFi SSID
#define password  "tully123"  // WiFi password
#define DHTTYPE   DHT22       // DHT type (DHT11, DHT22)
#define DHTPIN    D4          // Broche du DHT / DHT Pin
#define LEDPIN    D3          // Led
float   t = 0 ;
float   h = 0 ;
float   p = 0;
float   a = 0;
float   b = 0;
String  etatLed = "OFF";
// Création des objets / create Objects
DHT dht(DHTPIN, DHTTYPE);
Adafruit_BMP085 bmp;
ESP8266WebServer server ( 80 );

String getPage(){
  String page = "<html lang=fr-FR><head><meta http-equiv='refresh' content='10'/>";
  page += "<title>STATIE METEO CORBU - corbu.atlas.com.ro</title>";
  page += "<style> body { background-color: #fffff; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }</style>";
  page += "</head><body><h1>STATIE METEO CORBU</h1>";
  page += "<h5><ul><li>Latitudine:44,38108/Longitudine:28,65608</h5>";
  page += "<h3>Senzor-1</h3>";
  page += "<h3><ul><li>Temperatura_ : ";
  page += a;
//  page += "°C</li>";
  page += "gradeC</li>";
  page += "<li>Umiditate___       : ";
  page += b;
  page += "%</li></ul><h3>Senzor-2</h3>";
  page += "<h3><ul><li>Presiune atmosferica_ : ";
  page += p;
  page += " mbar</li></ul>";
  page += "<h3>RELEU</h3>";
  page += "<form action='/' method='POST'>";
  page += "<h3><ul><li>D3 (stare: ";
  page += etatLed;
  page += ")";
  page += "<INPUT type='radio' name='LED' value='1'>ON";
  page += "<INPUT type='radio' name='LED' value='0'>OFF</li></ul>";
  page += "<h3><INPUT type='submit' value='Actualizeaza'<h3>";
  page += "<ul><li>Conectat la: ";
  page += ssid;
  page += "<li>Adresa IP:  ";
  page += "192.168.100.99";
  page += "<br><br><p><a hrf='https://atlas.com.ro'>corbu.atlas.com.ro</p>";
  page += "</body></html>";
  return page;
}
void handleRoot(){ 
  if ( server.hasArg("LED") ) {
    handleSubmit();
  } else {
    server.send ( 200, "text/html", getPage() );
  }  
}

void handleSubmit() {
  // Actualise le GPIO / Update GPIO 
  String LEDValue;
  LEDValue = server.arg("LED");
  Serial.println("Set GPIO "); Serial.print(LEDValue);
  if ( LEDValue == "1" ) {
    digitalWrite(LEDPIN, 1);
    etatLed = "On";
    server.send ( 200, "text/html", getPage() );
  } else
    if ( LEDValue == "0" ) {
    digitalWrite(LEDPIN, 0);
    etatLed = "Off";
    server.send ( 200, "text/html", getPage() );
  } else {
    Serial.println("Err Led Value");
  }
}

void setup() {
  pinMode (LEDPIN , OUTPUT);
  //pinMode (DHTPIN , INPUT);
  Serial.begin ( 115200 );
  // Initialisation du BMP180 / Init BMP180
  if ( !bmp.begin() ) {
    Serial.println("BMP180 KO!");
    while(1);
  } else {
    Serial.println("BMP180 OK");
  }
  
  WiFi.begin ( ssid, password );
  // Attente de la connexion au réseau WiFi / Wait for connection
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 ); Serial.print ( "." );
  }
  // Connexion WiFi établie / WiFi connexion is OK
  Serial.println ( "" ); 
  Serial.print ( "Connected to " ); Serial.println ( ssid );
  Serial.print ( "IP address: " ); Serial.println ( WiFi.localIP() );
  // On branche la fonction qui gère la premiere page / link to the function that manage launch page 
  server.on ( "/", handleRoot );

  server.begin();
  Serial.println ( "HTTP server started" );
}

void loop() {
  delay(2500);
  //delay(100);//delay(dht.getMinimumSamplingPeriod())
  server.handleClient();
  t = dht.readTemperature();
  h = dht.readHumidity();
  p = bmp.readPressure() / 100.0F;
  if (isnan(h) || isnan(t)) {
    //Serial.println("Failed to read from DHT sensor!");
    return;
  }
  a=t;
  b=h;  
  Serial.print((float)t); Serial.print(" *C, ");
  Serial.print((float)h); Serial.println(" RH%");
  Serial.println (etatLed);
  delay ( 800 );
}
