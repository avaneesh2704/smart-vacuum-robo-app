#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

// ==================== CONFIGURATION ====================
// WiFi Settings - CHANGE THESE!
const char* ssid = "YOUR_WIFI_NAME";        // Replace with your WiFi name
const char* password = "YOUR_WIFI_PASSWORD"; // Replace with your WiFi password

// Pin Definitions (matching your hardware)
#define MOTOR_LEFT_A 5   // D1 - Left motor forward
#define MOTOR_LEFT_B 4   // D2 - Left motor backward  
#define MOTOR_RIGHT_A 0  // D3 - Right motor forward
#define MOTOR_RIGHT_B 2  // D4 - Right motor backward
#define VACUUM_MOTOR 14  // D5 - Vacuum motor control
#define STATUS_LED 12    // D6 - Status LED
#define BUZZER_PIN 13    // D7 - Buzzer
#define TRIGGER_PIN 15   // D8 - Ultrasonic trigger
#define ECHO_PIN 13      // D7 - Ultrasonic echo (shared with buzzer)
#define BATTERY_PIN A0   // A0 - Battery voltage monitor

// ==================== GLOBAL VARIABLES ====================
ESP8266WebServer server(80);

// Robot state
bool isCleaningMode = false;
bool isAutoMode = false;
int batteryLevel = 100;
int storageLevel = 0;
String robotStatus = "idle";
unsigned long lastSensorUpdate = 0;
unsigned long cleaningStartTime = 0;

// ==================== UTILITY FUNCTIONS ====================

// Read ultrasonic distance
int readDistance() {
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
  
  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // 30ms timeout
  if (duration == 0) return -1; // No echo received
  
  int distance = duration * 0.034 / 2; // Convert to cm
  return distance;
}

// Update sensor readings
void updateSensors() {
  if (millis() - lastSensorUpdate < 1000) return; // Update every second
  
  // Read battery level (assuming voltage divider)
  int batteryReading = analogRead(BATTERY_PIN);
  batteryLevel = map(batteryReading, 0, 1024, 0, 100);
  batteryLevel = constrain(batteryLevel, 0, 100);
  
  // Read storage level using ultrasonic sensor
  int distance = readDistance();
  if (distance > 0 && distance < 20) {
    // Convert distance to storage percentage (2cm = full, 20cm = empty)
    storageLevel = map(distance, 2, 20, 100, 0);
    storageLevel = constrain(storageLevel, 0, 100);
  }
  
  // Storage full alert
  if (storageLevel >= 90 && isCleaningMode) {
    // Beep pattern for full storage
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_PIN, LOW);
    delay(100);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_PIN, LOW);
  }
  
  // Low battery alert
  if (batteryLevel <= 20) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(50);
    digitalWrite(BUZZER_PIN, LOW);
  }
  
  lastSensorUpdate = millis();
}

// ==================== MOTOR CONTROL FUNCTIONS ====================

void moveForward() {
  digitalWrite(MOTOR_LEFT_A, HIGH);
  digitalWrite(MOTOR_LEFT_B, LOW);
  digitalWrite(MOTOR_RIGHT_A, HIGH);
  digitalWrite(MOTOR_RIGHT_B, LOW);
  robotStatus = "moving_forward";
  Serial.println("Moving Forward");
}

void moveBackward() {
  digitalWrite(MOTOR_LEFT_A, LOW);
  digitalWrite(MOTOR_LEFT_B, HIGH);
  digitalWrite(MOTOR_RIGHT_A, LOW);
  digitalWrite(MOTOR_RIGHT_B, HIGH);
  robotStatus = "moving_backward";
  Serial.println("Moving Backward");
}

void turnLeft() {
  digitalWrite(MOTOR_LEFT_A, LOW);
  digitalWrite(MOTOR_LEFT_B, HIGH);
  digitalWrite(MOTOR_RIGHT_A, HIGH);
  digitalWrite(MOTOR_RIGHT_B, LOW);
  robotStatus = "turning_left";
  Serial.println("Turning Left");
}

