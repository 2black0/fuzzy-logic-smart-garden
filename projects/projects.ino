/**
 * Fuzzy Logic Smart Garden Controller
 * 
 * This program implements a smart garden watering system using fuzzy logic to determine
 * optimal watering rates based on soil moisture and temperature readings.
 * 
 * Hardware Components:
 * - ESP8266 WiFi Module
 * - DHT11 Temperature Sensor
 * - Soil Moisture Sensor
 * - Water Pump
 * - 16x2 I2C LCD Display
 * - Motor Driver (for water pump control)
 * 
 * Features:
 * - Automatic watering based on environmental conditions
 * - Fuzzy logic control system with 9 rules
 * - Blynk IoT integration for remote monitoring
 * - Real-time status display on LCD
 * - Serial debugging output
 * 
 * Created by: 2black0@gmail.com
 * Year: 2021
 */

// Include necessary libraries
#include <Adafruit_Sensor.h>  // Base sensor library for DHT sensor
#include <DHT.h>              // DHT sensor library
#include <DHT_U.h>            // DHT sensor unified sensor library
#include <LiquidCrystal_I2C.h> // Library for I2C LCD display
#include <ESP8266WiFi.h>      // WiFi functionality for ESP8266
#include <BlynkSimpleEsp8266.h> // Blynk IoT platform integration
#include <Fuzzy.h>            // Fuzzy logic library for control system

// Define pins
#define soilPin A0            // Analog pin for soil moisture sensor
#define pumpPin 14            // Digital pin for pump control (PWM)
#define pinMotorIN1 12        // Motor driver input 1
#define pinMotorIN2 13        // Motor driver input 2

// Network credentials and Blynk authentication
char auth[] = "YourAuthToken";   // Authentication token from Blynk app
char ssid[] = "YourNetworkName"; // WiFi network name
char pass[] = "YourPassword";    // WiFi password

// Initialize components
DHT_Unified dht(D4, DHT11);    // DHT sensor on pin D4
BlynkTimer timer;              // Timer for periodic tasks
Fuzzy *fuzzy = new Fuzzy();    // Fuzzy logic controller

// Global variables
uint32_t delayMS;             // Delay variable for sensor readings
float temp;                   // Current temperature reading
float soil;                   // Current soil moisture reading

LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD at I2C address 0x27, 16x2 display

/**
 * Setup function - runs once at startup
 * Initializes all hardware components, sets up Blynk connection,
 * and configures the fuzzy logic system
 */
void setup()
{
  Serial.begin(9600);         // Initialize serial communication

  // Configure motor driver pins
  pinMode(pinMotorIN1, OUTPUT);
  pinMode(pinMotorIN2, OUTPUT);
  digitalWrite(pinMotorIN1, LOW);
  digitalWrite(pinMotorIN2, HIGH);
  
  lcd.begin();                // Initialize LCD
  dht.begin();                // Initialize DHT sensor
  Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 9600); // Connect to Blynk

  lcd.backlight();            // Turn on LCD backlight
  serial_show(0, "Fuzzy Logic Smart Garden", 0, "", 0, "", 0, ""); // Print welcome message to serial
  lcd_show("Fuzzy Logic", "Smart Garden", 2500); // Show welcome message on LCD for 2.5 seconds

  fuzzy_system();             // Setup fuzzy logic controller

  timer.setInterval(2000, send_data); // Schedule data reading every 2 seconds
}

/**
 * Main loop function - runs repeatedly
 * Handles Blynk communication and timer events
 */
void loop()
{
  Blynk.run();                // Process Blynk communication
  timer.run();                // Process timer events
}

/**
 * Function to read sensors, process fuzzy logic, and send data
 * Called periodically by the timer
 */
void send_data()
{
  // Read sensor data
  temp = read_temp();         // Get temperature reading
  soil = read_soil();         // Get soil moisture reading

  // Process with fuzzy logic
  fuzzy->setInput(1, temp);   // Set temperature as first input
  fuzzy->setInput(2, soil);   // Set soil moisture as second input
  fuzzy->fuzzify();           // Process inputs through fuzzy system

  // Get pump control output and apply it
  float pump = fuzzy->defuzzify(1);  // Get defuzzified output (0-100%)
  int pumpS = map(pump, 0, 100, 0, 1023); // Map to PWM range
  analogWrite(pumpPin, pumpS);      // Set pump speed via PWM

  // Output results to serial monitor
  serial_show(0, "Temperature: ", 0, String(temp), 0, "°C", 1, "");
  serial_show(0, "Soil Moisture: ", 0, String(soil), 0, "%", 1, "");
  serial_show(0, "Water Pump: ", 0, String(pump), 0, "%", 1, "");

  // Send data to Blynk app
  Blynk.virtualWrite(V0, temp);  // Temperature to virtual pin V0
  Blynk.virtualWrite(V1, soil);  // Soil moisture to virtual pin V1
  Blynk.virtualWrite(V2, pump);  // Pump status to virtual pin V2

  // Update LCD with current readings
  lcd_show("t:" + String(temp) + "C h:" + String(soil) + "%", "w:" + String(pump) + "%", 100);
}

