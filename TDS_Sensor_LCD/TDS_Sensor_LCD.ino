#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Use the I2C address found from the scanner
LiquidCrystal_I2C lcd(0x3F, 16, 2);

const int TDS_PIN = A0;

float calculateTDS(int analogValue) {
  float voltage = analogValue * (3.3 / 1024.0); // Convert analog value to voltage
  float tds = (133.42 * voltage * voltage * voltage - 255.86 * voltage * voltage + 857.39 * voltage) * 0.5; // Example calibration
  return tds;
}

void setup() {
  // Initialize the LCD
  lcd.begin();
  lcd.backlight();
  lcd.print("TDS Sensor");

  // Initialize serial communication
  Serial.begin(115200);
}

void loop() {
  // Read the analog value from the TDS sensor
  int sensorValue = analogRead(TDS_PIN);

  // Calculate the TDS value
  float tdsValue = calculateTDS(sensorValue);

  // Print TDS value to the LCD
  lcd.setCursor(0, 1);
  lcd.print("TDS: ");
  lcd.print(tdsValue);
  lcd.print(" ppm");

  // Print TDS value to the Serial Monitor
  Serial.print("TDS: ");
  Serial.print(tdsValue);
  Serial.println(" ppm");

  // Delay before the next reading
  delay(3000);
  lcd.clear();
}
