# K-2SO Controller
K-2SO remote control for lights and sounds.  Using an ESP32, NeoPixels, and DFPlayer Mini.
________________________________________
Current hardware:
- ESP32 Development Board (I am using ESP32-WROVER)
- DFPlayer Mini, plus SD Card.
- WS2812 or similar NeoPixels
- SN74AHCT125N Logic level shifter.

________________________________________
Wiring Diagram

Level Shifter
- VCC -> 5V (External Power)
- GND -> GND (Common Ground)
- 1A -> ESP32 GPIO##
- 1OE -> GND
- 1Y -> NeoPixel Din
    ** If have anomalies withe chip tie all other A and OE pins to ground.

NeoPixel Strip(s)
- VCC -> 5V (External Power)
- GND -> GND (Common Ground)

DFPlayer Mini
- TX -> ESP32 GPIO##
- RX -> ESP32 GPIO##
- VCC -> 5V (External Power)
- GND -> GND (Common Ground)
- SPK+ -> Speaker Positive
- SPK- -> Speaker Negative

ESP32 (Additional Pins)
- VCC -> 5V (External Power) ***** Only needed if not powering from USB *****
- GND -> GND (Common Ground)

________________________________________
Update: June 29, 2025
- Code base is created and tested to run in its current state.
- Code is now broken out to multiple files that allows easier editing and future code expasnion.
