// created by 2black0@gmail.com
// 2021

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Fuzzy.h>

#define soilPin A0
#define pumpPin 14
#define pinMotorIN1 12
#define pinMotorIN2 13

char auth[] = "YourAuthToken";   // ganti pakai token dari blynk
char ssid[] = "YourNetworkName"; // ganti wifi ssid
char pass[] = "YourPassword";    // ganti wifi password

DHT_Unified dht(D4, DHT11);
BlynkTimer timer;
Fuzzy *fuzzy = new Fuzzy();

uint32_t delayMS;
float temp;
float soil;

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup()
{
  Serial.begin(9600);

  pinMode(pinMotorIN1, OUTPUT);
  pinMode(pinMotorIN2, OUTPUT);
  digitalWrite(pinMotorIN1, LOW);
  digitalWrite(pinMotorIN2, HIGH);
  
  lcd.begin();
  dht.begin();
  Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 9600);

  lcd.backlight();
  serial_show(0, "Fuzzy Logic Smart Garden", 0, "", 0, "", 0, "");
  lcd_show("Fuzzy Logic", "Smart Garden", 2500);

  fuzzy_system();

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

  fuzzy->setInput(1, temp);
  fuzzy->setInput(2, soil);
  fuzzy->fuzzify();

  float pump = fuzzy->defuzzify(1);
  int pumpS = map(pump, 0, 100, 0, 1023);
  analogWrite(pumpPin, pumpS);

  serial_show(0, "Temperature: ", 0, String(temp), 0, "°C", 1, "");
  serial_show(0, "Soil Moisture: ", 0, String(soil), 0, "%", 1, "");
  serial_show(0, "Water Pump: ", 0, String(pump), 0, "%", 1, "");

  Blynk.virtualWrite(V0, temp);
  Blynk.virtualWrite(V1, soil);
  Blynk.virtualWrite(V2, pump);

  lcd_show("t:" + String(temp) + "C h:" + String(soil) + "%", "w:" + String(pump) + "%", 100);
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
    sensorValue = sensorValue + analogRead(soilPin);
    delay(1);
  }
  sensorValue = sensorValue / 100.0;
  float sensorValues = map(sensorValue, 0, 1023, 0, 100);
  return sensorValue;
}

