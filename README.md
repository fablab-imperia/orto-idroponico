# Orto idroponico
> Sistema di controllo per coltivazione idroponica che consente circolazione dell'acqua e parziale correzione dei valori di PH e conducibilità della stessa. 

*Read this in other languages: [English](README.EN.md).*

## Schema di montaggio

I collegamenti ai pin sono definiti nel file ```pinouts.hpp``` e possono essere modificati opportunamente (prestando attenzione a non utilizzare impropriamente i pin riservati o con funzioni particolari, si veda il pinout della scheda per i dettagli).

Prossimamente verrà pubblicato uno schema completo del progetto.

# Compilazione del software

- [Arduino IDE](#arduino-ide)
- [Visual Studio Code e PlatformIO](#visual-studio-code-e-platformio)
- [Licenza](#licenza)

## Arduino IDE

Per procedere con la compilazione mediante Arduino IDE è possibile procedere con i seguenti passaggi:
1. Installazione dell'[IDE di Arduino](https://www.arduino.cc/en/software)
2. Clonazione del repository
```
git clone https://github.com/fablab-imperia/orto-idroponico.git
```
3. Installazione delle dipendenze tramite Arduino Library Manager (selezionare *Strumenti > Gestione Librerie* ):
  * DallasTemperature
  * OneWire
  * LiquidCrystal I2C
  * ArduinoJson
  * ESPAsyncWebServer
  * AsyncTCP
  
4. Installazione della scheda ESP32 nell'IDE:
  - a. Da *File > Preferenze > URL Gestore Schede Aggiuntive* aggiungere ```https://dl.espressif.com/dl/package_esp32_index.json```
  - b. Da *Strumenti > Schede > Gestore Schede* cercare ESP32 e installare la relativa scheda
  - c. Selezionare *ESP32 Dev Module* da *Strumenti > Schede*

5. Avviare la compilazione e caricare il programma sulla scheda (assicurarsi di aver selezionato la scheda e la porta seriale corretta dal menu *Strumenti*)


 ## Visual Studio Code e PlatformIO
 
 Per procedere con la compilazione mediante Visual Studio Code:
 1. Installare [Visual Studio Code](https://code.visualstudio.com/)
 2. Installare l'estensione [Platformio](https://platformio.org/install/ide?install=vscode)
 3. Aprire la cartella orto-controller 
 4. Avviare la compilazione  

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
