# TruthLens — Dead Zone Prediction System

## Overview

TruthLens is a hardware-based water quality monitoring and dead zone prediction prototype built using an ESP32 microcontroller and multiple water quality sensors.

The system collects sensor readings, displays measurements on an LCD, and uses programmed decision logic to indicate potential water quality risks.

The project explores how sensor-based monitoring can support the early identification of conditions associated with low dissolved oxygen in aquatic environments.

## Objectives

* Monitor selected water quality parameters using electronic sensors.
* Display sensor readings through an LCD.
* Use programmed logic to identify potentially concerning conditions.
* Provide visual alerts through indicator LEDs.
* Explore the use of embedded systems for environmental monitoring.

## Hardware Components

The project uses the following components, as applicable to the implemented setup:

* ESP32 development board
* pH sensor
* TDS sensor
* Turbidity sensor
* DS18B20 temperature sensor
* LCD display
* Breadboard and jumper wires
* Indicator LEDs
* Buzzer

## Working Principle

1. The ESP32 reads the connected sensor signals.
2. The program processes the readings to estimate or calculate the relevant water quality parameters.
3. The LCD displays the available measurements.
4. Programmed decision logic evaluates selected conditions.
5. Indicator LEDs and other connected alert components provide feedback according to the implemented logic.

## Software and Technologies

* Arduino IDE
* ESP32
* Embedded C/C++ (Arduino framework)
* Water quality sensors

## Project Files

### Arduino Source Code

- [Main Dead Zone Prediction Code](dead_zone_prediction.ino)
- [Turbidity Sensor Calibration Code](turbidity_calibration.ino)

### Demonstration Video

[Watch or download the project demonstration video](Demonstration%20video.mp4)

## Project Demonstration

The following photographs show the hardware setup and implementation of the TruthLens Dead Zone Prediction System.

### Hardware Setup

![Dead Zone Project Hardware](dead%20zone%20picture%20%281%29.png)

### Sensor and Circuit Setup

![Sensor and Circuit Setup](dead%20zone%20picture%20%282%29.png)

### ESP32 and Sensor Connections

![ESP32 and Sensor Connections](dead%20zone%20picture%20%283%29.png)

### Additional Project View

![Additional Hardware View](dead%20zone%20picture%20%284%29.png)

## Important Technical Note

This is a prototype, not a certified water quality testing system.

Dissolved oxygen is an important factor in aquatic dead zones. If dissolved oxygen is estimated using a mathematical model rather than measured with a dedicated dissolved oxygen sensor, the estimate should not be treated as a direct measurement.

The reliability of the prediction depends on sensor calibration, the implemented decision logic, and validation against appropriate reference measurements.

## Future Improvements

* Add a dedicated dissolved oxygen sensor.
* Improve sensor calibration and measurement accuracy.
* Validate prediction logic using reference water quality data.
* Record readings over time for further analysis.
* Explore data visualization and machine learning for improved risk assessment.

## Project Status

Hardware prototype developed. Further testing and validation can improve the accuracy and reliability of the system.
