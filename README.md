# 3707ICT Automation and IoT

Group Project - Smart Home Comfort System

**Team**

| Name | Student ID |
|------|-----------|
| Ayush Lal | S5409751 |
| Brenda Powi | Sxxx |
| Jason Gardner | S5369290 |

## Project

PlatformIO firmware for the ESP32 DOIT DevKit V1, simulated in Wokwi, in `src/`. It reads DHT sensor data, controls a servo, runs an occupancy model (`occupancy_xgboost.h`) and posts to ThingSpeak over HTTPS.

## Setup

1. Install [VS Code](https://code.visualstudio.com/), the [PlatformIO IDE](https://platformio.org/install/ide?install=vscode) extension and the [Wokwi Simulator](https://marketplace.visualstudio.com/items?itemName=wokwi.wokwi-vscode) extension.
2. Open the `src` folder in VS Code (it is the PlatformIO project root).

### Secrets (Assignment project)

The Wi-Fi credentials and the ThingSpeak API key go in `include/secrets.h`. This file is gitignored, so each person has to create their own:

```sh
cd src
cp include/secrets.example.h include/secrets.h
```

Then edit `include/secrets.h`:

```cpp
const char* WIFI_SSID = "Wokwi-GUEST";   // Keep this value for the Wokwi simulator
const char* WIFI_PASSWORD = "";
const char* THINGSPEAK_WRITE_API_KEY = "your_Write_API_key_here"; // ThingSpeak channel > API Keys > Write API Key
```

Never commit `secrets.h`. Check with `git status` before you push.

### Build and run

1. Build the project with **PlatformIO: Build** (or `pio run`).
2. Open `Diagram.json` and start the simulation with **Wokwi: Start Simulator**. Wokwi loads the firmware from `.pio/build` through `wokwi.toml`.
