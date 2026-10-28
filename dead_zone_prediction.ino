// ============================================================
//  Dead Zone Prediction System (Without DS18B20)
//  Components: ESP32, pH Sensor, Turbidity Sensor, TDS Sensor,
//              LCD 16x2 I2C, Buzzer, Red LED, Green LED
// ============================================================

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ---------- PIN DEFINITIONS ----------
#define PH_PIN        34    // Analog pH sensor signal
#define TURB_PIN      35    // Turbidity sensor signal
#define TDS_PIN       32    // TDS / Salinity sensor signal
#define BUZZER_PIN    26    // Active buzzer
#define RED_LED_PIN   27    // Red LED (danger)
#define GREEN_LED_PIN 25    // Green LED (safe)

// ---------- LCD SETUP ----------
// I2C address is usually 0x27 or 0x3F — try 0x3F if display is blank
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ---------- CALIBRATION CONSTANTS ----------
// pH Sensor: adjust these based on your calibration buffer solutions
const float PH_VOLTAGE_OFFSET = 0.0;   // Fine-tune after calibration
const float PH_SLOPE          = -5.70; // Typical value for analog pH modules

// TDS: Gravity TDS module uses a known conversion factor
const float TDS_FACTOR        = 0.5;   // ppm = (voltage / 3.3) * 1000 * TDS_FACTOR * 1000

// Turbidity: TSW-10 output is inversely related to clarity
// At 5V supply: ~4.2V = clear water (0 NTU), lower V = turbid
const float TURB_CLEAR_VOLTAGE = 4.2;

// ---------- THRESHOLDS ----------
const float DO_HYPOXIA_THRESHOLD = 2.0;   // mg/L — dead zone begins below this
const float PH_LOW               = 6.5;
const float PH_HIGH              = 8.5;
const float TURB_WARN_NTU        = 50.0;
const float TDS_WARN_PPM         = 600.0;

// ---------- ASSUMED TEMPERATURE ----------
// Since DS18B20 is not available, use a fixed ambient water temperature.
// Update this value based on your region / season.
const float ASSUMED_TEMP_C = 28.0;  // degrees Celsius

// ---------- ADC AVERAGING ----------
const int NUM_SAMPLES = 30;  // Number of ADC samples to average (reduces noise)

// ============================================================
//  HELPER: Read averaged ADC voltage (0–3.3V on ESP32)
// ============================================================
float readVoltage(int pin) {
  long sum = 0;
  for (int i = 0; i < NUM_SAMPLES; i++) {
    sum += analogRead(pin);
    delay(2);
  }
  float avgADC = sum / (float)NUM_SAMPLES;
  return (avgADC / 4095.0) * 3.3;  // ESP32 ADC is 12-bit, 3.3V reference
}

// ============================================================
//  SENSOR READING: pH
//  Typical analog pH module: 0V=pH14, 3.3V=pH0 (inverted)
// ============================================================
float readPH() {
  float voltage = readVoltage(PH_PIN);
  // Standard formula for analog pH modules (calibrate with buffer solutions)
  float ph = 7.0 + ((2.5 - voltage) / PH_SLOPE) * (-1.0);
  // Clamp to realistic range
  ph = constrain(ph, 0.0, 14.0);
  return ph;
}

// ============================================================
//  SENSOR READING: Turbidity (NTU)
//  TSW-10: Higher voltage = clearer water
// ============================================================
float readTurbidity() {
  float voltage = readVoltage(TURB_PIN);
  // Approximate conversion: NTU increases as voltage drops from ~4.2V
  // Scaled to 0–200 NTU range for typical water quality use
  float ntu = 0.0;
  if (voltage >= TURB_CLEAR_VOLTAGE) {
    ntu = 0.0;
  } else {
    ntu = (TURB_CLEAR_VOLTAGE - voltage) * (200.0 / TURB_CLEAR_VOLTAGE);
  }
  return constrain(ntu, 0.0, 200.0);
}

// ============================================================
//  SENSOR READING: TDS / Salinity (ppm)
//  Gravity TDS sensor — 3.3V powered
// ============================================================
float readTDS() {
  float voltage = readVoltage(TDS_PIN);
  // Temperature compensation using assumed temp
  float compensationCoeff = 1.0 + 0.02 * (ASSUMED_TEMP_C - 25.0);
  float compensatedVoltage = voltage / compensationCoeff;
  // Gravity TDS formula (from DFRobot documentation)
  float tds = (133.42 * pow(compensatedVoltage, 3)
             - 255.86 * pow(compensatedVoltage, 2)
             + 857.39 * compensatedVoltage) * TDS_FACTOR;
  return constrain(tds, 0.0, 2000.0);
}

// ============================================================
//  ESTIMATION: DO Saturation from Temperature
//  Benson & Krause equation (empirical)
// ============================================================
float doSaturation(float tempC) {
  return 14.62
       - 0.3898  * tempC
       + 0.006969 * tempC * tempC
       - 0.00005896 * tempC * tempC * tempC;
}

