#include "ThingSpeak.h"
#include <SPI.h>
#include <WiFi.h>
#include "DHT.h"
#include"AirQuality.h"
#include"Arduino.h"

#define DHTPIN D5     // what pin we're connected to
#define DHTTYPE DHT11   // DHT 11 

AirQuality airqualitysensor;
int current_quality =-1;
char ssid[] = "ssid-name";    //  your network SSID (name) 
char pass[] = "password";   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;
DHT dht(DHTPIN, DHTTYPE);

unsigned long myChannelNumber = 000000; //  ThingSpeak channel number 
const char * myWriteAPIKey = "api-key";     // ThingSpeak api key 

// Initialize our values

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect.
  }

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }
    
  ThingSpeak.begin(client);  // Initialize ThingSpeak
  dht.begin();
  airqualitysensor.init(A0);
}

void loop() 
{

  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    
    while(WiFi.status() != WL_CONNECTED)
    {
      
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000); 
          
    } 

  }
    
    Serial.println("\nConnected.");
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    float a = airqualitysensor.first_vol;

    // set the fields with the values
    ThingSpeak.setField(1, h);
    ThingSpeak.setField(2, t);
    ThingSpeak.setField(3, a);



  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
      if(x == 200){
                    Serial.println("Channel update successful.");
                    Serial.println("Humidity: ");
                    Serial.println(h); 
                    Serial.println("Temperature: ");
                    Serial.println(t);
                    Serial.println("AQI: ");
                    Serial.println(a);
                    
                  }
      else{
            Serial.println("Problem updating channel. HTTP error code " + String(x));
          }
  
  delay(20000);
}

ISR(TIMER2_OVF_vect)
{
    if(airqualitysensor.counter==122)//set 2 seconds as a detected duty
    {
        airqualitysensor.last_vol=airqualitysensor.first_vol;
        airqualitysensor.first_vol=analogRead(A0);
        airqualitysensor.counter=0;
        airqualitysensor.timer_index=1;
        PORTB=PORTB^0x20;
    }
    else
    {
        airqualitysensor.counter++;
    }
}
