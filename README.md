# CPS_IOT_HW

# Smart Plant Monitoring and Irrigation System

## Team:
- Alireza Hosseini
- Soheil HajianManesh
- SeyedMahdi HajiSeyedHossein
- Amirali Shahriary

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

## 🔌 I2C Communication Setup

```cpp
#define SLAVE_ADDR_1 2  // Address for ARD1 (slave)
#define SLAVE_ADDR_2 3  // Address for ARD2 (slave)
Wire.begin();

The Master is initialized to use I2C (Wire.begin()) and communicates with two slaves identified by addresses 2 and 3.

const int tempraturePin = A0;
const int ldrLeftPin = A1;
const int ldrRightPin = A2;


Analog pins are assigned to:

A0: Temperature sensor

A1: LDR (left)

A2: LDR (right)

