# SPRYGGKLOPPA

This is the complete repository for RAYDIY's QUOTEFRAME (in Germany it's called SPRYGGKLOPPA) project.  
It includes all the 3D printing files and code needed to build this digital quote frame.

More informations here: https://raydiy.de/spryggkloppa-digitaler-spruechekalender/

# 3rd Party Resources

The following third party resources are used. Thank you!

- [GxEPD2 Library by ZinggJM](https://github.com/ZinggJM/GxEPD2)
- [ESP Async WebServer by Hristo Gochkov](https://github.com/me-no-dev/ESPAsyncWebServer.git)
- [ArduinoJson by Benoit Blanchon](https://github.com/bblanchon/ArduinoJson)
- [U8g2_for_Adafruit_GFX by Oliver Kraus](https://github.com/olikraus/U8g2_for_Adafruit_GFX.git)
- [mini.css by Angelos Chalaris](https://minicss.org/)

# Project Folders

The following folders are in the download archive:

- **QuoteFrameCode/**: PlatformIO project folder, contains the software for the esp32 microcontroller
- **FreeCAD/**: contains all FreeCAD files for the display mount and the frame
- **Fusion360/**: contains the Fusion 360 archive for the display mount and the frame
- **STL/**: contains the exported ready-to-print STL files from the Fusion 360 project.

## Assembling Instructions (german language only, yet)

Currently there is only a german video instruction. 

[![ASSEMBLING Video](https://img.youtube.com/vi/EmYzOi-3Cw0/0.jpg)](https://www.youtube.com/watch?v=EmYzOi-3Cw0)

## Materials & Tools I Used for This Project

The following links are Amazon affiliate links. If you use these links I do get a little provision from Amazon. The price for you stays the same!
So if you use these links you help RAYDIY to stay alive – at no costs. Thank you!

* [IKEA RIBBA Frame](https://geni.us/8dEyD)
* [7.5" ePaper Display Waveshare with HAT](https://geni.us/FhP4Ga6)
* [ESP32](https://geni.us/08mKhC4)
* [Silver wire, 0,6mm](https://geni.us/1WwKYS)
* [Stranded wire silicone insulation](https://amzn.to/3lhfChR)
* [Dupont sockets and connectors](https://geni.us/DBqo)
* [JST-XH sockets and connectors](https://geni.us/hBZhYYS)
* 1 x M2, 4 mm Screw (siehe auch Screw Set)
* 4 x M2, 6 mm Sscre (siehe auch Screw Set)
* [M3 M3 M4 Screw Set](https://geni.us/83MSv)
* [4 LiFePo Accumulators](https://geni.us/QBTAzw)
* [LiFePo Charger](https://geni.us/xk6Eb1)
* [Super glue GEL by Langlauf Schuhbedarf](https://geni.us/cV1n1T)
* [Crimping Pliers, if you create Dupont and JST wires by yourself](https://geni.us/oAjWw)
* [3D-Printer Artillery Sidewinder X1](https://geni.us/zfrg)
* [String Cutter](https://geni.us/kcM2)
* [PCB Board holder](https://geni.us/VXm4)
* [Tweezers](https://geni.us/MKAVfAc)
* [Mini Clamps](https://geni.us/uD2xH)
* Screwdriver
* Armored Tape
* Double sided tape

## Q&A

### Where do I change the pin settings?
If you want to use a different pin settings, open the file **include/Display.h**  
You will find the pin settings at the top of the file.

### Are there other configurations I can change?
If you like to, you can change some settings in the file **include/Config.h**

## Support the project ❤️
Do you like this Project?

Please subscribe to [my YouTube chanel](https://www.youtube.com/c/raydiyde) to support this and all follow up projects.

Thank you!
