#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Adafruit_NeoPixel.h>
#include <DFRobotDFPlayerMini.h>
#include <Adafruit_PWMServoDriver.h> // Added for PCA9685

// Include your custom header files
#include "config.h"     // <--- ESSENTIAL: This must be included here
#include "webpage.h"
#include "animations.h"
#include "handlers.h"

// --- Global Variable Definitions (No 'extern' here!) ---
// These are the actual instances of the objects and variables.
// They match the 'extern' declarations in config.h.
Adafruit_NeoPixel pixels(NUM_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

HardwareSerial &mySerial = Serial2; // Define a reference to Serial2

DFRobotDFPlayerMini myDFPlayer;

WebServer server(80); // Web server on port 80

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(PCA9685_I2C_ADDRESS); // Added for PCA9685

uint32_t currentColor = pixels.Color(100, 100, 100); // Initial color (off)
uint8_t currentBrightness = BRIGHTNESS;         // Current pixel brightness
uint8_t currentVolume = 25;                     // Initial DFPlayer volume (0-30)

// Animation state variables
PixelMode currentPixelMode = SOLID_COLOR;
unsigned long animationStartTime = 0;
unsigned long lastAnimationUpdateTime = 0;
uint8_t fadeStartBrightness = 0;
uint32_t fadeStartColor = 0;
uint32_t fadeTargetColor = 0;

// --- FORWARD DECLARATION ---
// This tells the compiler that 'moveServo' exists in handlers.cpp,
// so it can be called before its full definition appears.
void moveServo(int servoNum, int angle);

void setup() {
  Serial.begin(115200);
  while (!Serial); // Wait for Serial Monitor to open

  // Initialize NeoPixel
  pixels.begin();
  setPixelColorAndBrightness(currentColor, currentBrightness); // Set initial color and brightness

  // Initialize DFPlayer Mini
  mySerial.begin(9600, SERIAL_8N1, DFPLAYER_RX_PIN, DFPLAYER_TX_PIN);

  Serial.println("Waiting for DFPlayer to boot...");
  delay(2000); // <--- ADD THIS DELAY: Give the MP3 chip 2 seconds to read the SD card
  
  if (myDFPlayer.begin(mySerial)) {
    Serial.println(F("DFPlayer Mini online."));
    myDFPlayer.volume(currentVolume); // Set initial volume
    myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
    myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
  } else {
    Serial.println(F("DFPlayer Mini not connected or error!"));
  }

// --- WiFi Connection ---
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  Serial.print("Connecting to WiFi");
  
  int attempts = 0;
  // Try to connect for 10 seconds (20 attempts * 500ms)
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  // Check if we successfully connected to the home network
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to home network!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    // If not, spin up the fallback Access Point
    Serial.println("\nHome network not found. Starting Fallback AP...");
    
    WiFi.disconnect();      // Clear the failed station configuration
    WiFi.mode(WIFI_AP);     // Switch ESP32 to Access Point mode
    
    // Start the AP using the credentials from config.h
    WiFi.softAP(AP_SSID, AP_PASS);
    
    Serial.println("Fallback AP is active.");
    Serial.print("Connect to SSID: ");
    Serial.println(AP_SSID);
    Serial.print("Webpage IP: ");
    Serial.println(WiFi.softAPIP()); // This will default to 192.168.4.1
  }
  // --- MDNS Setup ---
  if (!MDNS.begin("k2so")) { // Set custom hostname (e.g., http://k2so.local)
    Serial.println("Error setting up MDNS responder!");
    // No halt here, as MDNS is not critical for basic operation
  } else {
    Serial.println("MDNS responder started (k2so.local)");
  }

  // --- PCA9685 Servo Initialization ---
  Serial.print("Attempting to initialize PCA9685 at address 0x");
  Serial.println(PCA9685_I2C_ADDRESS, HEX);
  Wire.begin(I2C_SDA, I2C_SCL); // Initialize I2C bus with specific pins
  if (!pwm.begin()) {
    Serial.println("ERROR: PCA9685 initialization failed! Check wiring, power, and I2C address.");
    Serial.println("Halting program due to PCA9685 error.");
    while (1); // Halt execution if initialization fails
  }
  Serial.println("PCA9685 Initialized Successfully!");
  pwm.setPWMFreq(SERVO_FREQ);
  Serial.print("PWM Frequency set to: ");
  Serial.print(SERVO_FREQ);
  Serial.println("Hz.");

  // --- Servo Centering on Startup ---
  Serial.println("Centering servos...");
  moveServo(0, ANGLE_CENTER); // Center Servo 0
  moveServo(1, ANGLE_CENTER); // Center Servo 1
  delay(1000); // Give servos time to move

  // --- Web Server Setup ---
  server.on("/", handleRoot);
  server.on("/red", handleRed);
  server.on("/green", handleGreen);
  server.on("/blue", handleBlue);
  server.on("/white", handleWhite);
  server.on("/off", handleOff);
  server.on("/brightness", handleBrightness);
  server.on("/volume", handleVolume);
  server.on("/flicker", handleFlicker);
  server.on("/pulse", handlePulse);
  server.on("/playSound", handlePlaySound);
  // Add new servo control handler
  server.on("/setServos", HTTP_GET, []() {
    Serial.println("DEBUG: /setServos handler triggered.");
    if (server.hasArg("angle0") && server.hasArg("angle1")) {
      int angle0 = server.arg("angle0").toInt();
      int angle1 = server.arg("angle1").toInt();

      if ((angle0 >= ANGLE_MIN && angle0 <= ANGLE_MAX) &&
          (angle1 >= ANGLE_MIN && angle1 <= ANGLE_MAX)) {
        Serial.print("Web Command: Setting Servo 0 to "); Serial.print(angle0);
        Serial.print(" and Servo 1 to "); Serial.print(angle1); Serial.println(" degrees.");
        moveServo(0, angle0);
        moveServo(1, angle1);
        server.send(200, "text/plain", "OK");
      } else {
        Serial.print("DEBUG: Invalid angles received: S0="); Serial.print(angle0);
        Serial.print(", S1="); Serial.println(angle1);
        server.send(400, "text/plain", "Invalid angle values.");
      }
    } else {
      Serial.println("DEBUG: Missing angle parameters for /setServos.");
      server.send(400, "text/plain", "Missing angle parameters.");
    }
  });


  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started.");

  currentPixelMode = SOLID_COLOR; // Ensure mode is solid at start
}

void loop() {
  server.handleClient();
  //MDNS.update(); // Uncomment if you use MDNS
  handlePixelAnimations(); // Call the animation handler
}