// ============================================================
//  ESTIMATION: Dissolved Oxygen (mg/L)
//  Proxy model using pH, Turbidity, TDS, and Temperature
// ============================================================
float estimateDO(float tempC, float ph, float turbNTU, float tdsPPM) {
  float sat = doSaturation(tempC);

  // Turbidity reduces DO: algae/particles block photosynthesis
  float turbPenalty = min(turbNTU * 0.015f, 4.0f);

  // Excess TDS reduces oxygen solubility
  float tdsPenalty = max(0.0f, (tdsPPM - 500.0f) * 0.001f);

  // Extreme pH stresses oxygen dynamics
  float phPenalty = 0.0;
  if (ph < PH_LOW)  phPenalty = (PH_LOW  - ph)  * 0.4;
  if (ph > PH_HIGH) phPenalty = (ph - PH_HIGH)   * 0.3;

  // High temperature reduces DO beyond saturation loss
  float tempBoost = (tempC > 30.0) ? (tempC - 30.0) * 0.08 : 0.0;

  float estimatedDO = sat - turbPenalty - tdsPenalty - phPenalty - tempBoost;
  return constrain(estimatedDO, 0.0, sat);
}

// ============================================================
//  RISK LEVEL as string
// ============================================================
String getRiskLevel(float doVal) {
  if (doVal < 1.0) return "CRITICAL";
  if (doVal < 2.0) return "HIGH RISK";
  if (doVal < 4.0) return "MODERATE";
  if (doVal < 6.0) return "LOW RISK";
  return "HEALTHY";
}

// ============================================================
//  ALERT: Buzzer and LEDs
// ============================================================
void triggerAlert(float doVal) {
  if (doVal < DO_HYPOXIA_THRESHOLD) {
    // Dead zone detected — RED LED on, GREEN off, Buzzer beeps
    digitalWrite(RED_LED_PIN,   HIGH);
    digitalWrite(GREEN_LED_PIN, LOW);
    // 3 short beeps
    for (int i = 0; i < 3; i++) {
      digitalWrite(BUZZER_PIN, HIGH);
      delay(200);
      digitalWrite(BUZZER_PIN, LOW);
      delay(200);
    }
  } else {
    // Safe — GREEN LED on, RED off, Buzzer silent
    digitalWrite(GREEN_LED_PIN, HIGH);
    digitalWrite(RED_LED_PIN,   LOW);
    digitalWrite(BUZZER_PIN,    LOW);
  }
}

// ============================================================
//  SERIAL DEBUG: Print all readings
// ============================================================
void printSerial(float ph, float turbNTU, float tdsPPM, float doEst) {
  Serial.println("==============================");
  Serial.printf("Temp (assumed): %.1f C\n",   ASSUMED_TEMP_C);
  Serial.printf("pH:             %.2f\n",      ph);
  Serial.printf("Turbidity:      %.1f NTU\n",  turbNTU);
  Serial.printf("TDS/Salinity:   %.1f ppm\n",  tdsPPM);
  Serial.printf("Est. DO:        %.2f mg/L\n", doEst);
  Serial.printf("Risk Level:     %s\n",        getRiskLevel(doEst).c_str());
  Serial.println("==============================");
}

// ============================================================
//  SETUP
// ============================================================
void setup() {
  Serial.begin(115200);

  // Pin modes
  pinMode(BUZZER_PIN,    OUTPUT);
  pinMode(RED_LED_PIN,   OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);

  // All off initially
  digitalWrite(BUZZER_PIN,    LOW);
  digitalWrite(RED_LED_PIN,   LOW);
  digitalWrite(GREEN_LED_PIN, LOW);

  // LCD init
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Dead Zone Pred.");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(2000);
  lcd.clear();

  Serial.println("Dead Zone Prediction System Ready");
  Serial.printf("Using fixed temperature: %.1f C\n", ASSUMED_TEMP_C);
}

// ============================================================
//  MAIN LOOP
// ============================================================
void loop() {
  // --- Read all sensors ---
  float ph      = readPH();
  float turbNTU = readTurbidity();
  float tdsPPM  = readTDS();

  // --- Estimate dissolved oxygen ---
  float doEst = estimateDO(ASSUMED_TEMP_C, ph, turbNTU, tdsPPM);
  String risk = getRiskLevel(doEst);

  // --- Print to Serial Monitor ---
  printSerial(ph, turbNTU, tdsPPM, doEst);

  // --- Display on LCD (rotates between two screens) ---
  // Screen 1: pH and Turbidity
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("pH:");
  lcd.print(ph, 2);
  lcd.setCursor(8, 0);
  lcd.print("T:");
  lcd.print((int)turbNTU);
  lcd.print("NTU");
  lcd.setCursor(0, 1);
  lcd.print("TDS:");
  lcd.print((int)tdsPPM);
  lcd.print("ppm");
  delay(3000);

  // Screen 2: Estimated DO and risk
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Est.DO:");
  lcd.print(doEst, 2);
  lcd.print("mg/L");
  lcd.setCursor(0, 1);
  lcd.print("Risk: ");
  lcd.print(risk);
  delay(3000);

  // --- Trigger buzzer/LED alerts ---
  triggerAlert(doEst);

  // --- Wait before next reading ---
  delay(2000);
}
