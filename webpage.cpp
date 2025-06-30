#include "webpage.h"

String getIndexPage(uint8_t currentBrightness, uint8_t currentVolume) {
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
    </script>
</body>
</html>
)rawliteral";
  return html;
}