void turnRight() {
  digitalWrite(MOTOR_LEFT_A, HIGH);
  digitalWrite(MOTOR_LEFT_B, LOW);
  digitalWrite(MOTOR_RIGHT_A, LOW);
  digitalWrite(MOTOR_RIGHT_B, HIGH);
  robotStatus = "turning_right";
  Serial.println("Turning Right");
}

void stopMotors() {
  digitalWrite(MOTOR_LEFT_A, LOW);
  digitalWrite(MOTOR_LEFT_B, LOW);
  digitalWrite(MOTOR_RIGHT_A, LOW);
  digitalWrite(MOTOR_RIGHT_B, LOW);
  robotStatus = "stopped";
  Serial.println("Motors Stopped");
}

void startVacuum() {
  digitalWrite(VACUUM_MOTOR, HIGH);
  isCleaningMode = true;
  robotStatus = "cleaning";
  cleaningStartTime = millis();
  Serial.println("Vacuum Started");
}

void stopVacuum() {
  digitalWrite(VACUUM_MOTOR, LOW);
  isCleaningMode = false;
  robotStatus = "idle";
  Serial.println("Vacuum Stopped");
}

// ==================== WEB SERVER HANDLERS ====================

void handleCORS() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
}

void handleRoot() {
  handleCORS();
  server.send(200, "text/plain", "ESP8266 Vacuum Robot Controller\nStatus: " + robotStatus);
}

void handleForward() {
  handleCORS();
  moveForward();
  delay(200); // Move for 200ms
  stopMotors();
  server.send(200, "text/plain", "OK");
}

void handleBackward() {
  handleCORS();
  moveBackward();
  delay(200);
  stopMotors();
  server.send(200, "text/plain", "OK");
}

void handleLeft() {
  handleCORS();
  turnLeft();
  delay(150); // Turn for 150ms
  stopMotors();
  server.send(200, "text/plain", "OK");
}

void handleRight() {
  handleCORS();
  turnRight();
  delay(150);
  stopMotors();
  server.send(200, "text/plain", "OK");
}

void handleStop() {
  handleCORS();
  stopMotors();
  server.send(200, "text/plain", "OK");
}

void handleStartCleaning() {
  handleCORS();
  startVacuum();
  server.send(200, "text/plain", "Cleaning started");
}

void handleStopCleaning() {
  handleCORS();
  stopVacuum();
  server.send(200, "text/plain", "Cleaning stopped");
}

void handleReturnHome() {
  handleCORS();
  stopVacuum();
  robotStatus = "returning_home";
  // Simple return home: move backward for 2 seconds
  moveBackward();
  delay(2000);
  stopMotors();
  robotStatus = "home";
  server.send(200, "text/plain", "Returning to base");
}

void handleAutoMode() {
  handleCORS();
  isAutoMode = !isAutoMode;
  if (isAutoMode) {
    startVacuum();
    server.send(200, "text/plain", "Auto mode enabled");
  } else {
    stopVacuum();
    server.send(200, "text/plain", "Auto mode disabled");
  }
}

void handleStatus() {
  handleCORS();
  
  // Create JSON response
  StaticJsonDocument<300> doc;
  doc["battery"] = batteryLevel;
  doc["storage"] = storageLevel;
  doc["status"] = robotStatus;
  doc["cleaning"] = isCleaningMode;
  doc["auto_mode"] = isAutoMode;
  doc["uptime"] = millis() / 1000; // uptime in seconds
  doc["wifi_strength"] = WiFi.RSSI();
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  server.send(200, "application/json", jsonString);
}

// Handle preflight requests
void handleOptions() {
  handleCORS();
  server.send(200);
}

// ==================== AUTO MODE LOGIC ====================
void runAutoMode() {
  if (!isAutoMode || !isCleaningMode) return;
  
  static unsigned long lastAutoMove = 0;
  static int autoMovePhase = 0;
  
  if (millis() - lastAutoMove < 3000) return; // Change direction every 3 seconds
  
  // Simple auto cleaning pattern
  switch (autoMovePhase % 4) {
    case 0:
      moveForward();
      delay(1000);
      break;
    case 1:
      turnRight();
      delay(300);
      break;
    case 2:
      moveForward();
      delay(1000);
      break;
    case 3:
      turnLeft();
      delay(300);
      break;
  }
  
  stopMotors();
  autoMovePhase++;
  lastAutoMove = millis();
  
  // Stop auto mode if battery low or storage full
  if (batteryLevel < 20 || storageLevel > 95) {
    isAutoMode = false;
    handleReturnHome();
  }
}

