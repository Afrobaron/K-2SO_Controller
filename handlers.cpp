#include "handlers.h"   // Include its own header
#include "config.h"     // Include common configurations
#include "animations.h" // For startColorFade, setPixelColorAndBrightness
#include "webpage.h"    // <--- ADD THIS LINE: Required for getIndexPage()
#include <WebServer.h>  // <--- ADD THIS LINE: Required for WebServer object
#include <Adafruit_NeoPixel.h> // <--- ADD THIS LINE: Required for pixels object definition
#include <DFRobotDFPlayerMini.h> // <--- ADD THIS LINE: Required for myDFPlayer object definition

// Web Server Handler Definitions
void handleRoot() {
  Serial.println("Received request for root page (/). Sending HTML.");
  // Call getIndexPage from webpage.h and pass required global variables
  server.send(200, "text/html", getIndexPage(currentBrightness, currentVolume));
}

void handleRed() {
  Serial.println("Received AJAX request for /red. Initiating fade to Red.");
  startColorFade(pixels.Color(255, 0, 0));
  server.send(200, "text/plain", "OK");
}

void handleGreen() {
  Serial.println("Received AJAX request for /green. Initiating fade to Green.");
  startColorFade(pixels.Color(0, 255, 0));
  server.send(200, "text/plain", "OK");
}

void handleBlue() {
  Serial.println("Received AJAX request for /blue. Initiating fade to Blue.");
  startColorFade(pixels.Color(0, 0, 255));
  server.send(200, "text/plain", "OK");
}

void handleWhite() {
  Serial.println("Received AJAX request for /white. Initiating fade to White.");
  startColorFade(pixels.Color(255, 255, 255));
  server.send(200, "text/plain", "OK");
}

void handleOff() {
  Serial.println("Received AJAX request for /off. Initiating fade to off.");
  currentPixelMode = FADE_OFF;
  animationStartTime = millis();
  fadeStartBrightness = pixels.getBrightness(); // Store current brightness to fade from
  setPixelColorAndBrightness(currentColor, pixels.getBrightness()); 
  server.send(200, "text/plain", "OK");
}

void handleBrightness() {
  if (server.hasArg("value")) {
    int brightness = server.arg("value").toInt();
    if (brightness >= 0 && brightness <= 255) {
      currentBrightness = (uint8_t)brightness;
      currentPixelMode = SOLID_COLOR; // Direct brightness change stops animation
      setPixelColorAndBrightness(currentColor, currentBrightness); // Apply new brightness to current color
      Serial.print("Received AJAX request for /brightness. Setting brightness to: ");
      Serial.println(currentBrightness);
    } else {
      Serial.println("Invalid brightness value received.");
    }
  } else {
    Serial.println("Brightness request received without 'value' argument.");
  }
  server.send(200, "text/plain", "OK");
}

void handleVolume() {
  if (server.hasArg("value")) {
    int volume = server.arg("value").toInt();
    if (volume >= 0 && volume <= 30) { // DFPlayer volume range
      currentVolume = (uint8_t)volume;
      if (myDFPlayer.available()) {
        myDFPlayer.volume(currentVolume);
        Serial.print("Received AJAX request for /volume. Setting volume to: ");
        Serial.println(currentVolume);
      } else {
        Serial.println("DFPlayer not available. Cannot set volume.");
      }
    } else {
      Serial.println("Invalid volume value received.");
    }
  } else {
    Serial.println("Volume request received without 'value' argument.");
  }
  server.send(200, "text/plain", "OK");
}

void handleFlicker() {
  Serial.println("Received AJAX request for /flicker. Activating Flicker mode.");
  currentPixelMode = FLICKER;
  animationStartTime = millis(); // Reset animation timer
  lastAnimationUpdateTime = millis(); // Reset update timer for flicker
  server.send(200, "text/plain", "OK");
}

void handlePulse() {
  Serial.println("Received AJAX request for /pulse. Activating Pulse mode.");
  currentPixelMode = PULSE;
  animationStartTime = millis(); // Reset animation timer
  server.send(200, "text/plain", "OK");
}

void handlePlaySound() {
  if (server.hasArg("file")) {
    int fileNum = server.arg("file").toInt();
    if (fileNum >= 1 && fileNum <= 255) { // Assuming sound files 0001.mp3 to 0255.mp3 are possible
      if (myDFPlayer.available()) {
        myDFPlayer.play(fileNum);
        Serial.print("Playing sound file: ");
        Serial.println(fileNum);
        server.send(200, "text/plain", "OK");
      } else {
        Serial.println("DFPlayer not ready or not found. Cannot play sound.");
        server.send(500, "text/plain", "DFPlayer Error: Not Available");
      }
    } else {
      Serial.print("Invalid sound file number requested: ");
      Serial.println(fileNum);
      server.send(400, "text/plain", "Bad Request: Invalid File Number");
    }
  } else {
    Serial.println("Play sound request received without 'file' argument.");
    server.send(400, "text/plain", "Bad Request: Missing 'file' argument");
  }
}

void handleNotFound() {
  Serial.print("Received request for unknown URL: ");
  Serial.println(server.uri());
  server.send(404, "text/plain", "Not Found");
}