#ifndef K2SO_CONFIG_H
#define K2SO_CONFIG_H

#include <Arduino.h> // For basic types like uint8_t, boolean, etc.

// --- Library Includes for extern declared types ---
// These are needed here so that any file including config.h knows what these types are.
#include <Adafruit_NeoPixel.h>
#include <DFRobotDFPlayerMini.h>
#include <WebServer.h>
#include <Adafruit_PWMServoDriver.h> // Added for PCA9685

// --- WiFi Configuration ---
// CHANGE THESE TO #DEFINE TO AVOID MULTIPLE DEFINITION ERRORS
#define WIFI_SSID         "YOUR_SSID"         // REPLACE WITH YOUR WIFI NETWORK NAME
#define WIFI_PASSWORD     "YOUR_PASSWORD"     // REPLACE WITH YOUR WIFI NETWORK PASSWORD

// --- NeoPixel Configuration ---
#define LED_PIN    13        // Data pin for your NeoPixel (connect to level shifter input)
#define NUM_PIXELS 1         // We only have one NeoPixel
#define BRIGHTNESS 50        // Default NeoPixel brightness (0-255)

// --- DFPlayer Mini Pins ---
#define DFPLAYER_RX_PIN 26   // Connect to DFPlayer TX
#define DFPLAYER_TX_PIN 27   // Connect to DFPlayer RX

// --- PCA9685 Servo Configuration ---
#define I2C_SDA 22           // SDA (Data Line) for ESP32
#define I2C_SCL 21           // SCL (Clock Line) for ESP32
#define PCA9685_I2C_ADDRESS 0x40 // PCA9685 I2C Address

#define SERVO1_CHANNEL 0     // PCA9685 channel for Servo 0 (your original Servo 1)
#define SERVO2_CHANNEL 1     // PCA9685 channel for Servo 1 (your original Servo 2)
#define SERVO_FREQ 50        // PWM frequency for most analog servos

// Calibrated pulse length values for your 270-degree servos.
// These values will need to determined by each setup. 
// I set my 270 degree servos to 0 degrees, lined up the actuator to 
// the min position then pushed it onto the servo gear.  
// For me the physical limits were:
//    Servo 0 - Min 0 degrees, max 45 degrees
//    Servo 1 - Min 5 degrees, max 40 degrees

#define SERVO0_MIN_PULSE 100     // Servo 0: Corresponds to 0 degrees
#define SERVO0_MAX_PULSE 200     // Servo 0: Corresponds to 45 degrees

#define SERVO1_MIN_PULSE 111     // Servo 1: Corresponds to 5 degrees
#define SERVO1_MAX_PULSE 188     // Servo 1: Corresponds to 40 degrees

// Define angle constants for clarity
#define ANGLE_MIN    0
#define ANGLE_CENTER 90
#define ANGLE_MAX    270

// --- Animation Durations & Parameters ---
#define FADE_DURATION_MS            1000     // Fade off duration in milliseconds (also used for color fades)
#define PULSE_MIN_BRIGHTNESS_PERCENTAGE 50   // Min brightness for pulse (50% of current max)
#define PULSE_SPEED_MS              3000     // Time for one full pulse cycle (dim to bright to dim)
#define FLICKER_UPDATE_INTERVAL_MS  50       // How often to update flicker brightness

// --- Pixel Animation Modes Enum ---
// Declared here so all files can use it
enum PixelMode {
  SOLID_COLOR,
  FADE_OFF,
  FADE_COLOR,
  FLICKER,
  PULSE
};

// --- External Declarations for Global Variables ---
// These variables are defined in K2SO_Controller.ino, but used in other .cpp files.
// The 'extern' keyword tells the compiler they exist elsewhere.
extern Adafruit_NeoPixel pixels;
extern DFRobotDFPlayerMini myDFPlayer;
extern WebServer server;
extern Adafruit_PWMServoDriver pwm; // Added for PCA9685

extern uint32_t currentColor;
extern uint8_t currentBrightness;
extern uint8_t currentVolume;
extern PixelMode currentPixelMode;
extern unsigned long animationStartTime;
extern unsigned long lastAnimationUpdateTime;
extern uint8_t fadeStartBrightness;
extern uint32_t fadeStartColor;
extern uint32_t fadeTargetColor;

#endif // K2SO_CONFIG_H
