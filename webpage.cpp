#include "webpage.h"
#include "config.h" // Include config.h to use ANGLE_MIN, ANGLE_CENTER, ANGLE_MAX

String getIndexPage(uint8_t currentBrightness, uint8_t currentVolume) {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8"> <!-- Added UTF-8 character set declaration -->
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>K-2SO</title>
    <style>
        body { font-family: Arial, Helvetica, Sans-Serif; text-align: center; background-color: #222; color: #FFF; margin-top: 50px; }
        .container { 
            max-width: 400px; 
            margin: auto; 
            padding: 20px; 
            border: 1px solid #444; 
            border-radius: 8px; 
            background-color: #333; 
            box-shadow: 0 0 10px rgba(0,0,0,0.5); }
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

        .servo-control-section {
            margin-top: 20px; /* Reduced margin-top to integrate better */
            padding-top: 0; /* Removed padding-top */
            border-top: none; /* Removed border-top */
        }
        .servo-control-section h2 {
            margin-top: 0;
            margin-bottom: 15px;
        }
        .servo-button-grid {
            display: grid;
            grid-template-columns: repeat(3, 1fr); /* 3 columns, equal width */
            gap: 0.5rem; /* Reduced gap between buttons */
            margin-bottom: 1.5rem; /* Reduced margin */
            max-width: 12rem; /* Condensed width for gamepad look */
            margin-left: auto;
            margin-right: auto;
            background-color: #444; /* Slightly darker background for the grid */
            padding: 0.5rem;
            border-radius: 0.75rem;
        }
        .servo-button-grid button {
            background-color: #000000; /* Matched Flicker/Pulse button background */
            color: #ffffff;
            font-weight: 600;
            width: 3.5rem; /* Fixed width for square buttons */
            height: 3.5rem; /* Fixed height for square buttons */
            padding: 0; /* Remove padding to let SVG fill */
            border-radius: 0.5rem;
            box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
            transition: background-color 0.2s ease-in-out, transform 0.1s ease-in-out;
            border: none;
            cursor: pointer;
            display: flex; /* Use flex to center SVG */
            align-items: center;
            justify-content: center;
        }
        .servo-button-grid button:hover {
            background-color: #333333; /* Matched Flicker/Pulse button hover background */
            transform: translateY(-2px); /* Slight lift effect */
        }
        .servo-button-grid button:active {
            transform: translateY(0); /* Press effect */
            box-shadow: none;
        }
        .servo-button-grid svg {
            width: 70%; /* Make SVG fill most of the button */
            height: 70%;
            fill: currentColor; /* Inherit button color */
            stroke: currentColor; /* For arrow lines */
            stroke-width: 2; /* For arrow lines */
            stroke-linecap: round;
            stroke-linejoin: round;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>K-2SO Control</h1>

        <div class="servo-control-section">
            <h2>Eye Controls</h2>
            <div class="servo-button-grid">
                <!-- Button 1: Servo0 = Max, Servo1=Max (Up-Left) -->
                <button onclick="setServos(270, 270)">
                    <svg viewBox="0 0 24 24"><path d="M17 17 L7 7 M7 7 L14 7 M7 7 L7 14"/></svg>
                </button>
                <!-- Button 2: Servo0 = Max, Servo1=Center (Up) -->
                <button onclick="setServos(270, 90)">
                    <svg viewBox="0 0 24 24"><path d="M12 19 L12 5 M12 5 L7 10 M12 5 L17 10"/></svg>
                </button>
                <!-- Button 3: Servo0=Max, Servo1=Min (Up-Right) -->
                <button onclick="setServos(270, 0)">
                    <svg viewBox="0 0 24 24"><path d="M7 17 L17 7 M17 7 L10 7 M17 7 L17 14"/></svg>
                </button>

                <!-- Button 4: Servo0=center, Servo1=Max (Left) -->
                <button onclick="setServos(90, 270)">
                    <svg viewBox="0 0 24 24"><path d="M19 12 L5 12 M5 12 L10 7 M5 12 L10 17"/></svg>
                </button>
                <!-- Button 5: Servo0=center, servo1=center (Center Dot) -->
                <button onclick="setServos(90, 90)">
                    <svg viewBox="0 0 24 24"><circle cx="12" cy="12" r="4"/></svg>
                </button>
                <!-- Button 6: Servo0=center, servo1=Min (Right) -->
                <button onclick="setServos(90, 0)">
                    <svg viewBox="0 0 24 24"><path d="M5 12 L19 12 M19 12 L14 7 M19 12 L14 17"/></svg>
                </button>

                <!-- Button 7: Servo0=Min, Servo1=Max (Down-Left) -->
                <button onclick="setServos(0, 270)">
                    <svg viewBox="0 0 24 24"><path d="M17 7 L7 17 M7 17 L14 17 M7 17 L7 10"/></svg>
                </button>
                <!-- Button 8: Servo0=Min, Servo1=Center (Down) -->
                <button onclick="setServos(0, 90)">
                    <svg viewBox="0 0 24 24"><path d="M12 5 L12 19 M12 19 L7 14 M12 19 L17 14"/></svg>
                </button>
                <!-- Button 9: Servo0=Min, Servo1=Min (Down-Right) -->
                <button onclick="setServos(0, 0)">
                    <svg viewBox="0 0 24 24"><path d="M7 7 L17 17 M17 17 L10 17 M17 17 L17 10"/></svg>
                </button>
            </div>
        </div>        
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
        <h2>Sound Controls</h2>
        <div class="slider-container">
            <label for="volumeSlider">Volume</label>
            <input type="range" id="volumeSlider" name="value" min="0" max="30" value=")rawliteral";
  html += String(currentVolume);
  html += R"rawliteral(" class="volume-slider" onchange="setVolume(this.value)">
        </div>
        
        <!-- Add or remove button lines below to allow more or less sound files. -->
        <!-- Edit the play sound number to associate to the files on your DFPLayer mini SD Card. -->
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
        
    </div> 
    <script>
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
            const url = '/playSound?file=' + fileNum;
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

        // --- NEW SERVO CONTROL FUNCTIONS ---
        // Function to send commands for both servos
        function setServos(angle0, angle1) {
            console.log("Setting Servo 0 to " + angle0 + " and Servo 1 to " + angle1 + " degrees.");
            // Send a single request to the ESP32 with both angles
            const url = `/setServos?angle0=${angle0}&angle1=${angle1}`;
            showStatus(`Moving servos...`);
            fetch(url)
                .then(response => {
                    if (response.ok) {
                        console.log(`Servos moved successfully to S0:${angle0}, S1:${angle1}`);
                        showStatus(`Servos moved.`);
                    } else {
                        console.error(`Failed to move servos. Status: ${response.status}`);
                        showStatus(`Error moving servos: ${response.status}`, true);
                    }
                })
                .catch(error => {
                    console.error('Network error:', error);
                    showStatus('Network error. Check connection.', true);
                });
        }

        // Initial setup on page load (centers servos)
        window.onload = function() {
            // Original K-2SO page might have its own onload, ensure this runs after.
            // Or if this is the only onload, it will center servos immediately.
            setServos(90, 90); // Center both servos on page load
        };
    </script>
</body>
</html>
)rawliteral";
  return html;
}
