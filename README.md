ESP8266 Vacuum Robot Controller Setup Guide
This guide provides instructions for setting up the ESP8266 Vacuum Robot Controller using the Arduino IDE, including the required firmware (esp8266_vacuum_code.txt) and the web-based control interface (vacuum_control_app.html).
Prerequisites

Hardware:
ESP8266-based microcontroller (e.g., NodeMCU, Wemos D1 Mini)
Motor driver for left and right motors (e.g., L298N)
Vacuum motor
Buzzer
LED for status indication
Ultrasonic sensor (HC-SR04) for storage level detection
Battery voltage monitoring circuit
USB cable for programming


Software:
Arduino IDE (version 1.8.19 or later)
Web browser for accessing the control interface



Installation Steps
1. Set Up Arduino IDE

Download and Install Arduino IDE:

Download the Arduino IDE from arduino.cc.
Install it on your computer.


Add ESP8266 Board Support:

Open Arduino IDE and go to File > Preferences.
In the "Additional Boards Manager URLs" field, add:http://arduino.esp8266.com/stable/package_esp8266com_index.json


Go to Tools > Board > Boards Manager, search for esp8266, and install the esp8266 platform by ESP8266 Community.


Install Required Libraries:

Go to Sketch > Include Library > Manage Libraries.
Install the following libraries:
ESP8266WiFi (pre-installed with ESP8266 platform)
ESP8266WebServer (pre-installed with ESP8266 platform)
ArduinoJson (search and install, version 6.x recommended)
NewPing (search and install, for ultrasonic sensor)





2. Prepare the Firmware

Open the Firmware File:

Copy the contents of esp8266_vacuum_code.txt into a new Arduino sketch in the Arduino IDE.
Save the sketch as esp8266_vacuum_code.ino.


Configure WiFi Credentials:

In the sketch, locate the following lines:const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";


Replace YOUR_WIFI_NAME and YOUR_WIFI_PASSWORD with your WiFi network's SSID and password.


Connect Hardware:

Connect the ESP8266 to your hardware as per the pin definitions in the code:
Motor Left: Pins D1 (5) and D2 (4)
Motor Right: Pins D3 (0) and D4 (2)
Vacuum Motor: Pin D5 (14)
Buzzer: Pin D6 (12)
Status LED: Pin D7 (13)
Battery Monitoring: Analog pin A0
Ultrasonic Sensor: Trigger Pin D8 (15), Echo Pin RX (3)


Ensure the ultrasonic sensor's echo pin (RX) is used carefully, as it’s a special pin on the ESP8266.
Verify all connections are secure and match the pin definitions.


Upload the Firmware:

Connect the ESP8266 to your computer via USB.
In Arduino IDE, select the appropriate board under Tools > Board (e.g., NodeMCU 1.0 (ESP-12E Module)).
Select the correct port under Tools > Port.
Click Upload to flash the firmware to the ESP8266.
Open the Serial Monitor (Tools > Serial Monitor, set to 115200 baud) to verify the ESP8266 connects to WiFi. It will display the IP address once connected.



3. Set Up the Web Interface

Host the Web Interface:

The vacuum_control_app.html file provides a web-based control interface.
You can host it locally or on a web server:
Local Hosting: Save vacuum_control_app.html on your computer and open it in a web browser.
Web Server: Upload the file to a web server (e.g., GitHub Pages, a local server, or any hosting service) to access it from any device on the same network.


Alternatively, access the ESP8266’s root endpoint (http://<ESP8266_IP_ADDRESS>/) directly to confirm it’s running (it will display "ESP8266 Vacuum Controller Ready").


Access the Interface:

Ensure your device is on the same WiFi network as the ESP8266.
Open vacuum_control_app.html in a web browser.
The interface communicates with the ESP8266 using its IP address (displayed in the Serial Monitor).
Update the JavaScript in vacuum_control_app.html to point to the ESP8266’s IP address. Locate and modify the relevant API endpoint URLs (e.g., http://<ESP8266_IP_ADDRESS>/forward) in the HTML file’s JavaScript section.



4. Using the Vacuum Robot

Manual Control: Use the web interface to control movement (Forward, Backward, Left, Right, Stop), start/stop cleaning, toggle auto mode, or return to base.
Auto Mode: Enable auto mode to make the robot move in a predefined pattern (forward, right, forward, left) every 2 seconds.
Status Monitoring: The interface displays battery level, storage level, runtime, mode, and home status.
Scheduling: Add cleaning schedules via the interface (requires implementing storage in EEPROM or additional logic in the firmware).
Alerts: The buzzer will sound if the storage level exceeds 85% during cleaning.

5. Troubleshooting

WiFi Connection Issues:
Verify SSID and password in the firmware.
Check the Serial Monitor for connection status.


Web Interface Not Responding:
Ensure the ESP8266’s IP address is correct in the HTML file’s JavaScript.
Verify the ESP8266 is powered and connected to WiFi.


Sensor Issues:
Check ultrasonic sensor wiring if storage level readings are incorrect.
Ensure the battery monitoring circuit provides valid analog readings.


Motor Issues:
Confirm motor driver connections and power supply.
Test each motor pin using the web interface’s manual controls.



6. Notes

The RX pin (GPIO 3) is used for the ultrasonic sensor’s echo pin. Be cautious, as this pin is also used for serial communication during programming.
The firmware includes CORS headers to allow cross-origin requests from the web interface.
The auto mode logic is basic and may need enhancement for obstacle avoidance or advanced navigation.
The scheduling feature requires additional implementation for persistent storage (e.g., EEPROM) to store and execute schedules.

For further assistance, refer to the ESP8266 Community Forum or the Arduino Forum.
