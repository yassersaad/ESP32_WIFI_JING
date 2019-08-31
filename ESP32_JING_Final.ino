void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include "DHT.h"
#include "Wire.h"
#include <WiFi.h>
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
int ldrValue = analogRead(A0); //for Light Sensor     
unsigned long lastConnectionTime = 0;
const unsigned long postingInterval = 30000; 
const char* ssid     = "XXXXX"; //WIFI NAME
const char* password = "XXXXXXXXX"; //WIFI PASSWORD
const String APIKEY="XXXXXXX";          
const char* URL = "emoncms.org";
WiFiClient client;
const int PORT = 80;
 
void setup()
{
  Serial.begin(9600);       
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());  
  dht.begin();
  lcd.init();
  lcd.backlight();
}

void loop()
{   
    delay(100);
    do_weblog();
}
// =========================================================================
/*
This is where it all happens.
If the posting interval is reached then a new POST is done with the latest data
If not time yet, we simply exit
 */
void do_weblog() {
    makeMeasurement();    
    if(!client.connected() && (millis() - lastConnectionTime > postingInterval)) {
        Serial.println("Transmitting.");
        sendData();
    }
}

void makeMeasurement()
{
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
  lcd.print(ldrValue);
  if (ldrValue>2500) {
   lcd.setCursor(0, 1);
   lcd.print("Light Off");
}
  else {
   lcd.setCursor(0, 1);
   lcd.print("Light On ");
}
delay(2000);
}

void sendData() {
    // Create the 'content' string to send. 
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
    if (client.connect(URL, PORT))
    {         
        client.print(str);  // send data to Emoncms
        delay(30000);        // wait for a response
        while (client.available()) {
          char c = client.read();
          Serial.print(c);
        }
    }

    else {
        // if you couldn't make a connection:
        Serial.println();
        Serial.println("Connection failed");
        Serial.println("disconnecting.");
        Serial.println();
        client.stop();
    }
    // note the time that the connection was made
    lastConnectionTime = millis();
}
