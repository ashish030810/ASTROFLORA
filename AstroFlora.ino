/*
 * 🌿 ASTRO-FLORA: Automated Martian Greenhouse System
 * Target Platform: Arduino Architecture (Uno / Mega / Nano)
 * Tracks soil moisture, automates irrigation, and manages crop status.
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Initialize LCD (Address 0x27, 16 columns, 2 rows)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// --- PIN CONFIGURATION ---
const int MOISTURE_SENSOR  = 0;  // Analog Sensor Input (or Potentiometer)
const int WATER_PUMP_RELAY = 5;   // Controls irrigation pump / Blue LED
const int GROW_LIGHT       = 4;   // Simulates solar grow lights
const int CRITICAL_BUZZER  = 7;   // Acoustic warning system

// --- SYSTEM THRESHOLDS ---
const int DRY_THRESHOLD = 400;    // Below this value means soil needs water
const int PUMP_DELAY_MS = 3000;   // How long to run the pump (3 seconds)

// Custom Sprout Graphic Symbol
uint8_t customSprout[8] = {
  0b00100, 0b00110, 0b01100, 0b00100, 0b00100, 0b01110, 0b11111, 0b00000
};

void setup() {
  Serial.begin(9600);
  
  // Initialize Hardware
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, customSprout);
  
  pinMode(WATER_PUMP_RELAY, OUTPUT);
  pinMode(GROW_LIGHT, OUTPUT);
  pinMode(CRITICAL_BUZZER, OUTPUT);
  
  // Turn on grow lights on boot
  digitalWrite(GROW_LIGHT, HIGH); 
  
  Serial.println(F("[SYSTEM-BOOT] Astro-Flora Life Support Active."));
}

void loop() {
  int rawMoisture = analogRead(MOISTURE_SENSOR);
  int moisturePercent = map(rawMoisture, 0, 1023, 0, 100);
  
  // Print telemetry to Serial Monitor for Hackatime logs
  Serial.print(F("[TELEMETRY] Soil Moisture Level: "));
  Serial.print(moisturePercent);
  Serial.println(F("%"));
  
  // Update LCD Interface
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write(0); // Print custom sprout symbol
  lcd.print(" ASTRO-FLORA OS");
  
  lcd.setCursor(0, 1);
  lcd.print("MOISTURE: ");
  lcd.print(moisturePercent);
  lcd.print("%");
  
  // Evaluate Crop Hydration Status
  if (rawMoisture < DRY_THRESHOLD) {
    triggerIrrigationSequence();
  }
  
  delay(2000); 
}

void triggerIrrigationSequence() {
  Serial.println(F("[ALERT] Soil moisture critical! Emergency watering active."));
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("⚠️ MOISTURE LOW!");
  lcd.setCursor(0, 1);
  lcd.print("PUMP ACTIVATED...");
  
  // Warning sound
  tone(CRITICAL_BUZZER, 880, 250);
  delay(300);
  tone(CRITICAL_BUZZER, 880, 250);
  
  // Activate pump relay
  digitalWrite(WATER_PUMP_RELAY, HIGH);
  delay(PUMP_DELAY_MS); 
  digitalWrite(WATER_PUMP_RELAY, LOW);
  
  Serial.println(F("[SUCCESS] Irrigation cycle complete."));
}s