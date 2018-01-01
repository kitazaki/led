#include "Arduino.h"
#include "ESP8266.h"
#include "Milkcocoa.h"
#include "Client_ESP8266.h"

#define ESP_Serial      Serial1

#define LOOPTIME  1000
#define ACCMAX    200

// Acc
#define PIN_X  A3
#define PIN_Y  A4
#define PIN_Z  A5

// LED
#define TAPELED_PIN     3 //pin for LEDs WS2812B
#define TAPELED_NUM     12 // number of led

// COTTON LED
#define LED_R   22
#define LED_G   23
#define LED_B   24

// LED OP
#define DI()      asm("di")
#define EI()      asm("ei")
//#define NOP()     asm("nop")

// register for rapid access to pin
#define TAPELED_PINREG  P1.BIT.bit6
#define TAPELED_ON      TAPELED_PINREG = 1
#define TAPELED_OFF     TAPELED_PINREG = 0

// Wait routine to construct the signal for LEDs.
// Needed to transfer the signal during DI because tight timing.
#define TAPELED_ON_WAIT0   NOP();NOP();NOP();NOP();NOP();NOP();
#define TAPELED_OFF_WAIT0  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
#define TAPELED_ON_WAIT1   NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
#define TAPELED_OFF_WAIT1  NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();

//WiFi config
#define WLAN_SSID       ""  // Wi-Fi SSID
#define WLAN_PASS       ""  // Wi-Fi Password

//Milkcocoa config
#define MILKCOCOA_APP_ID      ""  // Milkcocoa APP ID
#define MILKCOCOA_DATASTORE   ""  // Milkcocoa Datastore
#define MILKCOCOA_SERVERPORT  1883

// Create an ESP8266Client class to connect to the MQTT server.
ESP8266Client wifi;

const char MQTT_SERVER[] PROGMEM    = MILKCOCOA_APP_ID ".mlkcca.com";
const char MQTT_CLIENTID[] PROGMEM  = __TIME__ MILKCOCOA_APP_ID;
Milkcocoa milkcocoa = Milkcocoa(&wifi, MQTT_SERVER, MILKCOCOA_SERVERPORT, MILKCOCOA_APP_ID, MQTT_CLIENTID);

void onpush(DataElement *pelem);

int x,y,z,ax,ay,az,ox,oy,oz,acc,kanpai;
int r,g,b;
int temp;

void sendLedBuf(uint8_t* red, uint8_t* green, uint8_t* blue, uint8_t num);
void sendLedData(uint8_t red, uint8_t green, uint8_t blue);

void updateAccInfo(){
    
    acc = 0;

    for(int i=0;i < 10;i++){
  // 3 axis read
  ox = x; oy = y; oz = z;
  x = analogRead(PIN_X);
  y = analogRead(PIN_Y);
  z = analogRead(PIN_Z);
  z = 1023-z; // 基盤の取付向きが逆
  ax = x-ox; ay = y-oy; az = z-oz;
  acc = max(acc, abs(ax) + abs(ay) + abs(az));
  delay(100);
    }

}

void logging(){
  Serial.println("3axis x:"+String(x)+" y:"+String(y)+" z:"+String(z)+" ax:"+String(ax)+" ay:"+String(ay)+" az:"+String(az));
  Serial.println("LED r:"+String(r)+" g:"+String(g)+" b:"+String(b));
  Serial.print("acc:");
  Serial.println(acc);
  Serial.print("kanpai:");
  Serial.println(kanpai);
  Serial.print("temp:");
  Serial.println(temp);
}

