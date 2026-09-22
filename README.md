
# Underwater Vehicle – Minimum Travel Time

A Thermofluid Laboratory course project focused on the **design, development, and testing of an underwater vehicle prototype for minimum travel time under hydrodynamic constraints**.

The project combines concepts from **fluid mechanics, hydrodynamics, propulsion, mechanical design, and embedded motor control** to develop a functional underwater vehicle prototype.

---

## Project Information

| Parameter | Details |
|---|---|
| **Project** | Underwater Vehicle – Minimum Travel Time |
| **Course** | Thermofluid Laboratory |
| **Supervisor** | Dr. Navneeth KM |
| **Duration** | Jul 2025 – Nov 2025 |
| **Author** | Jahnavi Sharma |
| **Discipline** | Mechanical Engineering |
| **Achieved Speed** | ~0.5 m/s |

---

## Project Overview

The objective of this project was to design and develop an underwater vehicle capable of achieving **minimum travel time while operating under hydrodynamic constraints**.

The project involved studying the hydrodynamic resistance acting on the vehicle, evaluating propulsion requirements, and developing a suitable vehicle configuration for underwater motion.

A physical prototype was developed and integrated with a **dual-motor propulsion system**. An **ESP32-based control system** was implemented to control the propulsion motors using dual joystick inputs.

The final prototype achieved an operating speed of approximately **0.5 m/s** during testing.

---

## Objectives

- Design and develop an underwater vehicle prototype for minimum travel time.
- Analyze hydrodynamic resistance acting on the vehicle.
- Study the effect of vehicle geometry on underwater performance.
- Evaluate propulsion requirements for the desired operating speed.
- Develop a functional propulsion and motor-control system.
- Integrate mechanical and electronic subsystems.
- Experimentally evaluate the performance of the prototype.

---

## Engineering Concepts

- Fluid Mechanics
- Thermofluids
- Hydrodynamics
- Hydrodynamic Drag
- Propulsion
- Mechanical Design
- Vehicle Geometry
- Experimental Testing
- Embedded Motor Control

---

## Design & Analysis

The vehicle design considered the interaction between **vehicle geometry, hydrodynamic drag, propulsion requirements, and achievable speed**.

The drag force was represented using:

\[
F_D = \frac{1}{2}\rho V^2 C_D A
\]

where:

- `F_D` = Hydrodynamic drag force
- `ρ` = Fluid density
- `V` = Vehicle velocity
- `C_D` = Drag coefficient
- `A` = Reference area

The analysis was used to understand the resistance encountered during underwater motion and its influence on propulsion requirements and vehicle performance.

Detailed calculations and supporting analysis are provided in the `analysis/` directory.

---

## Propulsion System

The prototype uses a **dual-motor propulsion system**.

The propulsion system was designed to provide sufficient force to overcome the hydrodynamic resistance of the vehicle and enable controlled underwater movement.

The system uses:

- 2 × DC Motors
- L298N Dual H-Bridge Motor Driver
- ESP32 Development Board
- 2 × Analog Joysticks

The left and right motors can be controlled independently through the joystick inputs.

---

## Control System

The ESP32-based controller provides:

- Automatic joystick calibration
- Dead-zone compensation
- Input smoothing
- Bidirectional motor control
- PWM-based speed control
- Independent left and right motor control
- Serial monitoring for debugging

### Control Flow

```text
Dual Joystick Input
        ↓
      ESP32
        ↓
Input Smoothing & Calibration
        ↓
   Dead-Zone Processing
        ↓
  Motor Speed Mapping
        ↓
       PWM
        ↓
   L298N Motor Driver
        ↓
   DC Motors
        ↓
Vehicle Propulsion
