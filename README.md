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


# Master Code – Smart Irrigation Controller :

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

---

# Slave Code Explanation:

The Slave Arduino receives commands from the Master via I2C and processes them to control a servo and LEDs based on moisture levels and water rate calculations. The Slave also sends its moisture data back to the Master when requested. Below is a detailed explanation of the key parts of the code.

### Code Block 1: Pin Definitions and Initialization

```cpp
#define MY_ADDRESS 2  // Slave address

Servo myServo;
const int led1 = 7; 
const int led2 = 6;  
const int led3 = 5;  // LED 5
const int servo_pin = 8; 
const int soilMoisturePin = A3;
int moistureSensor;
int moisture;
```
MY_ADDRESS: The I2C address for the slave device (set to 2).

Servo myServo: Creates a Servo object to control the servo motor.

LED Pins (led1, led2, led3): Pins connected to the LEDs for visual feedback of water rate.

servo_pin: Pin connected to the servo.

soilMoisturePin: Pin connected to the soil moisture sensor.

moistureSensor: Stores the raw value from the moisture sensor.

moisture: Stores the categorized moisture level.

### Code Block 2: Setup Function:

```cpp
void setup() {
  Wire.begin(MY_ADDRESS); // Slave address
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  Serial.begin(9600);
  Serial.println("Slave 1 Ready to work");
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  myServo.attach(servo_pin);
}

```

Wire.begin(MY_ADDRESS): Initializes I2C communication with the specified slave address.

Wire.onReceive(receiveEvent): Registers the receiveEvent function to handle incoming data from the master.

Wire.onRequest(requestEvent): Registers the requestEvent function to handle requests from the master.

Serial.begin(9600): Starts serial communication for debugging.

pinMode(): Configures the LED pins and servo pin as output.

myServo.attach(servo_pin): Attaches the servo control to the specified pin.

### Code Block 3: Main Loop:

```cpp
void loop() {
  moistureSensor = analogRead(soilMoisturePin);
  if(moistureSensor >= 800){
    moisture = 250; // Moisture more than 80%
  } else if(moistureSensor >= 500 && moistureSensor < 800){
    moisture = 170; // Moisture less than 50%
  } else {
    moisture = 80; // Moisture between 80% and 50%
  }
  Serial.print("Moisture S1 = ");
  Serial.println(moistureSensor);
  Serial.print("Mapping S1= ");
  Serial.println(moisture);
  delay(250);
}

```

analogRead(soilMoisturePin): Reads the analog value from the soil moisture sensor.

Conditionals: Categorizes the moisture level based on the raw sensor reading:

moisture >= 800: High moisture (mapped to 250).

500 <= moisture < 800: Medium moisture (mapped to 170).

moisture < 500: Low moisture (mapped to 80).

Serial.print/println: Outputs the raw moisture reading and its mapped value to the serial monitor for debugging.

delay(250): Introduces a small delay for stability.


### Code Block 4: Event Handling (Receive and Request):

```cpp

void receiveEvent(int howMany) {
  char buffer[10];
  readMessage(buffer, sizeof(buffer));
  
  Serial.print("Received: ");
  Serial.println(buffer);

  char pos;
  int rate;
  if (parseMessage(buffer, pos, rate)) {
    Serial.print("---Parsed rate: ");
    Serial.println(rate);
    Serial.print("---Parsed position: ");
    Serial.println(pos);

    setServoPosition(pos);
    setLEDs(rate);
  }
}

void requestEvent(){
  Wire.write((byte*)&moisture, sizeof(moisture)); // Sizeof int is 2 bytes
}


```

readMessage(): Reads the incoming message from the master into a buffer.

parseMessage(): Extracts the position (pos) and water rate (rate) from the received message. The message is expected to be in the format "L-10", where L is the position and 10 is the water rate.

setServoPosition(pos): Adjusts the servo position based on the received position ('L' or 'R').

setLEDs(rate): Controls the LEDs based on the water rate:

LED 1: Lights up if rate >= 5.

LED 2: Lights up if rate >= 10.

LED 3: Lights up if rate >= 15.

