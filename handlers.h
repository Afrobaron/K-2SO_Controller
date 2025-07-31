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

// New function declaration for servo control
void handleSetServos(); // Handler for the /setServos endpoint
void moveServo(int servoNum, int angle); // Forward declaration for moveServo

#endif // K2SO_HANDLERS_H
