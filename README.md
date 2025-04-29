# CPS_IOT_HW

# Smart Plant Monitoring and Irrigation System

## Team:
- Alireza Hosseini (810100125)
- Soheil HajianManesh (810100119)
- SeyedMahdi HajiSeyedHossein (810100118)
- Amirali Shahriary (810100173)

## Project Overview
This project implements a smart plant monitoring system using Arduino boards that can:
- Measure soil moisture, light intensity, and temperature.
- Automatically decide when and how much to water plants.
- Adjust the plant orientation based on light intensity.

The system uses two Edge Nodes (slaves) and one Central Node (master) communicating via I2C protocol.

---

## System Architecture

- **Central Node (Master)**
  - Gathers soil moisture data from two Edge Nodes.
  - Reads environmental sensors (temperature and light).
  - Decides watering rates and plant movement.
  - Sends control commands to Edge Nodes via I2C.

- **Edge Nodes (Slave 1 & Slave 2)**
  - Measure local soil moisture.
  - Receive commands from the Central Node.
  - Actuate motors and water valves accordingly.


## Hardware Components Used
- 3x Arduino UNO (or compatible)
- 2x Soil Moisture Sensors
- 2x Light Dependent Resistors (LDRs)
- 1x Temperature Sensor (Analog)
- 2x Servo Motors
- 6x LEDs (3 for each slave)
- I2C wires (SDA/SCL)
- Resistors (for LDR voltage dividers)
- Breadboard and jumper wires



## Communication Protocol

- **Master → Slave (write):**
  - Message format: `'L-10'` (position-light + watering rate)
- **Slave → Master (read):**
  - Sends 1 byte (mapped from analog moisture reading)
 
---


# Master Code – Smart Irrigation Controller

This section provides a detailed explanation of the Master Arduino code used in a smart irrigation system. The Master device reads environmental data (temperature and light), requests soil moisture data from two slave devices, computes irrigation rates, and sends appropriate commands back to the slaves using the I2C protocol.

---

### Code Block 1: Slave Addresses and Sensor Pins

```cpp
// Slave Addresses
#define SLAVE_ADDR_RIGHT 2
#define SLAVE_ADDR_LEFT  3

// Sensor Pins
const int TEMP_PIN = A0;
const int LDR_LEFT_PIN = A1;
const int LDR_RIGHT_PIN = A2;

```
SLAVE_ADDR_RIGHT: I2C address for the right slave (ARD1).

SLAVE_ADDR_LEFT: I2C address for the left slave (ARD2).

TEMP_PIN: Pin for reading temperature data.

LDR_LEFT_PIN: Pin for reading the left Light Dependent Resistor (LDR) sensor.

LDR_RIGHT_PIN: Pin for reading the right Light Dependent Resistor (LDR) sensor.

### Code Block 2: Setup Function

```cpp
void setup() {
  Wire.begin();
  Serial.begin(9600);
  Serial.println("Master Setup Complete");
}


```

Wire.begin(): Initializes I2C communication as the master.

Serial.begin(9600): Starts serial communication for debugging.

Serial.println("Master Setup Complete"): Sends a message to indicate that the setup process has finished.

### Code Block 3: Main Loop

```cpp
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

```

readTemperature(): Reads the temperature from the sensor and maps it to a value.

readLightSensors(): Reads the light levels from both LDR sensors.

readMoistureFromSlave(): Requests moisture data from the slave devices and processes it.

sendCommandToSlave(): Sends commands to the slaves based on the moisture level and light dominance.

Delays: Introduces pauses between sensor reads and communication for stability.

### Code Block 4: Temperature Reading

```cpp
void readTemperature() {
  int rawValue = analogRead(TEMP_PIN);
  temperature = map(rawValue, 0, 205, 0, 100);
  Serial.print("Temperature: ");
  Serial.println(temperature);
}

```
analogRead(TEMP_PIN): Reads the analog value from the temperature sensor.

map(rawValue, 0, 205, 0, 100): Maps the raw sensor value to a temperature value between 0 and 100.

Serial.print/println: Prints the temperature value to the serial monitor for debugging.


### Code Block 5: Light Sensor Readings

```cpp
void readLightSensors() {
  lightLeft = analogRead(LDR_LEFT_PIN);
  lightRight = analogRead(LDR_RIGHT_PIN);
  Serial.print("Light (L: ");
  Serial.print(lightLeft);
  Serial.print(" - R: ");
  Serial.print(lightRight);
  Serial.println(")");
}

```

analogRead(LDR_LEFT_PIN): Reads the analog value from the left LDR sensor.

analogRead(LDR_RIGHT_PIN): Reads the analog value from the right LDR sensor.

Serial.print/println: Outputs the light readings of both sensors for debugging.

### Code Block 6: Moisture Reading from Slaves
```cpp

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


```

Wire.requestFrom(slaveAddr, 1): Requests 1 byte of data from the slave device at the specified address.

Wire.read(): Reads the moisture status sent by the slave.

Serial.print/println: Displays the moisture level for each side (left or right) on the serial monitor.

Moisture Levels: The moisture status is categorized as:

1: High moisture.

0: Normal moisture.

-1: Low moisture.


### Code Block 7: Moisture Reading from Slaves

```cpp
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

```

calculateWaterRate(moisture, temp): Calculates the water rate based on moisture level and temperature.

If moisture is -1, a high water rate of 15 is returned.

If moisture is 0, a moderate water rate of 10 or 5 is returned depending on the temperature.

If moisture is 1, no water is needed (0).

getLightDominance(): Determines which LDR (left or right) has higher light intensity.

sendCommandToSlave(): Sends a command to the slave based on the calculated water rate and light dominance. The command is formatted as a string in the form "L-10", where L is the light dominance and 10 is the water rate.
