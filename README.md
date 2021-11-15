# Orto idroponico
> Sistema di controllo per coltivazione idroponica che consente circolazione dell'acqua e parziale correzione dei valori di PH e conducibilità della stessa.

*Read this in other languages: [English :uk: :us:](README.EN.md).*

### Sommario
- [Schema di Montaggio](#schema-di-montaggio)
- [Configurazione](#configurazione-e-calibrazione)
- [Compilazione del Software](#compilazione-del-software)
  - [Arduino IDE](#arduino-ide)
  - [Visual Studio Code e PlatformIO](#visual-studio-code-e-platformio)
- [Utilizzo e Interfaccia Grafica](#utilizzo-e-interfaccia-grafica)
- [Licenza](#licenza)

# Schema di Montaggio

I collegamenti ai pin sono definiti nel file `pinouts.hpp` e possono essere modificati opportunamente (prestando attenzione a non utilizzare impropriamente i pin riservati o con funzioni particolari, si veda il [pinout della scheda](https://raw.githubusercontent.com/fablab-imperia/orto-idroponico/main/pinouts/esp32-az-delivery.jpg) per i dettagli).

![alt Orto Idroponico Schematic](https://raw.githubusercontent.com/fablab-imperia/orto-idroponico/main/schematic-pcb/Schematic_Orto-Idroponico.png)

# Configurazione e Calibrazione

Prima di procedere con la compilazione del software è necessario assegnare a `condCalibrationPoints[]` e `phCalibrationPoints[]` i valori per la calibrazione dei sensori.

Inoltre è possibile modificare i parametri principali dal file [parameters.hpp](https://github.com/fablab-imperia/orto-idroponico/blob/main/orto-controller-web/parameters.hpp) e i collegamenti con i pin della scheda dal file [pinouts.hpp](https://github.com/fablab-imperia/orto-idroponico/blob/main/orto-controller-web/pinouts.hpp)

# Compilazione del Software

## Arduino IDE

Per procedere con la compilazione mediante Arduino IDE è possibile procedere con i seguenti passaggi:
1. Installazione dell'[IDE di Arduino](https://www.arduino.cc/en/software)

2. Clonazione del repository
```
git clone https://github.com/fablab-imperia/orto-idroponico.git
```
3. Installazione delle dipendenze tramite Arduino Library Manager (selezionare *Sketch > Gestione Librerie* ) oppure scaricando i file zip dal sito di ogni libreria oppure copiando le librerie della cartella [libraries](https://github.com/fablab-imperia/orto-idroponico/tree/main/libraries):
  * [DallasTemperature](https://www.milesburton.com/Dallas_Temperature_Control_Library)
  * [OneWire](http://www.pjrc.com/teensy/td_libs_OneWire.html)
  * [LiquidCrystal I2C](https://github.com/marcoschwartz/LiquidCrystal_I2C)
  * [ArduinoJson](https://github.com/bblanchon/ArduinoJson)
  * [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)
  * [AsyncTCP](https://github.com/me-no-dev/AsyncTCP)
  * [Adafruit_ADS1X15](https://github.com/adafruit/Adafruit_ADS1X15)

4. Installazione della scheda ESP32 nell'IDE:
   - a. Da *File > Preferenze > URL Gestore Schede Aggiuntive* aggiungere ```https://dl.espressif.com/dl/package_esp32_index.json```
   - b. Da *Strumenti > Schede > Gestore Schede* cercare ESP32 e installare la relativa scheda
   - c. Selezionare *ESP32 Dev Module* da *Strumenti > Schede*

5. Installare [ESP32 FS-plugin](https://github.com/me-no-dev/arduino-esp32fs-plugin) nell'ide di Arduino

6. Aprire il file [orto-controller-web/orto-controller-web.ino](https://github.com/fablab-imperia/orto-idroponico/blob/main/orto-controller-web/orto-controller-web.ino) nell'IDE di Arduino

7. Avviare la compilazione e caricare il programma sulla scheda (assicurarsi di aver selezionato la scheda e la porta seriale corretta dal menu *Strumenti*)

8. Caricare i file web sulla scheda da *Strumenti > ESP32 Sketch Data Upload*



## Visual Studio Code e PlatformIO

Per procedere con la compilazione mediante Visual Studio Code:
1. Installare [Visual Studio Code](https://code.visualstudio.com/)
2. Installare l'estensione [Platformio](https://platformio.org/install/ide?install=vscode)
3. Aprire la cartella orto-controller-web
4. Installare le librerie richieste
5. Avviare la compilazione  
6. Caricare i file web sulla scheda

# Utilizzo e Interfaccia Grafica
Dopo aver caricato correttamente il programma e i dati, la scheda crea una rete WiFi con SSID e password specificate [qui](https://github.com/fablab-imperia/orto-idroponico/blob/main/orto-controller-web/parameters.hpp#L22) (di default SSID `OrtoWeb` e password `OrtoWeb12345`).
Collegarsi con le credenziali corrette alla rete WiFi e, tramite un browser (es. Firefox, Chrome), connettersi all'indirizzo IP della scheda, stampato all'avvio sul monitor seriale (di default `192.168.4.1`).

Verrà visualizzata la seguente interfaccia grafica.

![alt Manual Control Interface](https://raw.githubusercontent.com/fablab-imperia/orto-idroponico/main/images/Screenshoot_ESP_web_gui_manual_numbered.png)

![alt Automatic Control Interface](https://raw.githubusercontent.com/fablab-imperia/orto-idroponico/main/images/Screenshoot_ESP_web_gui_automatic_numbered.png)

L'interfaccia è composta dei seguenti elementi:

1. Indicatore sensori (i pallini neri negli indicatori *b* e *c* indicano il target attualmente impostato), il range dell'indicatore è impostato [qui](https://github.com/fablab-imperia/orto-idroponico/blob/main/orto-controller-web/data/main.js#L18) con i parametri *min* e *max*:
   * a. Temperatura dell'acqua (range indicatore: 0-35 °C)
   * b. pH dell'acqua (range indicatore: 0-14)
   * c. Conducibilita dell'acqua (range indicatore: 0-3000 uS/cm)

2. Pannello di controllo manuale (attivabile con pulsante 4.a)
   * a. Pulsante per abilitare il controllo automatico con i parametri definiti alla sezione (4); alla pressione questa sezione viene sostituita dalla sezione (4)
   * b. Attiva manualmente la pompa peristaltica del concime per la durata della pressione del pulsante (interrompe l'erogazione appena si rilascia il bottone)
   * c. Attiva manualmente la pompa peristaltica dell'acido (per la correzione del pH) per la durata della pressione del pulsante (interrompe l'erogazione appena si rilascia il bottone)
   * d. Attiva/Disattiva la pompa per il ricircolo dell'acqua, alla pressione commuta il suo stato e la sua funzione (da Attiva a Disattiva e viceversa)

3. Mostra o Nasconde i dati grezzi ricevuti dalla scheda, mostrati nella sezione (5) (di default sono nascosti)

4. Pannello di controllo automatico (attivabile con pulsante 2.a)
   * a. Pulsante per abilitare il controllo manuale, sospendento la correzione automatica di fertilizzante e pH e spegnendo la pompa di ricircolo; alla pressione questa sezione viene sostituita dalla sezione (2)
   * b. Parametri di configurazione del controllo automatico:
     * a. valore target del pH
     * b. valore target della conducibilità
     * c. durata di attivazione delle pompe di fertilizzante e acido in caso di correzione dei valori
     * d. tempo che intercorre tra due controlli e correzioni (attivazione pompe peristaltiche) successive
     * e. durata ciclo periodico di ricircolo dell'acqua (se posto a 0 la pompa sarà sempre attiva e il parametro *f* verrà ignorato)
     * f. tempo di attivazione della pompa per il ricircolo dell'acqua in ogni periodo definito al punto *e*
   * c. Il pulsante è normalmente disabilitato, viene attivato automaticamente quando si modifica uno o più dei precedenti parameteri (i parametri modificati vengono evidenziati in verde). Le modifiche dei parametri hanno effetto solo dopo il salvataggio. I dati salvati vengono mantenuti anche dopo lo spegnimento, perché memorizzati nella memoria EEPROM del controllore.

5. Log dei dati grezzi ricevuti dall scheda, la visualizzazione può essere abilitata e disabilitata con il pulsante (3)

I dati dei sensori e i parametri di stato (es controllo automatico/manuale, impostazioni salvate, attivazione manuale della pompa) vengono sincronizzati automaticamente in tempo reale su tutti i client connessi alla scheda.

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
