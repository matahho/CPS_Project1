# CPS_IOT_HW

# Smart Plant Monitoring and Irrigation System

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


## Hardware Components

- 3 x Arduino boards (1 Master + 2 Slaves)
- Soil moisture sensors (A3 analog pin)
- LDR sensors (A1 and A2 analog pins on Master)
- Temperature sensor (to be added, e.g., LM35 or DHT11)
- Servo motors (for rotating plants)
- DC pumps or solenoid valves (for watering plants)


## Communication Protocol

- **I2C Communication**
  - Master initiates communication.
  - Slaves provide soil moisture readings.
  - Master sends specific control codes for actuation.
