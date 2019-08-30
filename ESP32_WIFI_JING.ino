#include <WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include "Wire.h"
#include "DHT.h"
#define DHTPIN 33
#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT11     // DHT 11 
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27,16,2); // set the LCD address to 0x27 for a 16 chars and 2 line display
int h = dht.readHumidity(); //For DHT (No Decimal)
//float h = dht.readHumidity(); //For DHT (with Decimal)
//int t = dht.readTemperature(); //For DHT (No Decimal)
float t = dht.readTemperature(); //For DHT (with Decimal)
int ldrValue = analogRead(32); //for Light Sensor

const char* ssid     = "yy";
const char* password = "yyy";

const String APIKEY="2ad5a6a7efd868b5886d36f9f94b384b";          
const char URL[] ="emoncms.org";

WiFiServer server(80);

void setup()
{
  Serial.begin(115200);
  bool status;
  dht.begin();
  lcd.init();
  lcd.backlight();
  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();

}

int value = 0;

void loop() {

  h = dht.readHumidity();
  t = dht.readTemperature();
  ldrValue = analogRead(32);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("H:");
  lcd.print(h);
  lcd.print("%");
  lcd.setCursor(8, 0);
  lcd.print("T:");
  lcd.print(t);
  lcd.print("C");
  lcd.setCursor(11, 1);
//  lcd.print("Light:");
  lcd.print(ldrValue);
  if (ldrValue>2500) {
   lcd.setCursor(0, 1);
   lcd.print("Light Off"); }
  else {
   lcd.setCursor(0, 1);
   lcd.print("Light On "); }
   
  delay(3000);
}

  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {
    Serial.println("New Client.");           // print a message out the serial port
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {
        char c = client.read();             // read a byte, then
        Serial.write(c);
            String json;
            json =  "{t:";
            json +=  String(t);
            json += ",h:";
            json += String(h);
            json += ",l:";
            json += String(ldrValue);
            json +=  "}";
            String str;        
            str = "GET ";
            str += "/input/post.json?json=";
            str +=  json;
            str += "&apikey=";
            str +=  APIKEY;
            str += " HTTP/1.1\r\n";
            str += "Host: ";
            str += URL;
            str += "\r\n";
            str += "User-Agent: Arduino-wifi\r\n";
            str += "Connection: close\r\n";
            str += "\r\n";        
            Serial.println();              
            Serial.print(str);     
            Serial.println();
            Serial.println();
            Serial.println("connecting...");
            
            client.stop();
            Serial.println("Client Disconnected."); }
}
  }
