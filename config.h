#ifndef K2SO_CONFIG_H
#define K2SO_CONFIG_H

#include <Arduino.h> // For basic types like uint8_t, boolean, etc.

// --- Library Includes for extern declared types ---
// These are needed here so that any file including config.h knows what these types are.
#include <Adafruit_NeoPixel.h>
#include <DFRobotDFPlayerMini.h>
#include <WebServer.h>

// --- WiFi Configuration ---
// CHANGE THESE TO #DEFINE TO AVOID MULTIPLE DEFINITION ERRORS
#define WIFI_SSID         "YOUR_WIFI_SSID"         // REPLACE WITH YOUR WIFI NETWORK NAME
#define WIFI_PASSWORD     "YOUR_WIFI_PASSWORD"     // REPLACE WITH YOUR WIFI NETWORK PASSWORD

// --- NeoPixel Configuration ---
#define LED_PIN    13  // Data pin for your NeoPixel (connect to level shifter input)
#define NUM_PIXELS 1   // We only have one NeoPixel
#define BRIGHTNESS 50  // Default NeoPixel brightness (0-255)

// --- DFPlayer Mini Pins ---
#define DFPLAYER_RX_PIN 26 // Connect to DFPlayer TX
#define DFPLAYER_TX_PIN 27 // Connect to DFPlayer RX

// --- Animation Durations & Parameters ---
#define FADE_DURATION_MS            1000 // Fade off duration in milliseconds (also used for color fades)
#define PULSE_MIN_BRIGHTNESS_PERCENTAGE 50   // Min brightness for pulse (50% of current max)
#define PULSE_SPEED_MS              3000 // Time for one full pulse cycle (dim to bright to dim)
#define FLICKER_UPDATE_INTERVAL_MS  50   // How often to update flicker brightness

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