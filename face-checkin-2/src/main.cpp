#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <DHT.h>               
#include <time.h>
#include <Fonts/FreeSans12pt7b.h>


#include "1.h"   // img1[240*320]
#include "2.h"   // img2[240*320]
#include "3.h"   // img3[240*320]

// — PINOUT —
#define TFT_CS    5
#define TFT_DC    16
#define TFT_RST   17
#define TFT_BL    4
#define TFT_SCK   18
#define TFT_MOSI  23

#define LED_R     19
#define LED_G     21
#define LED_B     22
#define BUZZER    27

#define DHTPIN    14
#define DHTTYPE   DHT11

#define SCREEN_W  240
#define SCREEN_H  320

#define ALL_LEDS_OFF()  
  do {                  
    digitalWrite(LED_R, LOW); 
    digitalWrite(LED_G, LOW); 
    digitalWrite(LED_B, LOW); 
  } while(0)

// MQTT Key and Wifi
const char* WIFI_SSID   = "vivo S17e";
const char* WIFI_PASS   = "minh1710";
const char* MQTT_SERVER = "io.adafruit.com";
const uint16_t MQTT_PORT= 1883;
const char* AIO_USER    = "nhatminh1710";
const char* AIO_KEY     = "key";
const char* FEED_TOPIC  = "feed";

// Screen
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
WiFiClient    netClient;
PubSubClient  mqttClient(netClient);
DHT           dht(DHTPIN, DHTTYPE);

float currentTemp = NAN;

// Buzzer
void beep() {
  digitalWrite(BUZZER, LOW);
  vTaskDelay(pdMS_TO_TICKS(200));
  digitalWrite(BUZZER, HIGH);
}

void drawWifiIcon(int bars) {
  const int w=4, spacing=2;
  const int heights[5]={8,12,16,20,24};
  for(int i=0;i<bars && i<5;i++){
    int x=5 + i*(w+spacing);
    int h=heights[i];
    int y=SCREEN_H-5-h;
    tft.fillRect(x,y,w,h,ST77XX_WHITE);
  }
}

void displayNormal(){
  ALL_LEDS_OFF();
  tft.drawRGBBitmap(0,0,img1,SCREEN_W,SCREEN_H);
  struct tm tmInfo;
  if(!getLocalTime(&tmInfo)) return;

  char buf[32];
  int16_t x,y; uint16_t w,h;
  tft.setFont(&FreeSans12pt7b);
  tft.setTextColor(ST77XX_WHITE);

  // Time top-left
  snprintf(buf,sizeof(buf),"%02d:%02d",tmInfo.tm_hour,tmInfo.tm_min);
  tft.getTextBounds(buf,0,0,&x,&y,&w,&h);
  tft.setCursor(5,h+2);
  tft.print(buf);

  // Date top-right
  snprintf(buf,sizeof(buf),"%02d/%02d/%04d",tmInfo.tm_mday,tmInfo.tm_mon+1,tmInfo.tm_year+1900);
  tft.getTextBounds(buf,0,0,&x,&y,&w,&h);
  tft.setCursor(SCREEN_W-w-5,h+2);
  tft.print(buf);

  // Temp bottom-right
  if(!isnan(currentTemp)){
    snprintf(buf,sizeof(buf),"%.1f°C",currentTemp);
    tft.getTextBounds(buf,0,0,&x,&y,&w,&h);
    tft.setCursor(SCREEN_W-w-5,SCREEN_H-5);
    tft.print(buf);
  }

  // Wi-Fi icon
  int32_t rssi=WiFi.RSSI();
  int bars = (rssi>-55?5:rssi>-80?4:rssi>-100?3:rssi>-130?2:rssi>-150?1:0);
  drawWifiIcon(bars);
}

void displayResult(bool ok){
 
  tft.drawRGBBitmap(0,0, ok?img2:img3, SCREEN_W, SCREEN_H);
  digitalWrite(LED_R, ok?LOW:HIGH);
  digitalWrite(LED_G, ok?HIGH:LOW);
  digitalWrite(LED_B, LOW);
  beep();
  vTaskDelay(pdMS_TO_TICKS(2000));
  displayNormal();
}

// MQTT
void connectMQTT(){
  while(!mqttClient.connected()){
    if(mqttClient.connect("ESP32Client",AIO_USER,AIO_KEY)){
      mqttClient.subscribe(FEED_TOPIC);
    } else vTaskDelay(pdMS_TO_TICKS(2000));
  }
}
void mqttCallback(char* t, byte* p, unsigned int l){
  char msg[l+1]; memcpy(msg,p,l); msg[l]=0;
  displayResult(atoi(msg)==1);
}

void TaskMqtt(void*){
  mqttClient.setServer(MQTT_SERVER,MQTT_PORT);
  mqttClient.setCallback(mqttCallback);
  connectMQTT();
  for(;;){ if(!mqttClient.connected()) connectMQTT(); mqttClient.loop(); vTaskDelay(pdMS_TO_TICKS(10)); }
}
void TaskSensor(void*){
  for(;;){
    float t=dht.readTemperature();
    if(!isnan(t)) currentTemp=t;
    vTaskDelay(pdMS_TO_TICKS(120000));
  }
}
void TaskDisplay(void*){
  displayNormal();
  for(;;){ vTaskDelay(pdMS_TO_TICKS(60000)); displayNormal(); }
}

void setup(){
  Serial.begin(115200);
  dht.begin();
  pinMode(TFT_BL,OUTPUT); digitalWrite(TFT_BL, HIGH);
  SPI.begin(TFT_SCK,19,TFT_MOSI,TFT_CS);
  tft.init(SCREEN_W,SCREEN_H); tft.setRotation(0);
  tft.setSPISpeed(40000000UL);
  tft.setRotation(0);
  displayNormal();
  WiFi.begin(WIFI_SSID,WIFI_PASS);
  unsigned long t0=millis();
  while(WiFi.status()!=WL_CONNECTED && millis()-t0<10000) delay(200);
  configTime(7*3600,0,"pool.ntp.org","time.nist.gov");
  pinMode(LED_R,OUTPUT);pinMode(LED_G,OUTPUT);pinMode(LED_B,OUTPUT);pinMode(BUZZER,OUTPUT);
  digitalWrite(LED_R,LOW);digitalWrite(LED_G,LOW);digitalWrite(LED_B,LOW);digitalWrite(BUZZER,HIGH);
  xTaskCreatePinnedToCore(TaskMqtt,"MQTT",4096,NULL,1,NULL,1);
  xTaskCreatePinnedToCore(TaskSensor,"Sensor",2048,NULL,1,NULL,1);
  xTaskCreatePinnedToCore(TaskDisplay,"Display",4096,NULL,1,NULL,1);
}
void loop(){}
