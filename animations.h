#ifndef K2SO_ANIMATIONS_H
#define K2SO_ANIMATIONS_H

#include <Arduino.h> // For basic types
#include <Adafruit_NeoPixel.h> // For Adafruit_NeoPixel object type
#include "config.h" // For PixelMode enum and animation constants

// Function declarations
void setPixelColorAndBrightness(uint32_t color, uint8_t brightness);
void startColorFade(uint32_t targetColor);
void handlePixelAnimations();

#endif // K2SO_ANIMATIONS_H
