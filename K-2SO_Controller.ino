#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Adafruit_NeoPixel.h>
#include <DFRobotDFPlayerMini.h>
//#include <SoftwareSerial.h> // For DFPlayer

// Include your custom header files
#include "config.h"     // <--- ESSENTIAL: This must be included here
#include "webpage.h"
#include "animations.h"
#include "handlers.h"

// --- Global Variable Definitions (No 'extern' here!) ---
// These are the actual instances of the objects and variables.
// They match the 'extern' declarations in config.h.
Adafruit_NeoPixel pixels(NUM_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

//SoftwareSerial mySoftwareSerial(DFPLAYER_RX_PIN, DFPLAYER_TX_PIN); // RX, TX
HardwareSerial &mySerial = Serial2; // Define a reference to Serial2

DFRobotDFPlayerMini myDFPlayer;

WebServer server(80); // Web server on port 80

uint32_t currentColor = pixels.Color(0, 0, 0); // Initial color (off)
uint8_t currentBrightness = BRIGHTNESS;         // Current pixel brightness
uint8_t currentVolume = 15;                     // Initial DFPlayer volume (0-30)

// Animation state variables
PixelMode currentPixelMode = SOLID_COLOR;
unsigned long animationStartTime = 0;
unsigned long lastAnimationUpdateTime = 0;
uint8_t fadeStartBrightness = 0;
uint32_t fadeStartColor = 0;
uint32_t fadeTargetColor = 0;


void setup() {
  Serial.begin(115200);

  // Initialize NeoPixel
  pixels.begin();
  setPixelColorAndBrightness(currentColor, currentBrightness); // Set initial color and brightness

  // Initialize DFPlayer Mini
  mySerial.begin(9600, SERIAL_8N1, DFPLAYER_RX_PIN, DFPLAYER_TX_PIN);
  if (myDFPlayer.begin(mySerial)) {
    Serial.println(F("DFPlayer Mini online."));
    myDFPlayer.volume(currentVolume); // Set initial volume
    myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
    myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
  } else {
    Serial.println(F("DFPlayer Mini not connected or error!"));
  }

  // --- WiFi Connection ---
  Serial.print("Attempting to connect to WiFi network: ");
  Serial.println(WIFI_SSID); // Use the #define here
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD); // Use the #define here

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // --- MDNS Setup ---
  if (!MDNS.begin("k2so")) { // Set custom hostname (e.g., http://k2so.local)
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("MDNS responder started (k2so.local)");

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
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started.");

  currentPixelMode = SOLID_COLOR; // Ensure mode is solid at start
}

void loop() {
  server.handleClient();
  //MDNS.update();
  handlePixelAnimations(); // Call the animation handler
}