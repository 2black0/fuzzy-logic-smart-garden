// created by 2black0@gmail.com
// 2021

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Fuzzy.h>

#define SensorPin A0

char auth[] = "YourAuthToken";   // ganti pakai token dari blynk
char ssid[] = "YourNetworkName"; // ganti wifi ssid
char pass[] = "YourPassword";    // ganti wifi password

DHT_Unified dht(D4, DHT11);
BlynkTimer timer;

uint32_t delayMS;
float temp;
float soil;

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup()
{
  Serial.begin(9600);
  lcd.begin();
  dht.begin();
  Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 9600);

  lcd.backlight();
  serial_show(0, "Fuzzy Logic Smart Garden", 0, "", 0, "", 0, "");
  lcd_show("Fuzzy Logic", "Smart Garden", 2500);

  timer.setInterval(2000, send_data);
}

void loop()
{
  Blynk.run();
  timer.run();
}

void send_data()
{
  temp = read_temp();
  soil = read_soil();

  serial_show(0, "Temperature: ", 0, String(temp), 0, "°C", 0, "");
  serial_show(0, "Soil Moisture: ", 0, String(soil), 0, "%", 0, "");

  Blynk.virtualWrite(V0, temp);
  Blynk.virtualWrite(V1, soil);

  lcd_show("t:" + String(temp) + "C", "h:" + String(soil) + "%", 100);
}

void serial_show(bool line1, String text1, bool line2, String text2, bool line3, String text3, bool line4, String text4)
{
  if (line1)
  {
    Serial.println(text1);
  }
  else
  {
    Serial.print(text1);
  }
  if (line2)
  {
    Serial.println(text2);
  }
  else
  {
    Serial.print(text2);
  }
  if (line3)
  {
    Serial.println(text3);
  }
  else
  {
    Serial.print(text3);
  }
  if (line4)
  {
    Serial.println(text4);
  }
  else
  {
    Serial.print(text4);
  }
}

void lcd_show(String text1, String text2, int delay_lcd)
{
  lcd.setCursor(0, 0);
  lcd.print(text1);
  lcd.setCursor(1, 0);
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