void setup() {
  // Serial
  Serial.begin(9600);
  delay(10);
  Serial.println("wait..");
  delay(5000);
  Serial.println();

  // Connect to WiFi access point.
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  wifi.begin(ESP_Serial, 115200);

  Serial.print("FW Version:");
  Serial.println(wifi.getVersion().c_str());

  while (!wifi.setOprToStation()) {
    Serial.println("to station retry");
    delay(3000);
  }
  Serial.println("to station ok");

  if (wifi.joinAP(WLAN_SSID, WLAN_PASS)) {
      Serial.println("Join AP success");
      Serial.print("IP: ");
      Serial.println(wifi.getLocalIP().c_str());    
  } else {
      Serial.println("Join AP failure");
  }

  if (wifi.disableMUX()) {
      Serial.println("single ok");
  } else {
      Serial.println("single err");
  }

  if(milkcocoa.on(MILKCOCOA_DATASTORE, "push", onpush)){
    Serial.println("milkcocoa on sucesss");   
  }
  else {
    Serial.println("milkcocoa on failure");   
    }

  // LED
  //setPowerManagementMode(PM_STOP_MODE);
  pinMode(TAPELED_PIN, OUTPUT);
  //pinMode(2, INPUT_PULLUP);
  //attachInterrupt(0, modeChange, FALLING);

  // Acc
  pinMode(PIN_X, INPUT);
  pinMode(PIN_Y, INPUT);
  pinMode(PIN_Z, INPUT);
  x=0,y=0,z=0,ax=0,ay=0,az=0,ox=0,oy=0,oz=0,acc=0,kanpai=0;
  r=0,g=0,b=0;
  temp=0;

  Serial.println("setup end");
  
}

void loop() {
  
  updateAccInfo();
  temp = (int) getTemperature(0);
  logging();
  
  if(acc > ACCMAX){
      kanpai = 1;
  }
  else{
      kanpai = 0;
  }
  
  if(kanpai){
      Serial.println("LED ON");
      for (int i = 0; i < TAPELED_NUM; i++){
          sendLedData(255, 0, 0);
      }
      analogWrite(LED_R, 0);
      analogWrite(LED_G, 255);
      analogWrite(LED_B, 255);

        milkcocoa.loop();
      DataElement elem = DataElement();
      if (abs(ax) > 255) {
	    elem.setValue("r", 255);
      } else {
        elem.setValue("r", abs(ax));
      }      		
      if (abs(ay) > 255) {
        elem.setValue("g", 255);
      } else {
        elem.setValue("g", abs(ay));
      }
      if (abs(az) > 255) {
        elem.setValue("b", 255);
      } else {
        elem.setValue("b", abs(az));
      }
//      elem.setValue("temp", temp);
//      elem.setValue("acc", acc);
//      elem.setValue("ax", ax);
//      elem.setValue("ay", ay);
//      elem.setValue("az", az);
      elem.setValue("kanpai", kanpai);
        milkcocoa.push(MILKCOCOA_DATASTORE, &elem);

  }
  else {
      Serial.println("LED OFF");
      for (int i = 0; i < TAPELED_NUM; i++){
      sendLedData(0, 0, 0);
    }
      analogWrite(LED_R, 255);
      analogWrite(LED_G, 255);
      analogWrite(LED_B, 255);
  }

  delay(LOOPTIME);

}

void onpush(DataElement *pelem) {
  Serial.println("onpush");
//  Serial.println(pelem->getInt("v"));
}

void sendLedBuf(uint8_t* red, uint8_t* green, uint8_t* blue, uint8_t num){
  for(int i = 0; i < num; i++){
    sendLedData(*(red + i), *(green + i), *(blue + i));
  }
}

