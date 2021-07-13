# 1 "/Users/thinkmac/Documents/GitHub/fuzzy-logic-smart-garden/main.ino"
# 2 "/Users/thinkmac/Documents/GitHub/fuzzy-logic-smart-garden/main.ino" 2
# 3 "/Users/thinkmac/Documents/GitHub/fuzzy-logic-smart-garden/main.ino" 2
# 4 "/Users/thinkmac/Documents/GitHub/fuzzy-logic-smart-garden/main.ino" 2
# 5 "/Users/thinkmac/Documents/GitHub/fuzzy-logic-smart-garden/main.ino" 2





DHT_Unified dht(2, 11 /**< DHT TYPE 11 */);
uint32_t delayMS;

float temp;
float hum;

LiquidCrystal_I2C lcd(0x27, 16, 2);

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
    sensorValue = sensorValue + analogRead(A0);
    delay(1);
  }
  sensorValue = sensorValue / 100.0;
  return sensorValue;
}
