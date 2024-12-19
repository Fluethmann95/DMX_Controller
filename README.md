# DMX_Controller
Simple static DMX Controller using Serial-to-RS485 Shield, 20x4 I2C LCD and Rotary Encoder

#Features
- Individual Setting of all 512 Channels
- Live Display of set-values while configurating
- Backup of the settings in Atmega EEPROM, Auto-Load while startup
- Simple configuration by using only one rotary-encoder and few possibilities

#Usage
1. Boot device. The last saved settings will automatically be loaded
2. By using the rotary-encoder, select the channel to configure.
3. Short-Click the encoder-button to enter the configuration view
4. When in configuration mode (shown by "set value" in 2nd row), use the encoder to set the desired value for the previously selected channel
5. Leave the menu by short-clicking the encoder-button again.
6. To store the values in EEPROM, long-press the button in any state. Successful writing-process is shown by a flashing lcd-backlight.
