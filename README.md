# Ultraschallradar #

# Installation

## Raspbian

Wir standen zwischen der Auswahl von ArchLinux und dem Raspbiannetinstaller ( minimale Version von Respbian ), haben uns für den Raspbiannetinstaller entschieden, da die größe der beiden Systeme ähnlich ist, der Raspiannetinstaller aber eine gewohnte Umgebung mit sich bringt.

Für die Installation muss folgendes zip-file herunter geladen werden [raspbian-ua-netinst v1.0.2] (https://github.com/debian-pi/raspbian-ua-netinst/releases#raspbian-ua-netinst/raspbian-ua-netinst-latest.zip).
Das zip-file muss entpackt und auf eine SD-Karte, die zuvor mit FAT32 formatiert wurde, kopiert werden.
Wenn diese Schritte erledigt sind, kann die SD-Karte in das Raspberry Pi eingelegt und gestartet werden.
    

Folgende Packete müssen installiert werden.
```
apt-get install build-essential git-core 
apt-get update
apt-get upgrade
```

Für die GUI verwendete Packete
```
apt-get install xserver-xorg-core xinit
apt-get install openbox
```

Um die I/O Ports des Raspberry Pi anzusprechen
```
git clone git://git.drogon.net/wiringPi
cd wiringPi
./build
```

Allegro
```
git clone git://git.code.sf.net/p/alleg/allegro
cd allegro
git checkout 4.0
mkdir build; cd build
```

## zusätzlicher Temperatursensor

Zusätzlich wurde noch der Temperatur Sensor [BMP085](http://www.adafruit.com/datasheets/BMP085_DataSheet_Rev.1.0_01July2008.pdf "Bosch BMP085") verwendet. Dieser kann wie auf folgender [Seite](https://bitbucket.org/doriemel/temperaturmessung/wiki/Home "Installation des BMP085") installiert werden.
Dieser Temperatur Sensor fließt in die Berechnung für die Schallgeschwindigkeit mit ein.
In der Datei alegroUltraschall.c wurde ein Flag definiert.
```c
bool tempSensor = true;
```
Setzt man dieses Flag auf false, weil man keinen Sensor hat, wir für die Berechnung des Schalls mit eine Temperatur von 20 Grad gerechnet.

## Pinbelegung

Die von uns gewählt Pinbelegungen.

Temperatursensor (Bosch BMP085):
```
UCC = 3 Volt
GND = Ground
SDA = SDA1 I2C / GPIO 2
SCL = SCL1 I2C / GPIO 3
```

Ultraschallsensor (HC - SR04):
```
Vcc = 5 Volt
Gnd = Ground
Echo = GPIO 8 / SPICS0 / CE0
Trig = GPIO 7 / SPICS1 / CE1
```

Motor (28BYJ-48 – 5V Stepper Motor):
```
+ = 5 Volt
- = Ground
IN1 = GPIO 18
IN2 = GPIO 23
IN3 = GPIO 24
IN4 = GPIO 25
```