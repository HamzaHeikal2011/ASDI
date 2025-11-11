# ASDI

This repo is dedicated to all the code of my project, ASDI (Adptive Smart Device Input).

## Components
- Arduino 
- MPU 6050 
- 5 potentiometers
- Breadboard
- male to male and female to male wires

***

## Readings Guide

Using the standard Arduino-IDE's Serial Plotter, you'll find 6 Values labeled A through F, each representing the Accelerometer/Gyroscope's X, Y and Z values. They are sorted according to this table:

|      | Gyro | Accelerometer |
| :--- | ---- | ------------- |
| X    | A    | D             |
| Y    | B    | E             |
| Z    | C    | F             |

The fingers, however, are much more simpler, each potentiometer sensor value being labeled according to the finger they belong to, all that's missing is connecting them to the correct pins

- Thumb: A0
- Pointer: A1
- Middle: A2
- Ring: A3*
- Pinkie: A5*

*MANUALLY ACTIVATED (Uncomment lines relevant to those fingers in the Main_Code.ino file for them to give readings)

---





