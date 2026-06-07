# 🌿 ASTRO-FLORA: Automated Martian Greenhouse Module

An automated bio-dome life-support terminal engineered for the Hack Club x NASA Stardance Challenge. Astro-Flora monitors soil hydration parameters in real-time, manages light cycles, and autonomously dispenses water via external automated relay sub-routines to keep space crops healthy on Mars.

---

## 🛠️ Hardware Mapping Matrix

| Component | Arduino Pin | Description |
| :--- | :--- | :--- |
| **I2C LCD Screen** | **A4 / A5** | System Status GUI Display Panel |
| **Moisture Sensor** | **A0** | Analog Soil Hydration Transducer |
| **Irrigation Pump** | **Pin 3** | Transistor/Relay Automated Water Output |
| **Solar Grow Light**| **Pin 4** | Simulates Space Station Daylight Cycle |
| **Alarm Buzzer** | **Pin 7** | Acoustic Diagnostic Status Indicator |

---

## 🛸 Key Features
* **Custom LCD Font Buffering:** Renders custom binary-mapped sprout glyphs natively onto the LCD interface screen.
* **Automatic Water Regulation:** Instantly activates emergency irrigation cycles for 3000ms when levels slip below safety thresholds.
* **Continuous Serial Telemetry:** Streams active percentages to the serial data output lines for remote data harvesting.
