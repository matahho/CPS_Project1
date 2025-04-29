#include <Wire.h>

// Slave Addresses
#define SLAVE_ADDR_RIGHT 2
#define SLAVE_ADDR_LEFT  3

// Sensor Pins
const int TEMP_PIN = A0;
const int LDR_LEFT_PIN = A1;
const int LDR_RIGHT_PIN = A2;

// Sensor Readings
int moistureRight, moistureLeft;
int temperature;
int lightRight, lightLeft;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  Serial.println("Master Setup Complete");
}

void loop() {
  readTemperature();
  delay(300); // Increased delay

  readLightSensors();
  delay(300); // Increased delay

  moistureRight = readMoistureFromSlave(SLAVE_ADDR_RIGHT, "Right");
  delay(300); // Increased delay

  moistureLeft  = readMoistureFromSlave(SLAVE_ADDR_LEFT,  "Left");
  delay(300); // Increased delay

  sendCommandToSlave(SLAVE_ADDR_RIGHT, moistureRight);
  delay(300); // Increased delay

  sendCommandToSlave(SLAVE_ADDR_LEFT,  moistureLeft);
  delay(1000); // Final delay before next cycle
}

// ------------------------- SENSOR READS -------------------------

void readTemperature() {
  int rawValue = analogRead(TEMP_PIN);
  temperature = map(rawValue, 0, 205, 0, 100);
  Serial.print("Temperature: ");
  Serial.println(temperature);
}

void readLightSensors() {
  lightLeft = analogRead(LDR_LEFT_PIN);
  lightRight = analogRead(LDR_RIGHT_PIN);
  Serial.print("Light (L: ");
  Serial.print(lightLeft);
  Serial.print(" - R: ");
  Serial.print(lightRight);
  Serial.println(")");
}

// ------------------------- MOISTURE READING -------------------------

int readMoistureFromSlave(uint8_t slaveAddr, const char* label) {
  delay(50); // Slight wait before request
  Wire.requestFrom(slaveAddr, 1);
  delay(20); // Allow time for slave response
  char moistureStatus = Wire.read();
  int level;

  if ((uint8_t)moistureStatus > 230)
    level = 1;
  else if ((uint8_t)moistureStatus > 168)
    level = 0;
  else
    level = -1;

  Serial.print(label);
  Serial.print(" Moisture: ");
  Serial.println(level);
  return level;
}

// ------------------------- WATER RATE & COMMAND -------------------------

int calculateWaterRate(int moisture, int temp) {
  if (moisture == -1) return 15;
  if (moisture == 0)  return (temp > 25) ? 10 : 5;
  return 0;
}

char getLightDominance() {
  return (lightLeft >= lightRight) ? 'L' : 'R';
}

void sendCommandToSlave(uint8_t slaveAddr, int moistureLevel) {
  int rate = calculateWaterRate(moistureLevel, temperature);
  char direction = getLightDominance();

  String message = String(direction) + "-" + String(rate);
  Wire.beginTransmission(slaveAddr);
  Wire.write(message.c_str());
  Wire.endTransmission();

  Serial.print("Sent to Slave ");
  Serial.print(slaveAddr);
  Serial.print(": ");
  Serial.println(message);
}
