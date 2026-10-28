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

* `dead_zone_prediction.ino` — Main project program.
* `turbidity_calibration.ino` — Turbidity sensor calibration helper.

## Project Demonstration

Project photographs and the demonstration video can be added to this repository as supporting evidence of the hardware implementation.

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