void fuzzy_system()
{
  FuzzyInput *temperature = new FuzzyInput(1);
  FuzzySet *cold = new FuzzySet(0, 11, 11, 22);
  temperature->addFuzzySet(cold);
  FuzzySet *warm = new FuzzySet(22, 24.5, 24.5, 27);
  temperature->addFuzzySet(warm);
  FuzzySet *hot = new FuzzySet(27, 31, 31, 35);
  temperature->addFuzzySet(hot);
  fuzzy->addFuzzyInput(temperature);

  FuzzyInput *soil = new FuzzyInput(2);
  FuzzySet *dry = new FuzzySet(0, 25, 25, 50);
  soil->addFuzzySet(dry);
  FuzzySet *normal = new FuzzySet(40, 50, 50, 60);
  soil->addFuzzySet(normal);
  FuzzySet *wet = new FuzzySet(50, 75, 75, 100);
  soil->addFuzzySet(wet);
  fuzzy->addFuzzyInput(soil);

  FuzzyOutput *pump = new FuzzyOutput(1);
  FuzzySet *veryslow = new FuzzySet(0, 0, 0, 25);
  pump->addFuzzySet(veryslow);
  FuzzySet *slow = new FuzzySet(0, 25, 25, 50);
  pump->addFuzzySet(slow);
  FuzzySet *average = new FuzzySet(25, 50, 50, 75);
  pump->addFuzzySet(average);
  FuzzySet *fast = new FuzzySet(50, 75, 75, 100);
  pump->addFuzzySet(fast);
  FuzzySet *veryfast = new FuzzySet(75, 100, 100, 100);
  pump->addFuzzySet(veryfast);
  fuzzy->addFuzzyOutput(pump);

  FuzzyRuleAntecedent *ifSoilDryAndTemperatureCold = new FuzzyRuleAntecedent();
  ifSoilDryAndTemperatureCold->joinWithAND(dry, cold);
  FuzzyRuleConsequent *thenPumpAverage = new FuzzyRuleConsequent();
  thenPumpAverage->addOutput(average);
  FuzzyRule *fuzzyRule01 = new FuzzyRule(1, ifSoilDryAndTemperatureCold, thenPumpAverage);
  fuzzy->addFuzzyRule(fuzzyRule01);

  FuzzyRuleAntecedent *ifSoilDryAndTemperatureWarm = new FuzzyRuleAntecedent();
  ifSoilDryAndTemperatureWarm->joinWithAND(dry, warm);
  FuzzyRuleConsequent *thenPumpFast = new FuzzyRuleConsequent();
  thenPumpFast->addOutput(fast);
  FuzzyRule *fuzzyRule02 = new FuzzyRule(2, ifSoilDryAndTemperatureWarm, thenPumpFast);
  fuzzy->addFuzzyRule(fuzzyRule02);

  FuzzyRuleAntecedent *ifSoilDryAndTemperatureHot = new FuzzyRuleAntecedent();
  ifSoilDryAndTemperatureHot->joinWithAND(dry, hot);
  FuzzyRuleConsequent *thenPumpVeryFast = new FuzzyRuleConsequent();
  thenPumpVeryFast->addOutput(veryfast);
  FuzzyRule *fuzzyRule03 = new FuzzyRule(3, ifSoilDryAndTemperatureHot, thenPumpVeryFast);
  fuzzy->addFuzzyRule(fuzzyRule03);

  FuzzyRuleAntecedent *ifSoilNormalAndTemperatureCold = new FuzzyRuleAntecedent();
  ifSoilNormalAndTemperatureCold->joinWithAND(normal, cold);
  FuzzyRuleConsequent *thenPumpSlow = new FuzzyRuleConsequent();
  thenPumpSlow->addOutput(slow);
  FuzzyRule *fuzzyRule04 = new FuzzyRule(4, ifSoilNormalAndTemperatureCold, thenPumpSlow);
  fuzzy->addFuzzyRule(fuzzyRule04);

  FuzzyRuleAntecedent *ifSoilNormalAndTemperatureWarm = new FuzzyRuleAntecedent();
  ifSoilNormalAndTemperatureWarm->joinWithAND(normal, warm);
  //FuzzyRuleConsequent *thenPumpAverage = new FuzzyRuleConsequent();
  //thenPumpAverage->addOutput(average);
  FuzzyRule *fuzzyRule05 = new FuzzyRule(5, ifSoilNormalAndTemperatureWarm, thenPumpAverage);
  fuzzy->addFuzzyRule(fuzzyRule05);

  FuzzyRuleAntecedent *ifSoilNormalAndTemperatureHot = new FuzzyRuleAntecedent();
  ifSoilNormalAndTemperatureHot->joinWithAND(normal, hot);
  //FuzzyRuleConsequent *thenPumpFast = new FuzzyRuleConsequent();
  //thenPumpFast->addOutput(fast);
  FuzzyRule *fuzzyRule06 = new FuzzyRule(6, ifSoilNormalAndTemperatureHot, thenPumpFast);
  fuzzy->addFuzzyRule(fuzzyRule06);

  FuzzyRuleAntecedent *ifSoilWetAndTemperatureCold = new FuzzyRuleAntecedent();
  ifSoilWetAndTemperatureCold->joinWithAND(wet, cold);
  FuzzyRuleConsequent *thenPumpVerySlow = new FuzzyRuleConsequent();
  thenPumpVerySlow->addOutput(veryslow);
  FuzzyRule *fuzzyRule07 = new FuzzyRule(7, ifSoilWetAndTemperatureCold, thenPumpVerySlow);
  fuzzy->addFuzzyRule(fuzzyRule07);

  FuzzyRuleAntecedent *ifSoilWetAndTemperatureWarm = new FuzzyRuleAntecedent();
  ifSoilWetAndTemperatureWarm->joinWithAND(wet, warm);
  //FuzzyRuleConsequent *thenPumpSlow = new FuzzyRuleConsequent();
  //thenPumpSlow->addOutput(slow);
  FuzzyRule *fuzzyRule08 = new FuzzyRule(8, ifSoilWetAndTemperatureWarm, thenPumpSlow);
  fuzzy->addFuzzyRule(fuzzyRule08);

  FuzzyRuleAntecedent *ifSoilWetAndTemperatureHot = new FuzzyRuleAntecedent();
  ifSoilWetAndTemperatureHot->joinWithAND(wet, hot);
  //FuzzyRuleConsequent *thenPumpAverage = new FuzzyRuleConsequent();
  //thenPumpAverage->addOutput(slow);
  FuzzyRule *fuzzyRule09 = new FuzzyRule(9, ifSoilWetAndTemperatureHot, thenPumpAverage);
  fuzzy->addFuzzyRule(fuzzyRule09);
}
