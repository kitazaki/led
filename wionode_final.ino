#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>
#include <Milkcocoa.h>

#define WLAN_SSID       ""  // Wi-Fi SSID
#define WLAN_PASS       ""  // Wi-Fi Password

#define MILKCOCOA_APP_ID      ""  // Milkcocoa APP ID
#define MILKCOCOA_DATASTORE   ""  // Milkcocoa Datastore
#define MILKCOCOA_SERVERPORT  1883

#define MOTOR 5 // Grove Port 1 IO4 or IO5
#define PIN 4   // Grove Port 1 IO4 or IO5
#define NUMPIXELS 16   // Number of LEDs
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

WiFiClient client;

const char MQTT_SERVER[] PROGMEM    = MILKCOCOA_APP_ID ".mlkcca.com";
const char MQTT_CLIENTID[] PROGMEM  = __TIME__ MILKCOCOA_APP_ID;

Milkcocoa milkcocoa = Milkcocoa(&client, MQTT_SERVER, MILKCOCOA_SERVERPORT, MILKCOCOA_APP_ID, MQTT_CLIENTID);

int red, green, blue, strength;

void onpush(DataElement *elem) {
  Serial.println("onpush");
  Serial.print("r:");
  Serial.print(elem->getInt("r"));
  Serial.print(",g:");
  Serial.print(elem->getInt("g"));
  Serial.print(",b:");
  Serial.print(elem->getInt("b"));
  Serial.print(",s:");
//  Serial.println(elem->getInt("s"));
//  strength = elem->getInt("s");
  Serial.print("50");
  strength = 100;
  Serial.println();

//  led(elem->getInt("r"), elem->getInt("g"), elem->getInt("b"), elem->getInt("s"));
// set RGBs to global parameters
  red = elem->getInt("r");
  green = elem->getInt("g");
  blue = elem->getInt("b");
  
  int type;
  type = (red + green + blue) % 6;

  //  types of coloring
  if (type == 0 ) {
    //  type 1
    vibration_w(200);
    led_wipe_a(red, green, blue, strength);
  } else if (type == 1) {
    //  type 2
    vibration(300);
    led_wipe_b(red, green, blue, strength);
  } else if (type == 2) {
    //  type 3
    rainbow(strength, 20);
  } else if (type == 3) {
    //  type 4
    rainbowCycle(strength, 20);
  } else if (type == 4) {
    //  type 5
    theaterChaseRainbow(strength, 50);
  } else if (type == 5) {  
    //  type 6
    theaterChase(strip.Color(red, green, blue), strength, 50);
  } else {
    led();
  }
};

void led() {
  strip.setBrightness(10);
  for(int i=0;i<NUMPIXELS;i++){
    strip.setPixelColor(i, strip.Color(255,255,255));
    strip.show();
    delay(50);
  }
  // vibration(500);
}

void vibration(int duration) {
  digitalWrite(MOTOR, 1);
  delay(duration);
  digitalWrite(MOTOR, 0);
  delay(duration);
}

void vibration_w(int duration) {
  digitalWrite(MOTOR, 1);
  delay(duration);
  digitalWrite(MOTOR, 0);
  delay(duration);
  digitalWrite(MOTOR, 1);
  delay(duration);
  digitalWrite(MOTOR, 0);
  delay(duration);
}

void led_wipe_a(int v_r, int v_g, int v_b, int v_s) {
  strip.setBrightness(v_s);
  for(int i=0;i<NUMPIXELS;i++){
    strip.setPixelColor(i, strip.Color(v_r,v_g,v_b));
    strip.show();
    delay(50);
  }
  for(int i=0;i<NUMPIXELS;i++){
//    strip.setPixelColor(i, strip.Color(0,0,0));
    strip.setPixelColor(NUMPIXELS - i - 1, strip.Color(0,0,0));
    strip.show();
    delay(50);
  }
}

void led_wipe_b(int v_r, int v_g, int v_b, int v_s) {
  strip.setBrightness(v_s);
  for(int i=0;i<NUMPIXELS;i++){
    strip.setPixelColor(i, strip.Color(v_r,v_g,v_b));
    strip.show();
    delay(50);
  }
  for(int i=0;i<NUMPIXELS;i++){
    strip.setPixelColor(i, strip.Color(0,0,0));
//    strip.setPixelColor(NUMPIXELS - i - 1, strip.Color(0,0,0));
    strip.show();
    delay(50);
  }
}

void rainbow(int v_s, uint8_t wait) {
  uint16_t i, j;
  strip.setBrightness(v_s);
  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

void rainbowCycle(int v_s, uint8_t wait) {
  uint16_t i, j;
  strip.setBrightness(v_s);
  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

void theaterChase(uint32_t c, int v_s, uint8_t wait) {
  strip.setBrightness(v_s);
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();
      delay(wait);
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

void theaterChaseRainbow(int v_s, uint8_t wait) {
  strip.setBrightness(v_s);
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();
      delay(wait);
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void setupWiFi() {
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


// main
void setup() {
  pinMode(15, OUTPUT);
  digitalWrite(15, 1);
  pinMode(MOTOR, OUTPUT);
  Serial.begin(115200);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
//  delay(10);
  setupWiFi();
  Serial.println( milkcocoa.on(MILKCOCOA_DATASTORE, "push", onpush) );
  vibration(500);
}

void loop() {
  milkcocoa.loop();
  led();
//  delay(100);
}

