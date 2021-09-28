# Hydroponic garden
> Control system for hydroponic cultivation that allows the flow of water and partial correction of pH values and it's conductivity.

*Read this in other languages: [Italiano :it:](README.md).*

### Summary
- [Schematic](#schematic)
- [Configuration and Calibration](#configuration-and-calibration)
- [Compiling the Software](#compiling-the-software)
   - [Arduino IDE](#arduino-ide)
   - [Visual Studio Code and PlatformIO](#visual-studio-code-and-platformio)
- [Use and Graphic Interface](#use-and-graphic-interface)
- [License](#license)


# Schematic

The connections to the pins are defined in the file ` pinouts.hpp` and can be modified appropriately (taking care not to use the reserved pins or with particular functions improperly, see the [pinout of the board] (https://raw.githubusercontent.com/fablab-imperia/orto-idroponico/main/pinouts/esp32-az-delivery.jpg) for details).

![alt Orto Idroponico Schematic](https://raw.githubusercontent.com/fablab-imperia/orto-idroponico/main/images/Schematic_Orto-Idroponico.png)

# Configuration and Calibration

Before proceeding with the compilation of the software it is necessary to assign the values for the sensor calibration to `condCalibrationPoints []` and `phCalibrationPoints []`.

It is also possible to modify the main parameters from the [parameters.hpp](https://github.com/fablab-imperia/orto-idroponico/blob/main/orto-controller-web/parameters.hpp) file and the connections with the pins of the board from the file [pinouts.hpp](https://github.com/fablab-imperia/orto-idroponico/blob/main/orto-controller-web/pinouts.hpp)

# Compiling the Software

## Arduino IDE
To proceed with the compilation using Arduino IDE it is possible to proceed with the following steps:
1. Installation of the [Arduino IDE] (https://www.arduino.cc/en/software)

2. Cloning the repository
```
git clone https://github.com/fablab-imperia/orto-idroponico.git
```
3. Dependency installation via Arduino Library Manager (select *Sketch> Manage Libraries*) or by downloading the zip files from the site of each library or by copying the libraries of the folder  [libraries](https://github.com/fablab-imperia/orto-idroponico/tree/main/libraries):
  * [DallasTemperature](https://www.milesburton.com/Dallas_Temperature_Control_Library)
  * [OneWire](http://www.pjrc.com/teensy/td_libs_OneWire.html)
  * [LiquidCrystal I2C](https://github.com/marcoschwartz/LiquidCrystal_I2C)
  * [ArduinoJson](https://github.com/bblanchon/ArduinoJson)
  * [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)
  * [AsyncTCP](https://github.com/me-no-dev/AsyncTCP)
  * [Adafruit_ADS1X15](https://github.com/adafruit/Adafruit_ADS1X15)

4. Installation of the board ESP32 in Arduino IDE:
   - a. From *File > Preferences > Additional Boards Manager URLs* add ```https://dl.espressif.com/dl/package_esp32_index.json```
   - b. From *Strumenti > Schede > Gestore Schede* search ESP32 and install
   - c. Select *ESP32 Dev Module* from *Tools > Boards*

5. Install [ESP32 FS-plugin](https://github.com/me-no-dev/arduino-esp32fs-plugin) in Arduino IDE

6. Open file [orto-controller-web/orto-controller-web.ino](https://github.com/fablab-imperia/orto-idroponico/blob/main/orto-controller-web/orto-controller-web.ino) in Arduino IDE

7. Start the compilation and load the program on the board (make sure you have selected the correct board and serial port from *Tools* menu)

8. Upload the web files to the board from  *Tools > ESP32 Sketch Data Upload*



## Visual Studio Code and PlatformIO

To proceed with compilation using Visual Studio Code:
1. Install [Visual Studio Code](https://code.visualstudio.com/)
2. Install the [PlatformIO](https://platformio.org/install/ide?install=vscode) extension
3. Open the orto-controller-web folder
4. Install the required libraries
5. Start the compilation
6. Upload the web files to the board

# Use and Graphic Interface
After successfully loading the program and data, the board creates a WiFi network with SSID and password specified [here](https://github.com/fablab-imperia/orto-idroponico/blob/main/orto-controller-web/parameters.hpp#L22) (default SSID `OrtoWeb` and password `OrtoWeb12345`).
Connect with the correct credentials to the WiFi network and, using a browser (eg. Firefox, Chrome), connect to the IP address of the device, printed at startup on the serial monitor (default `192.168.4.1`).

The following graphical interface will be displayed.

![alt Manual Control Interface](https://raw.githubusercontent.com/fablab-imperia/orto-idroponico/main/images/Screenshoot_ESP_web_gui_manual_numbered.png)

![alt Automatic Control Interface](https://raw.githubusercontent.com/fablab-imperia/orto-idroponico/main/images/Screenshoot_ESP_web_gui_automatic_numbered.png)

The interface consists of the following elements:

1. Sensors indicators (the black dots in the *b* and *c* indicators indicate the currently set target), the range of the indicator is set [here](https://github.com/fablab-imperia/orto-idroponico/blob/main/orto-controller-web/data/main.js#L18) with parameters *min* and *max*:
   * a. Water temperature (indicator range: 0-35 °C)
   * b. pH of the water (indicator range: 0-14)
   * c. Water conductivity (indicator range: 0-3000 uS/cm)

2. Manual control panel (activated with button 4.a)
   * a. Button to enable automatic control with the parameters defined in section (4); when pressed, this section is replaced by section (4)
   * b. Manually activates the peristaltic fertilizer pump for as long as the button is pressed (it stops dispensing as soon as the button is released)
   * c. Manually activates the peristaltic acid pump (for pH correction) for as long as the button is pressed (it stops dispensing as soon as the button is released)
   * d. Activates / Deactivates the pump for water circulation, when pressed it switches its status and function (from Activate to Deactivate and vice versa)

3. Show or Hide the raw data received from the board, shown in section (5) (by default they are hidden)

4. Automatic control panel (activated with button 2.a)
   * a. Button to enable manual control, suspending the automatic correction of fertilizer and pH and turning off the recirculation pump; when pressed, this section is replaced by section (2)
   * b. Automatic control configuration parameters:
     * a. target pH value
     * b. conductivity target value
     * c. duration of activation of the fertilizer and acid pumps in case of correction of the values
     * d. time between two successive checks and corrections (activation of peristaltic pumps)
     * e. duration of the periodic water recirculation cycle
     * f. activation time of the pump for water recirculation in each period defined in point *e*
   * c. The button is normally disabled, it is activated automatically when one or more of the previous parameters are modified (the modified parameters are highlighted in green). Parameter changes only take effect after saving. The saved data are kept even after switching off, because they are stored in the EEPROM memory of the controller.

5. Log of raw data received from the board, the display can be enabled and disabled with the button (3)

Sensor data and status parameters (eg automatic/manual control, saved settings, manual pump activation) are automatically synchronized in real time on all clients connected to the card.

## Licenza
Sistema di gestione orto idroponico Fablab Imperia

Copyright ©2021  Fablab Imperia, Francesco Scarrone, Stefano Semeria, Valerio Pace

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
