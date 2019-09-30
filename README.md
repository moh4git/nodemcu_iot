# nodemcu_iot
IoT Project nodeMCU Webserver
https://randomnerdtutorials.com/esp8266-nodemcu-access-point-ap-web-server/

To begin the project first :

Software : <br/>
Download the Arduino IDE from<br/>
https://www.arduino.cc/en/Main/Software


Install the nodeMCU from<br/>
https://arduino-esp8266.readthedocs.io/en/latest/installing.html


Install ESP8266 Filesystem Uploader in Arduino IDE<br/>
https://randomnerdtutorials.com/install-esp8266-filesystem-uploader-arduino-ide/

Hardware : <br/>
nodeMCU <br/>
https://en.wikipedia.org/wiki/NodeMCU


Dht11 sensor <br/>
https://learn.adafruit.com/dht

2 Channel Relay module<br/>
https://www.geeetech.com/wiki/index.php/2-Channel_Relay_module

We use the two mode to connect to our nodeMCU<br/>
Access point mode<br/>
Station mode
You can edit nodeMCU in FSBrows.ino

// Access point Config<br/>
#define APSSID "MySmartHome"<br/>
#define APPSK  "12345678"<br/>
// Station Config<br/>
#define STASSID "MyAccessPoint"<br/>
#define STAPSK  "12345678"<br/>

Flashing the nodeMCU and restart<br/>

to connect with access point mode use<br/> 
192.168.4.1

to connect with station mode<br/>
Get the nodeMCU ip address from Arduino IDE serial monitor


We use framework7 as frontend<br/>
https://framework7.io/
