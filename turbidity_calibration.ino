// ============================================================
//  Turbidity Sensor Calibration Helper
//  Upload this, open Serial Monitor at 115200 baud
//  Step 1: Put sensor in CLEAN water → note the voltage (this is your CLEAR_VOLTAGE)
//  Step 2: Put sensor in TURBID water → note the voltage
//  Step 3: Update TURB_CLEAR_VOLTAGE in dead_zone_prediction.ino
// ============================================================

#define TURB_PIN 35
const int NUM_SAMPLES = 30;

float readVoltage(int pin) {
  long sum = 0;
  for (int i = 0; i < NUM_SAMPLES; i++) {
    sum += analogRead(pin);
    delay(2);
  }
  float avgADC = sum / (float)NUM_SAMPLES;
  return (avgADC / 4095.0) * 3.3;
}

void setup() {
  Serial.begin(115200);
  Serial.println("=== Turbidity Calibration ===");
  Serial.println("Place sensor in CLEAN water first.");
  Serial.println("Reading every 2 seconds...");
}

void loop() {
  float voltage = readVoltage(TURB_PIN);

  // Raw NTU using current assumed clear voltage of 4.2V
  float ntu_42  = max(0.0f, (4.2f  - voltage) * (200.0f / 4.2f));

  Serial.println("------------------------------");
  Serial.printf("Raw Voltage:       %.4f V\n", voltage);
  Serial.printf("NTU (if clear=4.2V): %.1f NTU\n", ntu_42);
  Serial.println();
  Serial.println(">>> Copy the Raw Voltage when sensor is in CLEAN water");
  Serial.println(">>> That value = your TURB_CLEAR_VOLTAGE");
  Serial.println("------------------------------");

  delay(2000);
}
