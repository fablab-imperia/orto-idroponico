# Orto idroponico
> Sistema di controllo per coltivazione idroponica che consente circolazione dell'acqua e parziale correzione dei valori di PH e conducibilità della stessa. 

*Read this in other languages: [English](README.EN.md).*

## Schema di montaggio

![alt text](https://github.com/fablab-imperia/orto-idroponico/blob/main/orto-controller/frietzing/orto-idroponico_bb.jpg?raw=true)


# Compilazione del software

- [Arduino IDE](#arduino-ide)
- [Visual Studio Code e PlatformIO](#visual-studio-code-e-platformio)
- [Licenza](https://github.com/fablab-imperia/orto-idroponico/blob/main/LICENSE)

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
  * TaskScheduler

 ## Visual Studio Code e PlatformIO
 
 Per procedere con la compilazione mediante Visual Studio Code:
 1. Installare [Visual Studio Code](https://code.visualstudio.com/)
 2. Installare l'estensione [Platformio](https://platformio.org/install/ide?install=vscode)
 3. Aprire la cartella orto-controller 
 4. Avviare la compilazione  

