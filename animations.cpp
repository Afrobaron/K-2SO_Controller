#include "animations.h" // Include its own header
#include "config.h"     // Include common configurations
#include <math.h>       // For sin() in pulse animation
#include <Adafruit_NeoPixel.h> // <--- ADD THIS LINE: Required for pixels object definition

// Function definitions
void setPixelColorAndBrightness(uint32_t color, uint8_t brightness) {
  pixels.setPixelColor(0, color);
  pixels.setBrightness(brightness);
  pixels.show();
}

void startColorFade(uint32_t targetColor) {
  fadeStartColor = pixels.getPixelColor(0); // Get current color
  
  fadeTargetColor = targetColor;
  animationStartTime = millis();
  currentPixelMode = FADE_COLOR;
  Serial.print("Starting fade from ");
  Serial.print((fadeStartColor >> 16) & 0xFF); Serial.print(",");
  Serial.print((fadeStartColor >> 8) & 0xFF); Serial.print(",");
  Serial.print(fadeStartColor & 0xFF); Serial.print(" to ");
  Serial.print((fadeTargetColor >> 16) & 0xFF); Serial.print(",");
  Serial.print((fadeTargetColor >> 8) & 0xFF); Serial.print(",");
  Serial.print(fadeTargetColor & 0xFF); Serial.println(".");
}

void handlePixelAnimations() {
  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - animationStartTime;

  switch (currentPixelMode) {
    case SOLID_COLOR:
      // Do nothing, pixel is static.
      break;

    case FADE_OFF:
      if (elapsedTime < FADE_DURATION_MS) {
        uint8_t newBrightness = fadeStartBrightness * (1.0 - (float)elapsedTime / FADE_DURATION_MS);
        pixels.setBrightness(newBrightness);
        pixels.show();
      } else {
        pixels.setBrightness(0);
        pixels.setPixelColor(0, pixels.Color(0,0,0)); // Ensure color is black
        pixels.show();
        currentColor = pixels.Color(0,0,0); // Update global currentColor to black
        currentPixelMode = SOLID_COLOR;
        Serial.println("Fade off complete.");
      }
      break;
    
    case FADE_COLOR: { // Use a block for local variables
      if (elapsedTime < FADE_DURATION_MS) {
        float progress = (float)elapsedTime / FADE_DURATION_MS;

        // Extract RGB components for start and target colors
        uint8_t r1 = (fadeStartColor >> 16) & 0xFF;
        uint8_t g1 = (fadeStartColor >> 8) & 0xFF;
        uint8_t b1 = fadeStartColor & 0xFF;

        uint8_t r2 = (fadeTargetColor >> 16) & 0xFF;
        uint8_t g2 = (fadeTargetColor >> 8) & 0xFF;
        uint8_t b2 = fadeTargetColor & 0xFF;

        // Interpolate each component
        uint8_t r_interp = r1 + (r2 - r1) * progress;
        uint8_t g_interp = g1 + (g2 - g1) * progress;
        uint8_t b_interp = b1 + (b2 - b1) * progress;

        // Set the interpolated color with the current brightness
        pixels.setPixelColor(0, pixels.Color(r_interp, g_interp, b_interp));
        pixels.setBrightness(currentBrightness); // Keep brightness constant during color fade
        pixels.show();
      } else {
        // Fade complete: set to target color and switch to SOLID_COLOR
        currentColor = fadeTargetColor; // Update global currentColor to the final color
        pixels.setPixelColor(0, currentColor);
        pixels.setBrightness(currentBrightness);
        pixels.show();
        currentPixelMode = SOLID_COLOR;
        Serial.println("Color fade complete.");
      }
      break;
    }

    case FLICKER:
      if (currentTime - lastAnimationUpdateTime >= FLICKER_UPDATE_INTERVAL_MS) {
        int minFlickerBrightness = max(0, currentBrightness - (currentBrightness / 5));
        int maxFlickerBrightness = min(255, currentBrightness + (currentBrightness / 5));

        uint8_t flickerBrightness = random(minFlickerBrightness, maxFlickerBrightness + 1);
        pixels.setPixelColor(0, currentColor); // Ensure color is base color
        pixels.setBrightness(flickerBrightness);
        pixels.show();
        lastAnimationUpdateTime = currentTime;
      }
      break;

    case PULSE:
      float minPulseBrightness = currentBrightness * (PULSE_MIN_BRIGHTNESS_PERCENTAGE / 100.0);
      float brightnessRange = currentBrightness - minPulseBrightness;

      float phase = (float)(elapsedTime % PULSE_SPEED_MS) / PULSE_SPEED_MS;
      float brightnessFactor = (sin(phase * 2 * M_PI) + 1.0) / 2.0;

      uint8_t newPulseBrightness = minPulseBrightness + (brightnessRange * brightnessFactor);
      
      pixels.setPixelColor(0, currentColor); // Ensure color is base color
      pixels.setBrightness(newPulseBrightness);
      pixels.show();
      break;
  }
}