Wire.write(): Sends the moisture level data (an integer) to the master when requested.


### Code Block 5: ECode Block 5: Helper Functions ( Reading Message , Parsing the message , Servo control,LED control):

```cpp

void readMessage(char* buffer, size_t bufferSize) {
  byte i = 0;
  while (Wire.available() && i < bufferSize - 1) {
    buffer[i++] = Wire.read();
  }
  buffer[i] = '\0'; // null-terminate
}


```

  Wire.read(): Reads bytes from the I2C bus into the buffer.

Null-termination: Ensures the string is properly terminated for string manipulation.


```cpp
bool parseMessage(char* buffer, char& pos, int& rate) {
  char* dashPtr = strchr(buffer, '-');
  if (dashPtr == NULL) return false;

  *dashPtr = '\0';
  pos = buffer[0];
  rate = atoi(dashPtr + 1);
  return true;
}

```

strchr(): Finds the position of the dash (-) in the message buffer.

atoi(): Converts the string part after the dash into an integer (the water rate).

Returns: true if parsing is successful, otherwise false.

```cpp
void setServoPosition(char pos) {
  int position_rotate = (pos == 'L') ? 60 : 120;
  myServo.write(position_rotate);
}

```

myServo.write(): Sets the servo to the specified position. If the position is 'L', the servo rotates to 60 degrees; if it's 'R', it rotates to 120 degrees.

```cpp
void setLEDs(int rate) {
  digitalWrite(led1, rate >= 5);
  digitalWrite(led2, rate >= 10);
  digitalWrite(led3, rate >= 15);
}

```

digitalWrite(): Turns on the LEDs based on the water rate.

LED 1 lights up for rates >= 5.

LED 2 lights up for rates >= 10.

LED 3 lights up for rates >= 15.


---
##  Components Used (in Proteus)

###  Microcontrollers
- **2x Arduino Uno**
  - **Master Arduino**: Gathers sensor data (moisture, temperature), makes decisions, and sends instructions via I2C.
  - **Slave Arduino**: Receives servo and LED commands, and reports moisture level back to the master.

---

###  Sensors
- **Soil Moisture Sensors** (`SOIL1`, `SOIL2`):
  - Connected to analog pins .
  - Measures the volumetric water content in the soil.
  - Analog signal used for finer accuracy (thresholds defined in code).

- **LM35 Temperature Sensor**:
  - Connected to Master Arduino.
  - Provides ambient temperature data to calculate watering rate.

---

###  Actuators and Output Devices
- **2x Servo Motors**:
  - Connected to digital pins 
  - Represents the valve opening direction (`L` for left, `R` for right).
  
- **3x LEDs per Arduino**:
  - Indicate the watering rate
  

- **Buzzers**:
  - For audible alert (optional in simulation, connected in hardware setup).

- **LCD Displays**:
  - Display temperature and rate on the master side.
  - Display moisture and status on the slave side.

---

### 🔌 Communication
- **I2C Bus**:
  - **SDA** and **SCL** lines connected between the two Arduinos.
  - **Master address**: Implicit (`Wire.begin()`).
  - **Slave address**: `2` (defined via `#define MY_ADDRESS 2`).

---

### 🔋 Power Supply
- 5V and GND rails provided to all modules.
- Pull-up resistors used on I2C lines if needed (Proteus sometimes auto-configures).

---


## 📷 Circuit Diagram (Proteus)

![Circuit Diagram](https://github.com/notarealone/CPS_IOT_HW/blob/main/pic/Screenshot%202025-04-29%20231029.png)

## ▶️ How to Simulate

1. Open the `.pdsprj` file in Proteus.
2. Load the **Master Arduino** with the compiled Master sketch.
3. Load the **Slave Arduino** with the compiled Slave sketch.
4. Press **Play** to start the simulation.
5. Adjust soil moisture (via potentiometer or slider) and observe system behavior.

---

## 🧪 Output Observation

- LCDs show temperature, rate, and moisture levels.
- LEDs indicate the watering intensity.
- Servo rotates according to calculated position.
- I2C ensures proper master-slave synchronization.