/**
 * Helper function for printing to serial monitor
 * Provides flexibility for formatting output
 * 
 * @param line1   Boolean - whether to add newline after text1
 * @param text1   String - first text to print
 * @param line2   Boolean - whether to add newline after text2
 * @param text2   String - second text to print
 * @param line3   Boolean - whether to add newline after text3
 * @param text3   String - third text to print
 * @param line4   Boolean - whether to add newline after text4
 * @param text4   String - fourth text to print
 */
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

/**
 * Display text on the LCD screen
 * 
 * @param text1      String to display on first line
 * @param text2      String to display on second line
 * @param delay_lcd  How long to show the text (in milliseconds)
 */
void lcd_show(String text1, String text2, int delay_lcd)
{
  lcd.setCursor(0, 0);        // Position cursor at first line
  lcd.print(text1);           // Print first line
  lcd.setCursor(1, 0);        // Position cursor at second line 
  lcd.print(text2);           // Print second line
  delay(delay_lcd);           // Wait specified time
}

/**
 * Read temperature from DHT sensor
 * 
 * @return float Temperature in Celsius, or 0 if reading failed
 */
float read_temp()
{
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature))
  {
    return 0;                 // Return 0 if reading failed
  }
  else
  {
    return event.temperature; // Return temperature in Celsius
  }
}

/**
 * Read soil moisture level from analog sensor
 * Takes average of 100 readings for stability
 * 
 * @return float Soil moisture percentage (0-100%)
 */
float read_soil()
{
  float sensorValue = 0;
  for (int i = 0; i <= 100; i++)
  {
    sensorValue = sensorValue + analogRead(soilPin);
    delay(1);
  }
  sensorValue = sensorValue / 100.0;  // Average readings
  float sensorValues = map(sensorValue, 0, 1023, 0, 100); // Map to percentage
  return sensorValue;  // Note: This should likely return sensorValues instead
}

/**
 * Setup fuzzy logic control system
 * Defines membership functions and rules for the fuzzy logic controller
 * 
 * Input variables:
 * 1. Temperature (cold, warm, hot)
 * 2. Soil moisture (dry, normal, wet)
 * 
 * Output variable:
 * 1. Pump speed (veryslow, slow, average, fast, veryfast)
 * 
 * The system uses 9 rules to determine optimal watering based on conditions
 */