void sendLedData(uint8_t red, uint8_t green, uint8_t blue){
  DI();
  if ( green & 0x80 )
  {   // on data send
    TAPELED_ON;
    TAPELED_ON_WAIT1;
    TAPELED_OFF;
    TAPELED_OFF_WAIT1;
  }
  else
  {   // off data send
    TAPELED_ON;
    TAPELED_ON_WAIT0;
    TAPELED_OFF;
    TAPELED_OFF_WAIT0;
  }

  if ( green & 0x40 )
  {   // on data send
    TAPELED_ON;
    TAPELED_ON_WAIT1;
    TAPELED_OFF;
    TAPELED_OFF_WAIT1;
  }
  else
  {   // off data send
    TAPELED_ON;
    TAPELED_ON_WAIT0;
    TAPELED_OFF;
    TAPELED_OFF_WAIT0;
  }

  if ( green & 0x20 )
  {   // on data send
    TAPELED_ON;
    TAPELED_ON_WAIT1;
    TAPELED_OFF;
    TAPELED_OFF_WAIT1;
  }
  else
  {   // off data send
    TAPELED_ON;
    TAPELED_ON_WAIT0;
    TAPELED_OFF;
    TAPELED_OFF_WAIT0;
  }

  if ( green & 0x10 )
  {   // on data send
    TAPELED_ON;
    TAPELED_ON_WAIT1;
    TAPELED_OFF;
    TAPELED_OFF_WAIT1;
  }
  else
  {   // off data send
    TAPELED_ON;
    TAPELED_ON_WAIT0;
    TAPELED_OFF;
    TAPELED_OFF_WAIT0;
  }

  if ( green & 0x08 )
  {   // on data send
    TAPELED_ON;
    TAPELED_ON_WAIT1;
    TAPELED_OFF;
    TAPELED_OFF_WAIT1;
  }
  else
  {   // off data send
    TAPELED_ON;
    TAPELED_ON_WAIT0;
    TAPELED_OFF;
    TAPELED_OFF_WAIT0;
  }

  if ( green & 0x04 )
  {   // on data send
    TAPELED_ON;
    TAPELED_ON_WAIT1;
    TAPELED_OFF;
    TAPELED_OFF_WAIT1;
  }
  else
  {   // off data send
    TAPELED_ON;
    TAPELED_ON_WAIT0;
    TAPELED_OFF;
    TAPELED_OFF_WAIT0;
  }

  if ( green & 0x02 )
  {   // on data send
    TAPELED_ON;
    TAPELED_ON_WAIT1;
    TAPELED_OFF;
    TAPELED_OFF_WAIT1;
  }
  else
  {   // off data send
    TAPELED_ON;
    TAPELED_ON_WAIT0;
    TAPELED_OFF;
    TAPELED_OFF_WAIT0;
  }

  if ( green & 0x01 )
  {   // on data send
    TAPELED_ON;
    TAPELED_ON_WAIT1;
    TAPELED_OFF;
    TAPELED_OFF_WAIT1;
  }
  else
  {   // off data send
    TAPELED_ON;
    TAPELED_ON_WAIT0;
    TAPELED_OFF;
    TAPELED_OFF_WAIT0;
  }


  // 襍､LED
  if ( red & 0x80 )
  {   // on data send
    TAPELED_ON;
    TAPELED_ON_WAIT1;
    TAPELED_OFF;
    TAPELED_OFF_WAIT1;
  }
  else
  {   // off data send
    TAPELED_ON;
    TAPELED_ON_WAIT0;
    TAPELED_OFF;
    TAPELED_OFF_WAIT0;
  }

  if ( red & 0x40 )
  {   // on data send
    TAPELED_ON;
    TAPELED_ON_WAIT1;
    TAPELED_OFF;
    TAPELED_OFF_WAIT1;
  }
  else
  {   // off data send
    TAPELED_ON;
    TAPELED_ON_WAIT0;
    TAPELED_OFF;
    TAPELED_OFF_WAIT0;
  }

  if ( red & 0x20 )
  {   // on data send
    TAPELED_ON;
    TAPELED_ON_WAIT1;
    TAPELED_OFF;
    TAPELED_OFF_WAIT1;
  }
  else
  {   // off data send
    TAPELED_ON;
    TAPELED_ON_WAIT0;
    TAPELED_OFF;
    TAPELED_OFF_WAIT0;
  }

  if ( red & 0x10 )
  {   // on data send
    TAPELED_ON;
    TAPELED_ON_WAIT1;
    TAPELED_OFF;
    TAPELED_OFF_WAIT1;
  }
  else
  {   // off data send
    TAPELED_ON;
    TAPELED_ON_WAIT0;
    TAPELED_OFF;
    TAPELED_OFF_WAIT0;
  }

  if ( red & 0x08 )
  {   // on data send
    TAPELED_ON;
    TAPELED_ON_WAIT1;
    TAPELED_OFF;
    TAPELED_OFF_WAIT1;
  }
  else
  {   // off data send
    TAPELED_ON;
    TAPELED_ON_WAIT0;
    TAPELED_OFF;
    TAPELED_OFF_WAIT0;
  }

  if ( red & 0x04 )
  {   // on data send
    TAPELED_ON;
    TAPELED_ON_WAIT1;
    TAPELED_OFF;
    TAPELED_OFF_WAIT1;
  }
  else
  {   // off data send
    TAPELED_ON;
    TAPELED_ON_WAIT0;
    TAPELED_OFF;
    TAPELED_OFF_WAIT0;
  }

  if ( red & 0x02 )
  {   // on data send
    TAPELED_ON;
    TAPELED_ON_WAIT1;
    TAPELED_OFF;
    TAPELED_OFF_WAIT1;
  }
  else
  {   // off data send
    TAPELED_ON;
    TAPELED_ON_WAIT0;
    TAPELED_OFF;
    TAPELED_OFF_WAIT0;
  }

  if ( red & 0x01 )
  {   // on data send
    TAPELED_ON;
    TAPELED_ON_WAIT1;
    TAPELED_OFF;
    TAPELED_OFF_WAIT1;
  }
  else
  {   // off data send
    TAPELED_ON;
    TAPELED_ON_WAIT0;
    TAPELED_OFF;
    TAPELED_OFF_WAIT0;
  }


  // 髱鱈ED
  if ( blue & 0x80 )
  {   // on data send
    TAPELED_ON;
    TAPELED_ON_WAIT1;
    TAPELED_OFF;
    TAPELED_OFF_WAIT1;
  }
  else
  {   // off data send
    TAPELED_ON;
    TAPELED_ON_WAIT0;
    TAPELED_OFF;
    TAPELED_OFF_WAIT0;
  }

  if ( blue & 0x40 )
  {   // on data send
    TAPELED_ON;
    TAPELED_ON_WAIT1;
    TAPELED_OFF;
    TAPELED_OFF_WAIT1;
  }
  else
  {   // off data send
    TAPELED_ON;
    TAPELED_ON_WAIT0;
    TAPELED_OFF;
    TAPELED_OFF_WAIT0;
  }

  if ( blue & 0x20 )
  {   // on data send
    TAPELED_ON;
    TAPELED_ON_WAIT1;
    TAPELED_OFF;
    TAPELED_OFF_WAIT1;
  }
  else
  {   // off data send
    TAPELED_ON;
    TAPELED_ON_WAIT0;
    TAPELED_OFF;
    TAPELED_OFF_WAIT0;
  }

  if ( blue & 0x10 )
  {   // on data send
    TAPELED_ON;
    TAPELED_ON_WAIT1;
    TAPELED_OFF;
    TAPELED_OFF_WAIT1;
  }
  else
  {   // off data send
    TAPELED_ON;
    TAPELED_ON_WAIT0;
    TAPELED_OFF;
    TAPELED_OFF_WAIT0;
  }

  if ( blue & 0x08 )
  {   // on data send
    TAPELED_ON;
    TAPELED_ON_WAIT1;
    TAPELED_OFF;
    TAPELED_OFF_WAIT1;
  }
  else
  {   // off data send
    TAPELED_ON;
    TAPELED_ON_WAIT0;
    TAPELED_OFF;
    TAPELED_OFF_WAIT0;
  }

  if ( blue & 0x04 )
  {   // on data send
    TAPELED_ON;
    TAPELED_ON_WAIT1;
    TAPELED_OFF;
    TAPELED_OFF_WAIT1;
  }
  else
  {   // off data send
    TAPELED_ON;
    TAPELED_ON_WAIT0;
    TAPELED_OFF;
    TAPELED_OFF_WAIT0;
  }

  if ( blue & 0x02 )
  {   // on data send
    TAPELED_ON;
    TAPELED_ON_WAIT1;
    TAPELED_OFF;
    TAPELED_OFF_WAIT1;
  }
  else
  {   // off data send
    TAPELED_ON;
    TAPELED_ON_WAIT0;
    TAPELED_OFF;
    TAPELED_OFF_WAIT0;
  }

  if ( blue & 0x01 )
  {   // on data send
    TAPELED_ON;
    TAPELED_ON_WAIT1;
    TAPELED_OFF;
    TAPELED_OFF_WAIT1;
  }
  else
  {   // off data send
    TAPELED_ON;
    TAPELED_ON_WAIT0;
    TAPELED_OFF;
    TAPELED_OFF_WAIT0;
  }
  EI();
}



