#ifndef K2SO_HANDLERS_H
#define K2SO_HANDLERS_H

// Function declarations for web server handlers
void handleRoot();
void handleRed();
void handleGreen();
void handleBlue();
void handleWhite();
void handleOff();
void handleBrightness();
void handleVolume();
void handleFlicker();
void handlePulse();
void handlePlaySound(); // The single handler for all sounds
void handleNotFound();

#endif // K2SO_HANDLERS_H