void fuzzy_system()
{
  // Define temperature input and its membership functions
  FuzzyInput *temperature = new FuzzyInput(1);
  FuzzySet *cold = new FuzzySet(0, 11, 11, 22);    // Cold: 0-22°C (trapezoidal)
  temperature->addFuzzySet(cold);
  FuzzySet *warm = new FuzzySet(22, 24.5, 24.5, 27); // Warm: 22-27°C (trapezoidal)
  temperature->addFuzzySet(warm);
  FuzzySet *hot = new FuzzySet(27, 31, 31, 35);    // Hot: 27-35°C (trapezoidal)
  temperature->addFuzzySet(hot);
  fuzzy->addFuzzyInput(temperature);

  // Define soil moisture input and its membership functions
  FuzzyInput *soil = new FuzzyInput(2);
  FuzzySet *dry = new FuzzySet(0, 25, 25, 50);     // Dry: 0-50% (trapezoidal)
  soil->addFuzzySet(dry);
  FuzzySet *normal = new FuzzySet(40, 50, 50, 60); // Normal: 40-60% (trapezoidal)
  soil->addFuzzySet(normal);
  FuzzySet *wet = new FuzzySet(50, 75, 75, 100);   // Wet: 50-100% (trapezoidal)
  soil->addFuzzySet(wet);
  fuzzy->addFuzzyInput(soil);

  // Define pump output and its membership functions
  FuzzyOutput *pump = new FuzzyOutput(1);
  FuzzySet *veryslow = new FuzzySet(0, 0, 0, 25);      // Very slow: 0-25% (trapezoidal)
  pump->addFuzzySet(veryslow);
  FuzzySet *slow = new FuzzySet(0, 25, 25, 50);        // Slow: 0-50% (trapezoidal)
  pump->addFuzzySet(slow);
  FuzzySet *average = new FuzzySet(25, 50, 50, 75);    // Average: 25-75% (trapezoidal)
  pump->addFuzzySet(average);
  FuzzySet *fast = new FuzzySet(50, 75, 75, 100);      // Fast: 50-100% (trapezoidal)
  pump->addFuzzySet(fast);
  FuzzySet *veryfast = new FuzzySet(75, 100, 100, 100); // Very fast: 75-100% (trapezoidal)
  pump->addFuzzySet(veryfast);
  fuzzy->addFuzzyOutput(pump);

  // Rule 1: IF soil is DRY AND temperature is COLD THEN pump is AVERAGE
  FuzzyRuleAntecedent *ifSoilDryAndTemperatureCold = new FuzzyRuleAntecedent();
  ifSoilDryAndTemperatureCold->joinWithAND(dry, cold);
  FuzzyRuleConsequent *thenPumpAverage = new FuzzyRuleConsequent();
  thenPumpAverage->addOutput(average);
  FuzzyRule *fuzzyRule01 = new FuzzyRule(1, ifSoilDryAndTemperatureCold, thenPumpAverage);
  fuzzy->addFuzzyRule(fuzzyRule01);

  // Rule 2: IF soil is DRY AND temperature is WARM THEN pump is FAST
  FuzzyRuleAntecedent *ifSoilDryAndTemperatureWarm = new FuzzyRuleAntecedent();
  ifSoilDryAndTemperatureWarm->joinWithAND(dry, warm);
  FuzzyRuleConsequent *thenPumpFast = new FuzzyRuleConsequent();
  thenPumpFast->addOutput(fast);
  FuzzyRule *fuzzyRule02 = new FuzzyRule(2, ifSoilDryAndTemperatureWarm, thenPumpFast);
  fuzzy->addFuzzyRule(fuzzyRule02);

  // Rule 3: IF soil is DRY AND temperature is HOT THEN pump is VERY FAST
  FuzzyRuleAntecedent *ifSoilDryAndTemperatureHot = new FuzzyRuleAntecedent();
  ifSoilDryAndTemperatureHot->joinWithAND(dry, hot);
  FuzzyRuleConsequent *thenPumpVeryFast = new FuzzyRuleConsequent();
  thenPumpVeryFast->addOutput(veryfast);
  FuzzyRule *fuzzyRule03 = new FuzzyRule(3, ifSoilDryAndTemperatureHot, thenPumpVeryFast);
  fuzzy->addFuzzyRule(fuzzyRule03);

  // Rule 4: IF soil is NORMAL AND temperature is COLD THEN pump is SLOW
  FuzzyRuleAntecedent *ifSoilNormalAndTemperatureCold = new FuzzyRuleAntecedent();
  ifSoilNormalAndTemperatureCold->joinWithAND(normal, cold);
  FuzzyRuleConsequent *thenPumpSlow = new FuzzyRuleConsequent();
  thenPumpSlow->addOutput(slow);
  FuzzyRule *fuzzyRule04 = new FuzzyRule(4, ifSoilNormalAndTemperatureCold, thenPumpSlow);
  fuzzy->addFuzzyRule(fuzzyRule04);

  // Rule 5: IF soil is NORMAL AND temperature is WARM THEN pump is AVERAGE
  FuzzyRuleAntecedent *ifSoilNormalAndTemperatureWarm = new FuzzyRuleAntecedent();
  ifSoilNormalAndTemperatureWarm->joinWithAND(normal, warm);
  FuzzyRule *fuzzyRule05 = new FuzzyRule(5, ifSoilNormalAndTemperatureWarm, thenPumpAverage);
  fuzzy->addFuzzyRule(fuzzyRule05);

  // Rule 6: IF soil is NORMAL AND temperature is HOT THEN pump is FAST
  FuzzyRuleAntecedent *ifSoilNormalAndTemperatureHot = new FuzzyRuleAntecedent();
  ifSoilNormalAndTemperatureHot->joinWithAND(normal, hot);
  FuzzyRule *fuzzyRule06 = new FuzzyRule(6, ifSoilNormalAndTemperatureHot, thenPumpFast);
  fuzzy->addFuzzyRule(fuzzyRule06);

  // Rule 7: IF soil is WET AND temperature is COLD THEN pump is VERY SLOW
  FuzzyRuleAntecedent *ifSoilWetAndTemperatureCold = new FuzzyRuleAntecedent();
  ifSoilWetAndTemperatureCold->joinWithAND(wet, cold);
  FuzzyRuleConsequent *thenPumpVerySlow = new FuzzyRuleConsequent();
  thenPumpVerySlow->addOutput(veryslow);
  FuzzyRule *fuzzyRule07 = new FuzzyRule(7, ifSoilWetAndTemperatureCold, thenPumpVerySlow);
  fuzzy->addFuzzyRule(fuzzyRule07);

  // Rule 8: IF soil is WET AND temperature is WARM THEN pump is SLOW
  FuzzyRuleAntecedent *ifSoilWetAndTemperatureWarm = new FuzzyRuleAntecedent();
  ifSoilWetAndTemperatureWarm->joinWithAND(wet, warm);
  FuzzyRule *fuzzyRule08 = new FuzzyRule(8, ifSoilWetAndTemperatureWarm, thenPumpSlow);
  fuzzy->addFuzzyRule(fuzzyRule08);

  // Rule 9: IF soil is WET AND temperature is HOT THEN pump is AVERAGE
  FuzzyRuleAntecedent *ifSoilWetAndTemperatureHot = new FuzzyRuleAntecedent();
  ifSoilWetAndTemperatureHot->joinWithAND(wet, hot);
  FuzzyRule *fuzzyRule09 = new FuzzyRule(9, ifSoilWetAndTemperatureHot, thenPumpAverage);
  fuzzy->addFuzzyRule(fuzzyRule09);
}