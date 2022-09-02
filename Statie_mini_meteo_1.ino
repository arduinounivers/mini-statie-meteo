#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "Wire.h"
#include "Adafruit_BMP280.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Adafruit_Si7021.h"
#include <Adafruit_AHT10.h>
//#include <MQTT.h>

Adafruit_Si7021 sensor = Adafruit_Si7021();
Adafruit_AHT10 aht;

#define ssid      "atlas"    // WiFi SSID
#define password  "tully123" // WiFi password
#define LEDPIN    D8         // Relay
#define SWITCH    D7         // Switch_1
#define SWITCH_    D5        // Switch_2
String  etatLed = "OFF";

#define OLED_RESET D6  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);
ESP8266WebServer server ( 80 );
 
Adafruit_BMP280 bmp;

float t = 0; 
float alt = 0;
float p = 0;
float p_ = 0;
float h = 0;
float t_ = 0; 

String getPage(){
  String page = "<html lang=fr-FR><head><meta http-equiv='refresh' content='10'/>";
//  page += "<title>STATIE METEO CORBU - corbu.atlas.com.ro</title>";
  page += "<title>WEATHER STATION - corbu.atlas.com.ro</title>";
  page += "<style> body { background-color: #fffff; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }</style>";
  page += "</head><body><h1>-- WEATHER STATION --</h1>";
//  page += "<h5><ul><li>Latitudine:44,38108 / Longitudine:28,65608</h5>";
//  page += "<h3>Senzor-1</h3>";
  page += "<h3><ul><li>Temperature_ : ";
  page += t;
//  page += " ~ ";
  //page += t_;
//  page += "°C</li>";
  page += " degrees C</li>";
  page += "<li>Hmidity_____       : ";
  page += h;
// page += "RH%</li></ul><h3>Senzor-2</h3>";
  page += " RH%";
  page += "<li>Pressure____ : ";
  page += p;
  page += " mmHg / ";
  page += p_;
  page += " hPa</li>";
  page += "<li>Altitude_____ : ";
  page += alt;
  page += " m</li></ul>";
  page += "<h3>REMOTE CONTROL</h3>";
  page += "<form action='/' method='POST'>";
  page += "<h3><ul><li>D7 (status: ";
  page += etatLed;
  page += ")";
  page += "<INPUT type='radio' name='LED' value='1'>ON";
  page += "<INPUT type='radio' name='LED' value='0'>OFF</li></ul>";
  page += "<h2><INPUT type='submit' value='UPDATED'<h3>";
//  page += "<ul><li> ";
  page += "<ul><li>Connected to: ";
  page += ssid;
  page += "<h4><li>MQTT: ko";
//  page += "<br><br><p><a hrf='https://atlas.com.ro'>corbu.atlas.com.ro</p>";
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
  Serial.println("Set GPIO ");
  Serial.print(LEDValue);
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
  if (! aht.begin()) {
    Serial.println("Could not find AHT10? Check wiring");
    while (1) delay(10);
  }  
  //digitalWrite(SWITCH_, HIGH);
  Serial.begin(115200);  //Port série
  pinMode (D8, OUTPUT);
  pinMode (D7, INPUT);
  pinMode (D5, INPUT);
  if (!bmp.begin(BMP280_ADDRESS_ALT, BMP280_CHIPID)) {
  //if (!bmp.begin()) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                      "try a different address!"));
    while (1) delay(10);
  }
  //bmp.begin();  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 64x48)
  display.display();

   WiFi.begin ( ssid, password );
  // Attente de la connexion au réseau WiFi / Wait for connection
  ///while ( WiFi.status() != WL_CONNECTED ) {
    ///delay ( 500 ); 
    ///Serial.print ( "." );
  ///}
  // Connexion WiFi établie / WiFi connexion is OK
  Serial.println ( "" ); 
  Serial.print ( "Connected to " );
  Serial.println ( ssid );
  Serial.print ( "IP address: " );
  Serial.println ( WiFi.localIP() );
  // On branche la fonction qui gère la premiere page / link to the function that manage launch page 
  server.on ( "/", handleRoot );

  server.begin();
  Serial.println ( "HTTP server started" );
}
 
void loop() {
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
  Serial.print("Temperature: "); Serial.print(temp.temperature); Serial.println(" degrees C");
  Serial.print("Humidity: "); Serial.print(humidity.relative_humidity); Serial.println("% rH");
  h=humidity.relative_humidity;
    server.handleClient();
    Serial.print("Temperature = ");   //En degrés Celsius (SI)
    Serial.print(bmp.readTemperature());
    Serial.print(" *C");
    t = (bmp.readTemperature());
    Serial.print("   Pression = ");  //En Pascal (SI)
    //Serial.print(bmp.readPressure());
    // Serial.print(" Pa ");
    
    //Pour convertir vers d'autres unités de pression voir
    //http://www.engineeringtoolbox.com/pressure-units-converter-d_569.html
    // 1 Pa = 10-5 bar = 0.0075 mm Hg = 9.87.10-6 atm
    float PressionPascal;
    float PressionAtm;
    float PressionHg;
    PressionPascal = bmp.readPressure();  //Lire le capteur
    PressionAtm = PressionPascal*9.81*0.000001;
    PressionHg = PressionPascal*0.0075;
    
    Serial.print(PressionPascal);
    Serial.print(" Pa     ");
    Serial.print(PressionAtm);
    Serial.print(" Atm     ");
    Serial.print(PressionHg);
    Serial.print(" mm Hg     ");
    p = PressionHg;     
    p_ = PressionPascal/100;
    
    Serial.print("  Altitude = ");  
    //On présume avoir 1013,25 hPa au niveau de la mer
    Serial.print(bmp.readAltitude(101600));
    Serial.println(" metres");
    alt = (bmp.readAltitude(1020));

    Serial.print("Humidity:    "); 
    Serial.print(sensor.readHumidity(), 2);
    h = sensor.readHumidity();
    Serial.print("\tTemperature: "); 
    Serial.println(sensor.readTemperature(), 2);
    t_= sensor.readTemperature();


  // Efface l'écran et positionne le curseur dans le coin supérieur gauche - clear display and set cursor on the top left corner
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  
if (digitalRead(SWITCH) == HIGH) {
    display.println("-- WEATHER STATION --");
    display.print("SSID: ");
    display.println(ssid); 
    display.print("IP: ");
    display.println(WiFi.localIP());
    display.println("      ----------");
    display.display();
  }  

  else {
    display.print("Temperature: ");
 //   display.print(t,2);
//    display.print("-");
    display.print(t_,2);
    display.println(" C");
    display.print("Humidity: ");
    display.print(h,2);
    display.println(" RH%");
    display.print("Pressure: ");
  if (digitalRead(SWITCH_) == HIGH) {  
    display.print(p);
    display.println(" mmHg");
  }
  else {
    display.print(p_);
    display.println(" hPa");
  } 
    display.print("Altitude: ");
    display.print(alt);
    display.println(" m");
    display.display();
  }

  
  delay(5000); 
}
