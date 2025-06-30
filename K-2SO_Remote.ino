#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_NeoPixel.h>
#include <DFRobotDFPlayerMini.h>
#include <math.h> // Required for sin() function in pulse animation

// --- WiFi Configuration ---
const char* ssid = "YOUR_WIFI_SSID";         // REPLACE WITH YOUR WIFI NETWORK NAME
const char* password = "YOUR_WIFI_PASSWORD"; // REPLACE WITH YOUR WIFI NETWORK PASSWORD

// --- NeoPixel Configuration ---
#define LED_PIN    13  // Data pin for your NeoPixel (connect to level shifter input)
#define NUM_PIXELS 1   // We only have one NeoPixel
#define BRIGHTNESS 50  // Default NeoPixel brightness (0-255)
Adafruit_NeoPixel pixels(NUM_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

// --- DFPlayer Mini Configuration ---
HardwareSerial mySerial(2);
DFRobotDFPlayerMini myDFPlayer;
uint8_t currentVolume = 20; // Default DFPlayer volume (0-30)

// --- Web Server Setup ---
WebServer server(80); // Create a web server object that listens on port 80 (standard HTTP)

// --- Global State Variables ---
uint32_t currentColor = pixels.Color(0, 0, 0); // Current pixel color (default: Off)
uint8_t currentBrightness = BRIGHTNESS;       // Current pixel brightness

// --- Pixel Animation Modes ---
enum PixelMode {
  SOLID_COLOR,
  FADE_OFF,
  FADE_COLOR,
  FLICKER,
  PULSE
};
PixelMode currentPixelMode = SOLID_COLOR;
unsigned long animationStartTime = 0; // To track when the current animation started
unsigned long lastAnimationUpdateTime = 0; // To control animation frame rate (for flicker)
uint8_t fadeStartBrightness = 0; // To store brightness when fade starts/ends
uint32_t fadeStartColor = 0; // To store color when fade starts
uint32_t fadeTargetColor = 0; // To store target color for fade

// --- Animation Durations & Parameters ---
#define FADE_DURATION_MS            1000 // Fade off duration in milliseconds (also used for color fades)
#define PULSE_MIN_BRIGHTNESS_PERCENTAGE 50   // Min brightness for pulse (50% of current max)
#define PULSE_SPEED_MS              3000 // Time for one full pulse cycle (dim to bright to dim)
#define FLICKER_UPDATE_INTERVAL_MS  50   // How often to update flicker brightness

// --- HTML Page Content ---
String getIndexPage() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>K-2SO</title>
    <style>
        body { font-family: Arial, Helvetica, Sans-Serif; text-align: center; background-color: #222; color: #FFF; margin-top: 50px; }
        .container { max-width: 400px; margin: auto; padding: 20px; border: 1px solid #444; border-radius: 8px; background-color: #333; box-shadow: 0 0 10px rgba(0,0,0,0.5); }
        h1, h2 { color: #FFF; }
        h2 { margin-top: 20px; }

        /* General Button Styling */
        .button-group button, .vertical-button-group button, .extra-controls-group button {
            margin: 10px 5px;
            padding: 12px 0;
            font-size: 18px;
            border: none;
            border-radius: 5px;
            cursor: pointer;
            color: white;
            transition: background-color 0.3s ease;
        }

        /* Square Color Buttons */
        .color-button-group {
            display: flex;
            justify-content: center;
            flex-wrap: wrap;
            gap: 10px;
            margin-bottom: 20px;
        }
        .color-button-group button {
            width: 42px;
            height: 42px;
            line-height: 18px;
            padding: 12px 0;
        }
        button.red { background-color: #FF0000; }
        button.red:hover { background-color: #CC0000; }
        button.green { background-color: #00FF00; }
        button.green:hover { background-color: #00CC00; }
        button.blue { background-color: #0000FF; }
        button.blue:hover { background-color: #0000CC; }
        button.white { background-color: #DDDDDD; color: #333; }
        button.white:hover { background-color: #BBBBBB; }
        button.off { background-color: #555555; }
        button.off:hover { background-color: #333333; }

        /* Extra Pixel Control Buttons (new row) - Match Sound Button Style */
        .extra-controls-group {
            display: flex;
            justify-content: center;
            flex-wrap: wrap;
            gap: 10px;
            margin-top: 10px;
            margin-bottom: 20px;
        }
        .extra-controls-group button {
            background-color: #000000;
            min-width: 120px;
            padding: 12px 25px;
        }
        .extra-controls-group button:hover {
            background-color: #333333;
        }

        /* Sound Buttons (Vertical Stack) */
        .vertical-button-group {
            display: flex;
            flex-direction: column;
            align-items: center;
            gap: 8px; /* Slightly reduced gap for more buttons */
        }
        .vertical-button-group button {
            width: 80%;
            max-width: 250px;
            padding: 12px 25px;
        }
        button.sound-button { background-color: #000000; }
        button.sound-button:hover { background-color: #333333; }

        /* Slider Specific Styling */
        .slider-container { margin-top: 30px; margin-bottom: 20px; }
        label { display: block; margin-bottom: 10px; font-size: 1.1em; }
        input[type="range"] {
            width: 80%;
            -webkit-appearance: none;
            height: 10px;
            border-radius: 5px;
            background: #555;
            outline: none;
            opacity: 0.7;
            transition: opacity .2s;
        }
        /* All Slider Thumbs to White */
        input[type="range"]::-webkit-slider-thumb {
            -webkit-appearance: none;
            appearance: none;
            width: 25px;
            height: 25px;
            border-radius: 50%;
            background: #FFFFFF; /* White knob */
            cursor: pointer;
        }
        input[type="range"]::-moz-range-thumb {
            width: 25px;
            height: 25px;
            border-radius: 50%;
            background: #FFFFFF; /* White knob */
            cursor: pointer;
        }

        #statusMessage { margin-top: 20px; color: #FFD700; font-weight: bold; }
    </style>
</head>
<body>
    <div class="container">
        <h1>K-2SO</h1>

        <h2>Eye Control</h2>
        <div class="color-button-group">
            <button onclick="setColor('red')" class="red color-button">R</button>
            <button onclick="setColor('green')" class="green color-button">G</button>
            <button onclick="setColor('blue')" class="blue color-button">B</button>
            <button onclick="setColor('white')" class="white color-button">W</button>
            <button onclick="setColor('off')" class="off color-button">O</button>
        </div>

        <div class="extra-controls-group">
            <button onclick="setMode('flicker')" class="extra-control-button">Flicker</button>
            <button onclick="setMode('pulse')" class="extra-control-button">Pulse</button>
        </div>

        <div class="slider-container">
            <label for="brightnessSlider">Brightness</label>
            <input type="range" id="brightnessSlider" name="value" min="0" max="255" value=")rawliteral";
  html += String(currentBrightness);
  html += R"rawliteral(" onchange="setBrightness(this.value)">
        </div>

        <div class="slider-container">
            <label for="volumeSlider">Volume</label>
            <input type="range" id="volumeSlider" name="value" min="0" max="30" value=")rawliteral";
  html += String(currentVolume);
  html += R"rawliteral(" class="volume-slider" onchange="setVolume(this.value)">
        </div>

        <h2>Sound Controls</h2>
        <div class="vertical-button-group">
            <button onclick="playSound(1)" class="sound-button">Stand Back</button>
            <button onclick="playSound(2)" class="sound-button">Business</button>
            <button onclick="playSound(3)" class="sound-button">Vague</button>
            <button onclick="playSound(4)" class="sound-button">Blaster</button>
            <button onclick="playSound(5)" class="sound-button">Friend</button>
            <button onclick="playSound(6)" class="sound-button">Fresh One</button>
            <button onclick="playSound(7)" class="sound-button">About Time</button>
            <button onclick="playSound(8)" class="sound-button">Its High</button>
            <button onclick="playSound(9)" class="sound-button">Its Very High</button>
            <button onclick="playSound(10)" class="sound-button">Resist</button>
        </div>
        
        <div id="statusMessage"></div>

    </div> <script>
        const brightnessSlider = document.getElementById('brightnessSlider');
        const volumeSlider = document.getElementById('volumeSlider');
        const statusMessage = document.getElementById('statusMessage');

        // Function to update status message
        function showStatus(message, isError = false) {
            statusMessage.textContent = message;
            statusMessage.style.color = isError ? '#FF6347' : '#FFD700';
            setTimeout(() => { statusMessage.textContent = ''; }, 3000); // Clear message after 3 seconds
        }

        // Generic function for setting pixel modes (Flicker, Pulse)
        function setMode(mode) {
            const url = '/' + mode;
            showStatus(`Setting mode to ${mode}...`);
            fetch(url)
                .then(response => {
                    if (response.ok) {
                        console.log(`Set mode to ${mode} successfully.`);
                        showStatus(`Mode set to ${mode}.`);
                    } else {
                        console.error(`Failed to set mode to ${mode}. Status: ${response.status}`);
                        showStatus(`Error setting mode: ${response.status}`, true);
                    }
                })
                .catch(error => {
                    console.error('Network error:', error);
                    showStatus('Network error. Check connection.', true);
                });
        }

        // Modified setColor to trigger a fade
        function setColor(color) {
            const url = '/' + color;
            showStatus(`Initiating fade to ${color}...`);
            fetch(url)
                .then(response => {
                    if (response.ok) {
                        console.log(`Fade to ${color} initiated successfully.`);
                    } else {
                        console.error(`Failed to initiate fade to ${color}. Status: ${response.status}`);
                        showStatus(`Error initiating fade: ${response.status}`, true);
                    }
                })
                .catch(error => {
                    console.error('Network error:', error);
                    showStatus('Network error. Check connection.', true);
                });
        }

        function setBrightness(value) {
            const url = '/brightness?value=' + value;
            showStatus(`Setting brightness...`); // Simplified message
            fetch(url)
                .then(response => {
                    if (response.ok) {
                        console.log(`Set brightness to ${value} successfully.`);
                        showStatus(`Brightness updated.`); // Simplified message
                    } else {
                        console.error(`Failed to set brightness to ${value}. Status: ${response.status}`);
                        showStatus(`Error setting brightness: ${response.status}`, true);
                    }
                })
                .catch(error => {
                    console.error('Network error:', error);
                    showStatus('Network error. Check connection.', true);
                });
        }

        // Function for setting volume
        function setVolume(value) {
            const url = '/volume?value=' + value;
            showStatus(`Setting volume...`); // Simplified message
            fetch(url)
                .then(response => {
                    if (response.ok) {
                        console.log(`Set volume to ${value} successfully.`);
                        showStatus(`Volume updated.`); // Simplified message
                    } else {
                        console.error(`Failed to set volume to ${value}. Status: ${response.status}`);
                        showStatus(`Error setting volume: ${response.status}`, true);
                    }
                })
                .catch(error => {
                    console.error('Network error:', error);
                    showStatus('Network error. Check connection.', true);
                });
        }

        function playSound(fileNum) {
            // *** IMPORTANT CHANGE HERE ***
            // We now send the file number as a query parameter
            const url = '/play_sound?file=' + fileNum;
            showStatus(`Playing sound ${fileNum}...`);
            fetch(url)
                .then(response => {
                    if (response.ok) {
                        console.log(`Playing sound ${fileNum} successfully.`);
                        showStatus(`Playing sound ${fileNum}.`);
                    } else {
                        console.error(`Failed to play sound ${fileNum}. Status: ${response.status}`);
                        showStatus(`Error playing sound: ${response.status}`, true);
                    }
                })
                .catch(error => {
                    console.error('Network error:', error);
                    showStatus('Network error. Check connection.', true);
                });
        }
    </script>
</body>
</html>
)rawliteral";
  return html;
}

// --- Helper function to set NeoPixel color and update ---
void setPixelColorAndBrightness(uint32_t color, uint8_t brightness) {
  pixels.setPixelColor(0, color);
  pixels.setBrightness(brightness);
  pixels.show();
}


// --- Web Server Handlers ---
void handleRoot() {
  Serial.println("Received request for root page (/). Sending HTML.");
  server.send(200, "text/html", getIndexPage());
}

// Helper to start a color fade
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

// Handler for Volume
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

// *** NEW: Single Handler for Playing Sounds ***
void handlePlaySound() {
  if (server.hasArg("file")) {
    int fileNum = server.arg("file").toInt();
    // Validate file number - DFPlayer supports 1-255 files directly
    if (fileNum >= 1 && fileNum <= 255) { // Assuming sound files 0001.mp3 to 0255.mp3 are possible
      if (myDFPlayer.available()) {
        myDFPlayer.play(fileNum);
        Serial.print("Playing sound file: ");
        Serial.println(fileNum);
        server.send(200, "text/plain", "OK"); // Send OK response
      } else {
        Serial.println("DFPlayer not ready or not found. Cannot play sound.");
        server.send(500, "text/plain", "DFPlayer Error: Not Available"); // Internal server error
      }
    } else {
      Serial.print("Invalid sound file number requested: ");
      Serial.println(fileNum);
      server.send(400, "text/plain", "Bad Request: Invalid File Number"); // Bad request
    }
  } else {
    Serial.println("Play sound request received without 'file' argument.");
    server.send(400, "text/plain", "Bad Request: Missing 'file' argument"); // Bad request
  }
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

void handleNotFound() {
  Serial.print("Received request for unknown URL: ");
  Serial.println(server.uri());
  server.send(404, "text/plain", "Not Found");
}

// --- Animation Logic (called in loop) ---
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

// --- Setup Function ---
void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(0));
  while (!Serial) {;}

  Serial.println("\n--- ESP32 NeoPixel & Sound Web Control Startup ---");

  // --- WiFi Connection ---
  Serial.print("Attempting to connect to WiFi network: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  unsigned long connectStartTime = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - connectStartTime < 30000)) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected successfully!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFailed to connect to WiFi. Please check SSID and Password.");
    Serial.println("Restarting ESP32 in 5 seconds...");
    delay(5000);
    ESP.restart();
  }

  // --- NeoPixel Initialization ---
  Serial.println("Initializing NeoPixel library...");
  pixels.begin();
  pixels.setBrightness(currentBrightness);
  setPixelColorAndBrightness(pixels.Color(0, 0, 0), 0); // Turn off NeoPixel initially and set global currentColor to black
  currentPixelMode = SOLID_COLOR; // Ensure mode is solid at start
  Serial.println("NeoPixel library initialized. Pixel is OFF.");

  // --- DFPlayer Mini Initialization ---
  Serial.println("Initializing DFPlayer Mini on Serial2 (GPIO26/27)...");
  mySerial.begin(9600, SERIAL_8N1, 26, 27);

  if (!myDFPlayer.begin(mySerial)) {
    Serial.println("DFPlayer Mini not found or not connected. Please check wiring and SD card.");
    Serial.println("Continuing without DFPlayer functionality.");
  } else {
    Serial.println("DFPlayer Mini initialized.");
    myDFPlayer.volume(currentVolume); // Set initial volume from global variable
    delay(100); // Give player time to set volume
    myDFPlayer.play(1); // Play sound file 1 on boot
    Serial.println("Playing sound 1 on boot (mp3/0001.mp3).");
  }

  // --- Web Server Setup ---
  Serial.println("Setting up web server handlers...");
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
  
  // *** IMPORTANT CHANGE HERE: Consolidated Sound Handler ***
  server.on("/play_sound", handlePlaySound); // All sound requests go through this one now!
  
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started.");
  Serial.println("----------------------------------------");
  Serial.print("Access the web interface at: http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}

// --- Loop Function ---
void loop() {
  server.handleClient();
  handlePixelAnimations();
}