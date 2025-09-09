# Access Control System

An **Arduino-based access control system** with **real-time occupancy tracking**, **health-check verification**, and **live status display** using an **OLED module**. The project integrates multiple hardware components and provides both **visual feedback** on the OLED and **serial logging** for debugging and analysis.

---

## **Features**
- 🟢 **Ultrasonic sensor-based detection** – detects approaching individuals before entry.
- 🟢 **Multi-stage access control:**
  - Mandatory **health check** button before granting entry.
  - Separate **entry** and **exit** buttons for managing people flow.
- 🟢 **Maximum occupancy enforcement** – denies entry automatically when the set capacity is reached.
- 🟢 **Dynamic OLED display** *(SH1106-based)* showing:
  - People count
  - Occupancy level (**LOW / MEDIUM / HIGH / FULL**)
  - Health check & access results
- 🟢 **Automatic event logging** to a `.txt` file for tracking full system runs.
- 🟢 **Failsafe mode** – if OLED fails, system continues logging via Serial Monitor.

---

## **Hardware Components**
| Component                     | Purpose                                |
|------------------------------|----------------------------------------|
| **Arduino Uno**             | Core microcontroller for system logic |
| **HC-SR04 Ultrasonic Sensor** | Approaching individual detection     |
| **Adafruit SH1106 OLED (1.3")** | Live status display                 |
| **Push Buttons (x3)**       | Health Check, Entry, and Exit control |
| **LEDs (x2)**               | Green = Access Granted, Red = Denied |
| **Power Supply**            | 5V regulated supply                   |

---

## **How It Works**
1. **Person Approaches →** Ultrasonic sensor detects motion.
2. **System Displays Occupancy →** OLED shows **current capacity** and **status**.
3. **Health Check →**  
   - ✅ If passed → proceed to Entry button.
   - ❌ If failed → access denied, Red LED blinks.
4. **Entry / Exit Buttons →**  
   - Entry button increments people count.
   - Exit button decrements people count.
5. **At Max Capacity →**  
   - OLED displays **“Maximum Capacity Reached”**.
   - Red LED lights up, access permanently denied until someone exits.

---

## **Occupancy Levels**
| People Count      | Status   |
|-------------------|----------|
| **0 – 40%**      | LOW      |
| **41% – 80%**    | MEDIUM   |
| **81% – 99%**    | HIGH     |
| **100%**         | FULL     |

---

## **Project Improvements Over Build 1**
- Added **MAX_CAPACITY enforcement**.
- Introduced **real-time occupancy updates** before entry decisions.
- Integrated **OLED display** for live feedback.
- Implemented **logging** to `.txt` for full test runs.
- Optimized hardware polling & button debounce logic.
