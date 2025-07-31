#ifndef K2SO_WEBPAGE_H
#define K2SO_WEBPAGE_H

#include <Arduino.h> // For String type

// Function to get the main HTML page content
String getIndexPage(uint8_t currentBrightness, uint8_t currentVolume);

#endif // K2SO_WEBPAGE_H
