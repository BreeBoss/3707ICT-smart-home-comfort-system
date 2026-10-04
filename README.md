# 3707ICT Automation and IoT
## Group Project – Smart Home Energy & Comfort Management System

### Team

| Name | Student ID |
|---|---|
| Ayush Lal | S5409751 |
| Brenda Powi | S5457769|
| Jason Gardner | S5369290 |

## Project Overview

This project implements a Smart Home Energy & Comfort Management System using an ESP32 DOIT DevKit V1, developed with PlatformIO and simulated using Wokwi.

The system monitors environmental conditions and occupancy using:

- DHT22 temperature and humidity sensor
- PIR motion sensor
- LDR ambient light sensor

The ESP32 processes these sensor readings locally and controls:

- LED for automated lighting
- Relay for cooling control
- Servo to represent the cooling/fan operation in the Wokwi simulation

Sensor readings and actuator states are also uploaded to ThingSpeak over HTTPS for remote monitoring and historical visualisation.

## Automation Rules

The system implements three automation rules.

### Rule 1 – Cooling Control

Cooling turns ON when:

- Motion is detected, AND
- Temperature is above 28°C OR humidity is above 70%

Cooling turns OFF when:

- No motion is detected, OR
- Temperature is below 26°C AND humidity is below 65%

Separate ON and OFF thresholds are used to reduce rapid switching near the threshold values.

### Rule 2 – Smart Lighting

The LED turns ON when:

- Motion is detected, AND
- Ambient light is below 25%

The LED turns OFF when the ambient light level reaches or exceeds 25%.

### Rule 3 – Inactivity Lighting Control

When the light is ON and no motion is detected, the ESP32 monitors the time since movement was last detected.

If no further motion is detected for 5 minutes, the LED automatically switches OFF.

New motion resets the inactivity timer.

## Edge Intelligence

The smart lighting system uses a rule-based intelligent decision processed locally on the ESP32.

PIR motion information and LDR ambient-light information are combined to determine whether artificial lighting is required. This allows the system to distinguish between situations such as movement in a dark room and movement in an already well-lit room.

The decision is processed locally, allowing the core automation to continue operating independently of the cloud connection.

## ThingSpeak Cloud Integration

The ESP32 connects to Wi-Fi and uploads system information to ThingSpeak using HTTPS GET requests approximately every 20 seconds.

The ThingSpeak channel contains:

1. Temperature
2. Humidity
3. Motion state
4. Light level
5. Cooling state
6. Lighting state

ThingSpeak is used for remote monitoring and historical data visualisation. Automation decisions are performed locally on the ESP32.

## Project Structure

```text
.
├── .vscode/
│   └── extensions.json
├── include/
│   ├── secrets.example.h
│   └── secrets.h          # Local only – not committed
├── src/
│   └── main.cpp
├── .gitignore
├── Diagram.json
├── platformio.ini
├── README.md
└── wokwi.toml
```

`secrets.h` is intentionally excluded from GitHub using `.gitignore`.

## Setup

### 1. Install Required Software

Install:

- Visual Studio Code
- PlatformIO IDE extension
- Wokwi Simulator extension

### 2. Clone or Download the Repository

Open the project folder in Visual Studio Code.

The folder containing `platformio.ini` is the PlatformIO project root.

### 3. Configure Secrets

The project includes:

```text
include/secrets.example.h
```

Create your local secrets file by copying it to:

```text
include/secrets.h
```

The file should contain:

```cpp
#pragma once

const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASSWORD = "";
const char* THINGSPEAK_WRITE_API_KEY = "your_Write_API_key_here";
```

Replace:

```text
your_Write_API_key_here
```

with the appropriate ThingSpeak Write API key.

For the Wokwi simulator, `Wokwi-GUEST` does not require a password, so the password can remain blank.

### Important

Never commit `include/secrets.h` or a real ThingSpeak API key to the repository.

Before committing changes, use:

```bash
git status
```

to confirm that `include/secrets.h` is not being tracked.

## Build and Run

1. Open the project in Visual Studio Code.
2. Build the project using PlatformIO **Build**, or run:

```bash
pio run
```

3. Confirm that the project builds successfully.
4. Open the project using the Wokwi Simulator extension.
5. Start the Wokwi simulation.
6. Use the simulated DHT22, PIR and LDR inputs to test the automation rules.
7. Use the Serial Monitor to observe sensor readings, actuator states and HTTPS responses.
8. Confirm that data is being uploaded to the configured ThingSpeak channel.

## Security

Wi-Fi credentials and the ThingSpeak Write API key are stored separately from the main source code in `include/secrets.h`.

The secrets file is excluded from Git using `.gitignore`, while `secrets.example.h` provides the required structure without exposing real credentials.

Communication with ThingSpeak uses HTTPS.

## Main Technologies

- ESP32 DOIT DevKit V1
- PlatformIO
- Wokwi
- DHT22
- PIR sensor
- LDR
- LED
- Relay
- Servo
- Wi-Fi
- HTTPS
- ThingSpeak
