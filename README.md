# ESP8266 Vacuum Robot Controller

Welcome to the setup guide for the **ESP8266 Vacuum Robot Controller**. This project enables you to control a vacuum robot using an ESP8266 microcontroller and a web-based interface. The provided firmware (`esp8266_vacuum_code.txt`) and web interface (`vacuum_control_app.html`) allow for manual control, automatic cleaning, and status monitoring. Follow this guide to set up the project using the Arduino IDE.

## Table of Contents
- [Project Overview](#project-overview)
- [Prerequisites](#prerequisites)
- [Setup Instructions](#setup-instructions)
  - [Step 1: Install Arduino IDE and Dependencies](#step-1-install-arduino-ide-and-dependencies)
  - [Step 2: Prepare and Upload Firmware](#step-2-prepare-and-upload-firmware)
  - [Step 3: Set Up the Web Interface](#step-3-set-up-the-web-interface)
- [Using the Vacuum Robot](#using-the-vacuum-robot)
- [Troubleshooting](#troubleshooting)
- [Additional Notes](#additional-notes)
- [Support and Resources](#support-and-resources)

## Project Overview
This project uses an ESP8266 microcontroller to control a vacuum robot with the following features:

- **Manual Control**: Move forward, backward, left, right, or stop via a web interface.
- **Auto Mode**: Autonomous cleaning with a predefined movement pattern.
- **Status Monitoring**: Real-time battery and storage level updates.
- **Scheduling**: Add cleaning schedules (requires additional implementation for persistence).
- **Alerts**: Buzzer notifications for a full dust bin.

The firmware runs on the ESP8266, while the web interface (`vacuum_control_app.html`) provides a user-friendly control panel.

## Prerequisites

### Hardware
- ESP8266-based microcontroller (e.g., NodeMCU, Wemos D1 Mini)
- Motor driver (e.g., L298N) for left and right motors
- Vacuum motor
- Buzzer for alerts
- Status LED
- Ultrasonic sensor (HC-SR04) for dust bin level detection
- Battery voltage monitoring circuit
- USB cable for programming

### Software
- Arduino IDE (version 1.8.19 or later)
- Web browser (e.g., Chrome, Firefox) for the control interface

## Setup Instructions

### Step 1: Install Arduino IDE and Dependencies
1. **Install Arduino IDE**:
   - Download and install the Arduino IDE from [arduino.cc](https://www.arduino.cc/en/software).

2. **Add ESP8266 Board Support**:
   - Open Arduino IDE and navigate to **File > Preferences**.
   - In the "Additional Boards Manager URLs" field, add:
     ```
     http://arduino.esp8266.com/stable/package_esp8266com_index.json
     ```
   - Go to **Tools > Board > Boards Manager**, search for `esp8266`, and install the `esp8266` platform by ESP8266 Community.

3. **Install Required Libraries**:
   - Go to **Sketch > Include Library > Manage Libraries**.
   - Install the following libraries:
     - `ESP8266WiFi` (included with ESP8266 platform)
     - `ESP8266WebServer` (included with ESP8266 platform)
     - `ArduinoJson` (version 6.x recommended)
     - `NewPing` (for ultrasonic sensor support)

### Step 2: Prepare and Upload Firmware
1. **Create the Arduino Sketch**:
   - Copy the contents of `esp8266_vacuum_code.txt` into a new Arduino sketch.
   - Save the sketch as `esp8266_vacuum_code.ino`.

2. **Configure WiFi Credentials**:
   - Locate the following lines in the sketch:
     ```cpp
     const char* ssid = "YOUR_WIFI_NAME";
     const char* password = "YOUR_WIFI_PASSWORD";
     ```
   - Replace `YOUR_WIFI_NAME` and `YOUR_WIFI_PASSWORD` with your WiFi network’s SSID and password.

3. **Connect Hardware**:
   - Wire the ESP8266 to the hardware using the pin definitions:
     - **Left Motor**: Pins D1 (GPIO 5) and D2 (GPIO 4)
     - **Right Motor**: Pins D3 (GPIO 0) and D4 (GPIO 2)
     - **Vacuum Motor**: Pin D5 (GPIO 14)
     - **Buzzer**: Pin D6 (GPIO 12)
     - **Status LED**: Pin D7 (GPIO 13)
     - **Battery Monitoring**: Analog pin A0
     - **Ultrasonic Sensor**: Trigger Pin D8 (GPIO 15), Echo Pin RX (GPIO 3)
   - **Note**: The RX pin is used for the ultrasonic sensor’s echo. Use caution, as it’s also used for serial communication during programming.
   - Verify all connections are secure.

4. **Upload the Firmware**:
   - Connect the ESP8266 to your computer via USB.
   - In Arduino IDE, select the board (e.g., `NodeMCU 1.0 (ESP-12E Module)`) under **Tools > Board**.
   - Select the correct port under **Tools > Port**.
   - Click **Upload** to flash the firmware.
   - Open the Serial Monitor (**Tools > Serial Monitor**, 115200 baud) to confirm WiFi connection. The ESP8266’s IP address will be displayed.

### Step 3: Set Up the Web Interface
1. **Host the Web Interface**:
   - The `vacuum_control_app.html` file provides the control interface.
   - **Local Hosting**: Save the file locally and open it in a web browser.
   - **Web Server**: Upload the file to a web server (e.g., GitHub Pages or a local server) for network access.
   - **Test Endpoint**: Access `http://<ESP8266_IP_ADDRESS>/` in a browser to confirm the ESP8266 is running (should display "ESP8266 Vacuum Controller Ready").

2. **Configure the Interface**:
   - Ensure your device is on the same WiFi network as the ESP8266.
   - Update the JavaScript in `vacuum_control_app.html` to use the ESP8266’s IP address (from the Serial Monitor) for API endpoints (e.g., `http://<ESP8266_IP_ADDRESS>/forward`).
   - Open the HTML file in a browser to access the control panel.

## Using the Vacuum Robot
- **Manual Control**: Use the web interface to:
  - Move the robot (Forward, Backward, Left, Right, Stop)
  - Start or stop cleaning
  - Toggle auto mode
  - Return to base
- **Auto Mode**: Enable auto mode for a predefined movement pattern (forward, right, forward, left every 2 seconds).
- **Status Monitoring**: View real-time updates for:
  - Battery level
  - Dust bin storage level
  - Runtime
  - Current mode (Manual/Auto)
  - Home status
- **Scheduling**: Add cleaning schedules via the interface. **Note**: Persistent scheduling requires additional EEPROM or storage logic in the firmware.
- **Alerts**: The buzzer sounds if the dust bin exceeds 85% capacity during cleaning.

## Troubleshooting
- **WiFi Connection Issues**:
  - Verify SSID and password in the firmware.
  - Check the Serial Monitor for connection status.
- **Web Interface Not Responding**:
  - Confirm the ESP8266’s IP address in the HTML file’s JavaScript.
  - Ensure the ESP8266 is powered and connected to WiFi.
- **Sensor Issues**:
  - Verify ultrasonic sensor wiring if storage level readings are inaccurate.
  - Check the battery monitoring circuit for valid analog readings.
- **Motor Issues**:
  - Confirm motor driver connections and power supply.
  - Test motor controls using the web interface.
- **General Debugging**:
  - Use the Serial Monitor to view runtime logs and errors.

## Additional Notes
- The RX pin (GPIO 3) is shared between the ultrasonic sensor and serial communication. Ensure proper handling during programming.
- CORS headers are included in the firmware to support cross-origin requests from the web interface.
- Auto mode logic is basic and may benefit from enhancements like obstacle detection or advanced navigation.
- The scheduling feature requires additional logic for persistent storage (e.g., using EEPROM).

## Support and Resources
- [ESP8266 Community Forum](https://www.esp8266.com/) for hardware and firmware support
- [Arduino Forum](https://forum.arduino.cc/) for general Arduino-related queries
- [ArduinoJson Documentation](https://arduinojson.org/) for JSON handling
- [NewPing Library Documentation](https://bitbucket.org/teckel12/arduino-new-ping/wiki/Home) for ultrasonic sensor usage

For further assistance, refer to the above resources or check the Serial Monitor output for detailed logs.
