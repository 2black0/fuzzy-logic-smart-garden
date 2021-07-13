#include <Arduino.h>
#line 1 "/Users/thinkmac/Documents/GitHub/fuzzy-logic-smart-garden/main.ino"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <LiquidCrystal_I2C.h>

#define SensorPin A0
#define DHTPIN 2
#define DHTTYPE DHT11

DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;

float temp;
float hum;

LiquidCrystal_I2C lcd(0x27, 16, 2);

#line 18 "/Users/thinkmac/Documents/GitHub/fuzzy-logic-smart-garden/main.ino"
void setup();
#line 31 "/Users/thinkmac/Documents/GitHub/fuzzy-logic-smart-garden/main.ino"
void loop();
#line 43 "/Users/thinkmac/Documents/GitHub/fuzzy-logic-smart-garden/main.ino"
void serial_show(bool line1, String text1, bool line2, String text2, bool line3, String text3, bool line4, String text4);
#line 66 "/Users/thinkmac/Documents/GitHub/fuzzy-logic-smart-garden/main.ino"
void lcd_show(String text1, String text2, int delay_lcd);
#line 75 "/Users/thinkmac/Documents/GitHub/fuzzy-logic-smart-garden/main.ino"
float read_temp();
#line 89 "/Users/thinkmac/Documents/GitHub/fuzzy-logic-smart-garden/main.ino"
float read_soil();
#line 18 "/Users/thinkmac/Documents/GitHub/fuzzy-logic-smart-garden/main.ino"
void setup()
{
  Serial.begin(9600);
  lcd.begin();
  dht.begin();
  sensor_t sensor;
  delayMS = sensor.min_delay / 1000;

  lcd.backlight();
  serial_show(0, "Fuzzy Logic Smart Garden", 0, "", 0, "", 0, "");
  lcd_show("Fuzzy Logic", "Smart Garden", 2500);
}

void loop()
{
  temp = read_temp();
  hum = read_soil();

  serial_show(0, "Temperature: ", 0, String(temp), 0, "°C", 0, "");
  serial_show(0, "Soil Moisture: ", 0, String(hum), 0, "%", 0, "");

  lcd_show("t:"+String(temp)+"C", "h:"+String(hum)+"%", 100);
  delay(delayMS);
}

void serial_show(bool line1, String text1, bool line2, String text2, bool line3, String text3, bool line4, String text4){
  if(line1){
    Serial.println(text1);
  } else {
    Serial.print(text1);
  }
  if(line2){
    Serial.println(text2);
  } else {
    Serial.print(text2);
  }
  if(line3){
    Serial.println(text3);
  } else {
    Serial.print(text3);
  }
  if(line4){
    Serial.println(text4);
  } else {
    Serial.print(text4);
  }
}

void lcd_show(String text1, String text2, int delay_lcd)
{
  lcd.setCursor(0,0);
  lcd.print(text1);
  lcd.setCursor(1,0);
  lcd.print(text2);
  delay(delay_lcd);
}

float read_temp()
{
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature))
  {
    return 0;
  }
  else
  {
    return event.temperature;
  }
}

float read_soil()
{
  float sensorValue = 0;
  for (int i = 0; i <= 100; i++)
  {
    sensorValue = sensorValue + analogRead(SensorPin);
    delay(1);
  }
  sensorValue = sensorValue / 100.0;
  return sensorValue;
}
