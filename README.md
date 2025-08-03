# K-2SO Controller
This project set out to combine some neat things that other people were doing with animation and interaction on their K-2SO droid heads.  While many folks had completed things and showed them off none had made available the set of information, code, or learnings from their adventure.  I am not a professional developer nor electronics engineer but I have soem experience in both worlds, and AI (mostly Gemini) helped fill in the gaps. 

The project goals were to combine sound output, moving eyes, and light up eyes, with the overall set hopefully controllable in some remote fashion.  The ESP32 has been setup to control the pixels, servos, and DFPlayer Mini then host a webpage via WiFi to allow remote control. 

All of the original goals have been met so far with some new goals now being added to enhance what is created. I hope to track these to do items in the GitHub issues section so I don't lose track over time.  Of course the community as a whole is more than welcome to come in and help make things better.  The webpage could use some prettier style and layout, for sure.

Really hope this is useful for anyone even if you don't use it directly.  You can catch me in the Droid Division Facebook group if you want to chat or ask questions.


________________________________________
CURRENT HARDWARE

Droid Division - K-2SO bust or full body files are fine:
-     https://www.droiddivision.com/
Animatronic Eyes file:
-     https://drive.google.com/drive/folders/19iHBdptouNafrbKEvJxx6DNjNvaGwVwN?usp=drive_link
ESP32 Development Board (I am using ESP32-WROVER, other will be compatible but double check pin outs)
-     https://www.amazon.com/dp/B0B13X1KWW?ref=ppx_yo2ov_dt_b_fed_asin_title
DFPlayer Mini, plus SD Card.
-     https://www.amazon.com/dp/B0CH2WZT5Q?ref=ppx_yo2ov_dt_b_fed_asin_title
WS2812 or similar NeoPixels (Adafruit NeoPixel Jewel is recommend from the original printed parts)
-     https://www.amazon.com/dp/B0B2D6JDVJ?ref=ppx_yo2ov_dt_b_fed_asin_title&th=1
SN74AHCT125N Logic level shifter.
-     https://www.amazon.com/dp/B08NXJY1SM?ref=ppx_yo2ov_dt_b_fed_asin_title
PCA9685 I2C Servo Control Board
-     https://www.amazon.com/dp/B07BRS249H?ref=ppx_yo2ov_dt_b_fed_asin_title
9iMod 11kg Servo x 2 (or similar, 180 or 270 should not matter)
-     AliExpress is where I found these, look around there as vendor names and links change all the time.
5V Power Supply
- I have used an AC to DC adapter I had at my house that has 5V @ 2A output. You can use any such adapter, or if you have a battery pack that can support similar output it may be usable and should be fully tested, you do so at your own risk

________________________________________
WIRING DIAGRAM

- See the "_wiring_diagram.txt" file for details of connections.

________________________________________
HOW TO USE
1. Order all the hardware outlined and assemble the circuit as defined in the "_wiring_diagram.txt" file. I do not recommend installing everything inside of the K-2SO head until everything is fully tested working. 
2. Download and install Arduini IDE to your computer, setup the software. (tutorials exist for this I won't reinvent the wheel here).
3. Install the ESP32 board manager in Arduini (again tutorials exist for this elsewhere).
4. Install Arduino libraries for:
   - DFRobotDFPlayerMini
   - Adafruit_PWMServoDriver
   - Adafruit_NeoPixel
5. Download the latest release of this repistory to your local computer.
6. Open the K-2SO_Controller.ino file in Arduino.
7. Edit the "config.h" file:
   - Change the WiFi SSID/Password to match your network.
   - Edit the number of pixels and default brightness you desire.
   - Adjust the servo pulse limit values if desired. You will need a different servo test program to validate theses if you choose.
8. If desired you can edit the webpage (webpage.cpp) content, layout, colors, etc. but this is not required.
   
   *** DO NOT EDIT "animations" or "handlers" files unless you know what you are doing in them. ***
10. Upload to the ESP32 via Arduino (tutorials exist for this elsewhere), some tips I found was to make sure the serial speed was set to 115200, Ardunio sometimes reset this higher and it caused programming errors.
11. Using serial monitor you should see messages after programming and reboot to indicate status of the boot up.  Some communication issues will reset in a halted bootup, these must get fixed prior to be able to test functionality. Additionally, serial monitor should report the IP address obtained from your network.
12. Open the IP address from another device connected to the same network (phone, tablet, PC, Mac, etc.) to display the webpage and begin testing your hardware setup.
13. If needed adjust servo values, brightness, volume, etc and program the board again until it meets your desires.
14. Install all the physical hardware into the K-2SO head and enjoy. 



________________________________________
PROJECT UPDATE LOG

June 29, 2025
- Code base is created and tested to run in its current state.
- Code is now broken out to multiple files that allows easier editing and future code expasnion.

July 31, 2025
- Servo controls added and tested.

Aug. 3, 2025
- Documentation updates.
