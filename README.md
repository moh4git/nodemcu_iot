# nodemcu_iot
IoT Project nodeMCU Webserver


To begin the project first :


Download the Arduino IDE from<br/>
https://www.arduino.cc/en/Main/Software


Install the nodeMCU from<br/>
https://arduino-esp8266.readthedocs.io/en/latest/installing.html


Install ESP8266 Filesystem Uploader in Arduino IDE<br/>
https://randomnerdtutorials.com/install-esp8266-filesystem-uploader-arduino-ide/

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


We use framework7 as frontend<br/>
https://framework7.io/
