# VU MIF Robotics — Homework Nr. 3

## Main Goal

This project implements an interrupt-driven servo control system using an Arduino Uno.  
A button press triggers an external interrupt (INT0), toggling a servo motor between 0° and 90°.  
A hardware timer interrupt (Timer2) generates 10 ms periodic timing events.  
The current servo angle and button press count are stored in EEPROM and restored after power cycling.

---

## Features

- External interrupt using INT0 (button on pin D2)
- Hardware timer interrupt using Timer2 (10 ms period)
- Servo toggles between 0° ↔ 90°
- LED indicates the current servo state
- EEPROM stores:
  - Last servo angle
  - Button press count
  - Validation byte
- Fully non-blocking (no `delay()`)
- Real-time, interrupt-driven responsiveness

---

## System Overview

When powered on:

1. Servo resets to 0°.
2. Previous state is loaded from EEPROM.
3. Timer2 begins generating 10 ms interrupts.
4. A button press triggers the external interrupt, toggling servo and LED.
5. Every press increments a persistent counter.
6. After reboot, servo angle and press count are restored.

The system is fully asynchronous:
- External interrupt handles user input immediately.
- Timer interrupt provides periodic scheduling.
- Main loop only reacts to interrupt-set flags.

---

## Demo Video
https://www.youtube.com/watch?v=y7dThOe1k6w

___

## Arduino Code Highlights

### External Interrupt
Handles button presses immediately:
```cpp
attachInterrupt(digitalPinToInterrupt(buttonPin), onButtonPress, FALLING);
