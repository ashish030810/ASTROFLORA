/*
 * =========================================================================
 * 🌿 ASTRO-FLORA: MULTI-ZONE SPACE BOTANY & CREW ACCESS MONITORING OS
 * =========================================================================
 * Lead Systems Architect: [ASHISH ]
 * Target Hardware Platform: Arduino Mega 2560 / Standard ATmega Controllers
 * Operational Domain: Automated Martian Greenhouse & Habitat Security Matrix
 * * This highly expanded, industrial-grade software framework handles:
 * 1. Dual-Zone Ground Saturation Sensor Reading and Autonomous Irrigation.
 * 2. Active Climate Stability Regulation (Heating, Venting, Circulation).
 * 3. Personalized Crew Authentication Tracking (Dynamic Entry & Exit Logs).
 * 4. Real-Time Hardware UI Terminal Updates and Telemetry Stream Processing.
 * =========================================================================
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Initialize LCD Display Terminal Panel (I2C Target Address: 0x27)
// Grid layout configuration variables: 16 Columns by 2 Display Rows
LiquidCrystal_I2C lcd(0x27, 16, 2);

// =========================================================================
// 🛠️ HARDWARE I/O REGISTER PIN ASSIGNMENT MATRIX
// =========================================================================
// Analog Transducer Input Infrastructure Nodes
const int PIN_SENSOR_MOISTURE_ZONE_A = A0;  // Volumetric Hydration Sensor - Zone A
const int PIN_SENSOR_MOISTURE_ZONE_B = A1;  // Volumetric Hydration Sensor - Zone B
const int PIN_SENSOR_THERMISTOR_AIR  = A4;  // Atmospheric Thermal Sensor Node
const int PIN_SENSOR_FLUID_RESERVOIR = A5;  // Main Feedwater Storage Level Unit

// Digital Input Interlock Devices
const int PIN_BUTTON_SECURITY_SCAN   = 10;  // Simulated Biometric Crew Access Scanner

// Digital Relay Actuator Output Controllers
const int PIN_ACTUATOR_PUMP_A       = 2;   // Solenoid Water Valve Line - Zone A
const int PIN_ACTUATOR_PUMP_B       = 3;   // Solenoid Water Valve Line - Zone B
const int PIN_RELAY_SOLAR_LIGHTS    = 4;   // High-Intensity Solar Grow Light Array
const int PIN_RELAY_THERMAL_HEATER  = 5;   // Climate Control Radiant Heating Coil
const int PIN_RELAY_EXHAUST_FAN    = 6;   // Carbon Dioxide Gas Circulation Fan
const int PIN_AUDIO_ALARM_BUZZER    = 7;   // Master Acoustic Warning Transduzer

// =========================================================================
// ⚙️ SYSTEM SETPOINTS AND CRITICAL OPERATIONAL PARAMETERS
// =========================================================================
const int SETPOINT_DRY_THRESHOLD       = 450;  // Moisture floor trigger limit
const int SETPOINT_MIN_RESERVOIR_LEVEL = 200;  // Security floor limit for water pump
const int SETPOINT_CRITICAL_LOW_TEMP   = 16;   // Minimum Celsius before heating loop
const int SETPOINT_CRITICAL_HIGH_TEMP  = 30;   // Maximum Celsius before exhaust venting

// Non-Blocking Execution Delay Intervals
const unsigned long DELAY_TELEMETRY_REFRESH = 4000;  // Metric streaming clock speed
const unsigned long DELAY_SOLAR_CYCLE_SHIFT = 20000; // Simulated day shift duration
const unsigned long RUNTIME_PUMP_IRRIGATION = 2500;  // Pump running burst time span

// =========================================================================
// 📊 CENTRAL VOLATILE STORAGE & CREW ACCESS DATABASE REGISTER
// =========================================================================
unsigned long timeTrackerLastTelemetry    = 0;
unsigned long timeTrackerLastSolarShift   = 0;
unsigned long timeTrackerCurrentExecution  = 0;

bool statusFlagDaytimeCycleActive = true;
bool statusFlagAcousticMuteActive = false;
int trackingCounterCycleSequence   = 0;

// Security Registry State Variables
bool securityScannerButtonLastState = HIGH;
int activeCrewDatabasePointer       = 0;
const int TOTAL_AUTHORIZED_CREW     = 4;

// Simulated Astropass Registry Memory Array
String crewNameRegistry[TOTAL_AUTHORIZED_CREW] = {
  "COMMANDER ALY", 
  "ENGINEER LEO", 
  "PILOT ZARA", 
  "ADMIN BRO"
};
bool crewInsideStatus[TOTAL_AUTHORIZED_CREW] = {false, false, false, false};

// Custom Hexadecimal LCD Character Matrices 
uint8_t graphicIconSprout[8]  = { 0b00100, 0b00110, 0b01100, 0b00100, 0b00100, 0b01110, 0b11111, 0b00000 };
uint8_t graphicIconSun[8]     = { 0b00100, 0b10101, 0b01110, 0b11011, 0b01110, 0b10101, 0b00100, 0b00000 };
uint8_t graphicIconMoon[8]    = { 0b00110, 0b01100, 0b11000, 0b11000, 0b11000, 0b01100, 0b00110, 0b00000 };
uint8_t graphicIconWarning[8] = { 0b00000, 0b00100, 0b01010, 0b01010, 0b11111, 0b11111, 0b00000, 0b00000 };

// =========================================================================
// 📌 SYSTEM HARDWARE SETUP INITIALIZATION SEQUENCE
// =========================================================================
void setup() {
  // Ignite serial communications channel for logging terminal
  Serial.begin(9600);
  
  // Power up and flash the liquid crystal matrix panel
  lcd.init();
  lcd.backlight();
  
  // Inject specialized pixel icon vectors into CGRAM chip storage
  lcd.createChar(0, graphicIconSprout);
  lcd.createChar(1, graphicIconSun);
  lcd.createChar(2, graphicIconMoon);
  lcd.createChar(3, graphicIconWarning);
  
  // Register pin operational modes across hardware map
  pinMode(PIN_BUTTON_SECURITY_SCAN, INPUT_PULLUP);
  pinMode(PIN_ACTUATOR_PUMP_A, OUTPUT);
  pinMode(PIN_ACTUATOR_PUMP_B, OUTPUT);
  pinMode(PIN_RELAY_SOLAR_LIGHTS, OUTPUT);
  pinMode(PIN_RELAY_THERMAL_HEATER, OUTPUT);
  pinMode(PIN_RELAY_EXHAUST_FAN, OUTPUT);
  pinMode(PIN_AUDIO_ALARM_BUZZER, OUTPUT);
  
  // Secure initial state registers for output channels
  digitalWrite(PIN_RELAY_SOLAR_LIGHTS, HIGH);
  digitalWrite(PIN_RELAY_THERMAL_HEATER, LOW);
  digitalWrite(PIN_RELAY_EXHAUST_FAN, LOW);
  
  executeAcousticPulse(880, 250);
  
  // Present initial operational dashboard banner layout
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ASTRO-FLORA OS");
  lcd.setCursor(0, 1);
  lcd.print("SECURITY CONTROL");
  
  Serial.println(F("================================================================="));
  Serial.println(F("[SYSTEM-ONLINE] Astro-Flora OS Multi-Zone Station Interface."));
  Serial.println(F("[SECURITY] Astropass Gateway Access Protocols Loaded."));
  Serial.println(F("================================================================="));
  
  delay(3000); // Maintain title splash view for three operating seconds
  
  unsigned long baselineClockSetup = millis();
  timeTrackerLastTelemetry = baselineClockSetup;
  timeTrackerLastSolarShift = baselineClockSetup;
}

// =========================================================================
// 🔄 CORE CONTINUOUS CONTROL LOOP ENGINE
// =========================================================================
void loop() {
  timeTrackerCurrentExecution = millis();
  
  // 1. SCAN AND REGISTER HARDWARE TRANSDUCER METRICS
  int rawValueMoistureZoneA  = analogRead(PIN_SENSOR_MOISTURE_ZONE_A);
  int rawValueMoistureZoneB  = analogRead(PIN_SENSOR_MOISTURE_ZONE_B);
  int rawValueAtmosphereTemp = analogRead(PIN_SENSOR_THERMISTOR_AIR);
  int rawValueWaterReservoir = analogRead(PIN_SENSOR_FLUID_RESERVOIR);
  
  // Linear scale normalization conversion passes
  int computedPercentMoistureA = map(rawValueMoistureZoneA, 0, 1023, 0, 100);
  int computedPercentMoistureB = map(rawValueMoistureZoneB, 0, 1023, 0, 100);
  int computedCelsiusDegree     = map(rawValueAtmosphereTemp, 0, 1023, -10, 50);

  // 2. RUN ASTROPASS CREW ACCESS GATEWAY AUDITOR
  bool securityScannerButtonCurrentState = digitalRead(PIN_BUTTON_SECURITY_SCAN);
  
  // Detect falling-edge button toggle event (simulating a badge swipe)
  if (securityScannerButtonCurrentState == LOW && securityScannerButtonLastState == HIGH) {
    executeCrewAccessTransaction();
    delay(200); // Hardware switch debounce protection window
  }
  securityScannerButtonLastState = securityScannerButtonCurrentState;
  
  // 3. EXECUTE SOLAR ROTATION TIMING GRID SWEEP
  if (timeTrackerCurrentExecution - timeTrackerLastSolarShift >= DELAY_SOLAR_CYCLE_SHIFT) {
    statusFlagDaytimeCycleActive = !statusFlagDaytimeCycleActive;
    timeTrackerLastSolarShift = timeTrackerCurrentExecution;
    digitalWrite(PIN_RELAY_SOLAR_LIGHTS, statusFlagDaytimeCycleActive ? HIGH : LOW);
    
    Serial.print(F("[GRID-CYCLE] Environmental shift. Greenhouse Solar Array: "));
    Serial.println(statusFlagDaytimeCycleActive ? F("DAYTIME_GRID") : F("NIGHTTIME_GRID"));
  }
  
  // 4. MICROCLIMATE HEATING AND EXPULSION ROUTINES
  if (computedCelsiusDegree < SETPOINT_CRITICAL_LOW_TEMP) {
    digitalWrite(PIN_RELAY_THERMAL_HEATER, HIGH);
    digitalWrite(PIN_RELAY_EXHAUST_FAN, LOW);
  } else if (computedCelsiusDegree > SETPOINT_CRITICAL_HIGH_TEMP) {
    digitalWrite(PIN_RELAY_THERMAL_HEATER, LOW);
    digitalWrite(PIN_RELAY_EXHAUST_FAN, HIGH);
  } else {
    digitalWrite(PIN_RELAY_THERMAL_HEATER, LOW);
    digitalWrite(PIN_RELAY_EXHAUST_FAN, LOW);
  }
  
  // 5. PACKET LOGGING TELEMETRY BROADCAST OVER SERIAL LINK
  if (timeTrackerCurrentExecution - timeTrackerLastTelemetry >= DELAY_TELEMETRY_REFRESH) {
    trackingCounterCycleSequence++;
    timeTrackerLastTelemetry = timeTrackerCurrentExecution;
    
    Serial.println(F("\n🛰️ --- BIOMETRIC & BIOSPHERE DATASTREAM RECORD ---"));
    Serial.print(F("Uptime Clock Counter: ")); Serial.print(timeTrackerCurrentExecution / 1000); Serial.println(F("s"));
    Serial.print(F("Zone A Soil Matrix: ")); Serial.print(computedPercentMoistureA); Serial.println(F("%"));
    Serial.print(F("Zone B Soil Matrix: ")); Serial.print(computedPercentMoistureB); Serial.println(F("%"));
    Serial.print(F("Atmosphere Module Heat: ")); Serial.print(computedCelsiusDegree); Serial.println(F(" C"));
    Serial.print(F("Habitat Crew Manifest: "));
    for(int idx = 0; idx < TOTAL_AUTHORIZED_CREW; idx++) {
      Serial.print(crewNameRegistry[idx]);
      Serial.print(crewInsideStatus[idx] ? F(" [INSIDE] | ") : F(" [OUTSIDE] | "));
    }
    Serial.println();
    Serial.println(F("--------------------------------------------------"));
  }
  
  // 6. PROCESS REFRESH DATA CYCLES FOR HARDWARE USER INTERFACE SCREEN
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write(0); // Call sprout glyph
  lcd.print("A:"); lcd.print(computedPercentMoistureA);
  lcd.print("% B:"); lcd.print(computedPercentMoistureB); lcd.print("%");
  
  lcd.setCursor(0, 1);
  if (statusFlagDaytimeCycleActive) {
    lcd.write(1); // Call sun glyph
    lcd.print(" DAY ");
  } else {
    lcd.write(2); // Call moon glyph
    lcd.print(" NIGHT");
  }
  lcd.print(" T:"); lcd.print(computedCelsiusDegree); lcd.print("C");
  
  // 7. FEEDWATER SAFETY SYSTEM ANALYSIS CHECKS
  if (rawValueWaterReservoir < SETPOINT_MIN_RESERVOIR_LEVEL) {
    executeEmergencyReservoirCutoff();
  } else {
    if (rawValueMoistureZoneA < SETPOINT_DRY_THRESHOLD) {
      triggerPumpActivation(PIN_ACTUATOR_PUMP_A, "ZONE_A");
    }
    if (rawValueMoistureZoneB < SETPOINT_DRY_THRESHOLD) {
      triggerPumpActivation(PIN_ACTUATOR_PUMP_B, "ZONE_B");
    }
  }
  
  delay(300); // Standard loop core cycle throttling governor
}

// =========================================================================
// 📡 SECURITY INTERFACE: PROCESS ENTRY/EXIT TRACKING CYCLE
// =========================================================================
void executeCrewAccessTransaction() {
  // Reverse the inside/outside tracking status boolean state for target person
  crewInsideStatus[activeCrewDatabasePointer] = !crewInsideStatus[activeCrewDatabasePointer];
  
  String activePersonnel = crewNameRegistry[activeCrewDatabasePointer];
  bool isInside = crewInsideStatus[activeCrewDatabasePointer];
  
  // Broadcast security event signature out to system logs
  Serial.println(F("\n🔒 [ACCESS SECURITY] Scanning Badge Identity..."));
  Serial.print(F(" -> Personnel Authenticated: ")); Serial.println(activePersonnel);
  Serial.print(F(" -> Transaction Event Route: "));
  Serial.println(isInside ? F("HABITAT_ENTRY_REGISTERED") : F("HABITAT_EXIT_REGISTERED"));
  
  // Intercept layout controls to splash access notification banner
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(activePersonnel);
  lcd.setCursor(0, 1);
  if (isInside) {
    lcd.print("-> ACCESS: ENTRY");
    executeAcousticPulse(523, 150); // High melody beep for entry success
    executeAcousticPulse(659, 150);
  } else {
    lcd.print("-> ACCESS: EXIT ");
    executeAcousticPulse(659, 150); // Cascading beep for exit confirmation
    executeAcousticPulse(523, 150);
  }
  
  delay(2500); // Lock screen state view to show security report confirmation
  
  // Rotate pointer index values to step to the next crew member card file for the next swipe test
  activeCrewDatabasePointer++;
  if (activeCrewDatabasePointer >= TOTAL_AUTHORIZED_CREW) {
    activeCrewDatabasePointer = 0;
  }
}

// =========================================================================
// 💧 IRRIGATOR CONTROLLER SUBROUTINE: DISPENSE HYDRO-FLOWS
// =========================================================================
void triggerPumpActivation(int designatedPumpHardwarePin, String stringZoneLabel) {
  Serial.print(F("[AUTOMATION] Desiccation flag raised for tracking reference: "));
  Serial.println(stringZoneLabel);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write(3); // Display warning symbol
  lcd.print(" DISPENSING H2O");
  lcd.setCursor(0, 1);
  lcd.print("VALVE: "); lcd.print(stringZoneLabel);
  
  executeAcousticPulse(600, 100);
  delay(30);
  executeAcousticPulse(700, 100);
  
  digitalWrite(designatedPumpHardwarePin, HIGH);
  delay(RUNTIME_PUMP_IRRIGATION);
  digitalWrite(designatedPumpHardwarePin, LOW);
  
  Serial.print(F("[COMPLETED] Fluid sequence finalized for reference node: "));
  Serial.println(stringZoneLabel);
}

// =========================================================================
// 🚨 SAFETY CORE ROUTINE: EMERGENCY FEEDWATER SHUTDOWN
// =========================================================================
void executeEmergencyReservoirCutoff() {
  Serial.println(F("[CRITICAL ERROR] Reservoir depletion event detected! Pumping vectors offline."));
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write(3); // Warning icon
  lcd.print(" SYS-ALARM: DRY ");
  lcd.setCursor(0, 1);
  lcd.print("FEEDWATER FAULT ");
  
  if (!statusFlagAcousticMuteActive) {
    for (int ringIndex = 0; ringIndex < 2; ringIndex++) {
      tone(PIN_AUDIO_ALARM_BUZZER, 440, 200);
      delay(200);
      tone(PIN_AUDIO_ALARM_BUZZER, 220, 200);
      delay(200);
    }
  } else {
    delay(800);
  }
}

// =========================================================================
// 🔊 UTILITY FUNCTION: AUDIO HARDWARE CHANNEL DRIVER
// =========================================================================
void executeAcousticPulse(int parameterFrequencyHz, int parameterDurationMs) {
  if (!statusFlagAcousticMuteActive) {
    tone(PIN_AUDIO_ALARM_BUZZER, parameterFrequencyHz, parameterDurationMs);
    delay(parameterDurationMs);
  }
}