// ==================== SETUP FUNCTION ====================
void setup() {
  Serial.begin(115200);
  Serial.println("\n=== ESP8266 Vacuum Robot Starting ===");
  
  // Initialize pins
  pinMode(MOTOR_LEFT_A, OUTPUT);
  pinMode(MOTOR_LEFT_B, OUTPUT);
  pinMode(MOTOR_RIGHT_A, OUTPUT);
  pinMode(MOTOR_RIGHT_B, OUTPUT);
  pinMode(VACUUM_MOTOR, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  // Initialize all outputs to OFF
  digitalWrite(MOTOR_LEFT_A, LOW);
  digitalWrite(MOTOR_LEFT_B, LOW);
  digitalWrite(MOTOR_RIGHT_A, LOW);
  digitalWrite(MOTOR_RIGHT_B, LOW);
  digitalWrite(VACUUM_MOTOR, LOW);
  digitalWrite(STATUS_LED, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  
  Serial.println("Hardware initialized");
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  
  // Blink LED while connecting
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    digitalWrite(STATUS_LED, !digitalRead(STATUS_LED));
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("WiFi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Signal Strength: ");
    Serial.println(WiFi.RSSI());
    
    // Keep LED on when connected
    digitalWrite(STATUS_LED, HIGH);
    
    // Success beep
    digitalWrite(BUZZER_PIN, HIGH);
    delay(200);
    digitalWrite(BUZZER_PIN, LOW);
  } else {
    Serial.println();
    Serial.println("WiFi Connection Failed!");
    // Error beep pattern
    for (int i = 0; i < 3; i++) {
      digitalWrite(BUZZER_PIN, HIGH);
      delay(100);
      digitalWrite(BUZZER_PIN, LOW);
      delay(100);
    }
  }
  
  // Setup web server routes
  server.on("/", handleRoot);
  server.on("/forward", HTTP_GET, handleForward);
  server.on("/backward", HTTP_GET, handleBackward);
  server.on("/left", HTTP_GET, handleLeft);
  server.on("/right", HTTP_GET, handleRight);
  server.on("/stop", HTTP_GET, handleStop);
  server.on("/start_cleaning", HTTP_GET, handleStartCleaning);
  server.on("/stop_cleaning", HTTP_GET, handleStopCleaning);
  server.on("/return_home", HTTP_GET, handleReturnHome);
  server.on("/enable_auto_mode", HTTP_GET, []() {
    isAutoMode = true;
    handleAutoMode();
  });
  server.on("/disable_auto_mode", HTTP_GET, []() {
    isAutoMode = false;
    handleAutoMode();
  });
  server.on("/status", HTTP_GET, handleStatus);
  
  // Handle CORS preflight
  server.onNotFound([]() {
    if (server.method() == HTTP_OPTIONS) {
      handleOptions();
    } else {
      handleCORS();
      server.send(404, "text/plain", "Not Found");
    }
  });
  
  // Start web server
  server.begin();
  Serial.println("Web server started!");
  Serial.println("=== Setup Complete ===");
  
  // Initial sensor reading
  updateSensors();
}

// ==================== MAIN LOOP ====================
void loop() {
  // Handle web requests
  server.handleClient();
  
  // Update sensors
  updateSensors();
  
  // Run auto mode if enabled
  runAutoMode();
  
  // Heartbeat LED blink (every 2 seconds)
  static unsigned long lastHeartbeat = 0;
  if (millis() - lastHeartbeat > 2000) {
    if (WiFi.status() == WL_CONNECTED) {
      // Quick blink if connected
      digitalWrite(STATUS_LED, LOW);
      delay(50);
      digitalWrite(STATUS_LED, HIGH);
    } else {
      // Slow blink if disconnected
      digitalWrite(STATUS_LED, !digitalRead(STATUS_LED));
    }
    lastHeartbeat = millis();
  }
  
  // Small delay to prevent watchdog reset
  delay(10);
}
