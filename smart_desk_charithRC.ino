#define BLYNK_PRINT Serial
#define DHTPIN D1
#define DHTTYPE DHT11

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "ThingSpeak.h"

#include "DHT.h"

BlynkTimer timer;
WiFiClient  client; 
DHT dht(DHTPIN, DHTTYPE);

char auth[] = "5rNyW54y0w_tuMxVt7yFtNTf_044sy86";

// WiFi credentials.
char ssid[] = "virus";
char pass[] = "adamapple";
unsigned long myChannelNumber =1125518;
const char * myWriteAPIKey ="AZ4UGY9484VQ9419";
float temp = 0;
float humi;
int ir = D2;
int bulb = D4;
int irval = 1;
int fan = D5;
int pinval;
int bulb_val_blynk = 1;
int fan_val_blynk;
WidgetLED led1(V2);

void setup()  
{
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  ThingSpeak.begin(client);
  dht.begin(); 
  pinMode(ir,INPUT);
  pinMode(bulb,OUTPUT);
  pinMode(fan,OUTPUT);
  
  timer.setInterval(15000,update_thingsSpeak);
  
}

void read_dht(){
  humi = dht.readHumidity();
  temp = dht.readTemperature();
  Blynk.virtualWrite(V0,temp);
  Blynk.virtualWrite(V1,humi);
  Serial.print("humidity = ");
  Serial.println(humi);
  Serial.print("temperature = ");
  Serial.println(temp);
  delay(1000);

  }

void read_ir(){
  irval = digitalRead(ir);
  pinval = digitalRead(D4);  
  BLYNK_WRITE(V3);
  if(irval == 0 || bulb_val_blynk == 0){
    digitalWrite(bulb,HIGH);
    led1.on();
    }
    else{
    digitalWrite(bulb,LOW);
    led1.off();
    }
  if(pinval == 1){led1.on();}else{led1.off();}        //widget led will turn on ,even if bulb is switched on via IFTTT
 }

void update_thingsSpeak(){
  Serial.println(String(temp)+" " +String(humi)+" " +String(irval));
  ThingSpeak.setField(1,temp );
  ThingSpeak.setField(2,humi);
  ThingSpeak.setField(3,irval);
  ThingSpeak.setField(4,fan_val_blynk);
  
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  }
void read_fan_relay(){
  BLYNK_WRITE(V4);
    
  if(fan_val_blynk == 0  || temp>=30){
      digitalWrite(fan, HIGH);
    }
    else{
      digitalWrite(fan, LOW);
    }
  }

BLYNK_WRITE(V4)
      {
         fan_val_blynk = param.asInt(); // Get value as integer
      }
      
BLYNK_WRITE(V3)
      {
         bulb_val_blynk = param.asInt(); // Get value as integer
      }
void loop()
{ 
  Blynk.run();
  timer.run();
  read_dht();
  read_ir();
  read_fan_relay();
}
