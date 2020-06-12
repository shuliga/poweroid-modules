# Poweroid UART to MQTT bridge
## Description
Poweroid UART to MQTT bridge (PWM-BMU) is extension module, plugged into Poweroid UART extension socket, usually replacing  Bluetooth or Radio modules.
## Specs
* _Voltage_: 5VDC
* _Current_: 0,07A
* _UART connection speed_: 115200 baud.
* _WiFi_: 802.11b,g


## AT Commands
Start commands with `AT+`; use single `AT` for mode test.

Set variable: `AT+{command}=<value>` or get it: `AT+<command>`.

_(~) marks persistent variable_

##### General
* `RESTART` soft restart of the module
* `PERSIST` stores persistent variables
* `CONNECT` _=<0|1>_ enables **WiFi/MQTT** run-time connection, defaut _true_
* `VERBOSE` _=<0|1>_ enables extra output, defaut _false_ 
* `MASTER` _=<0|1>_ enables communication with Poweroid device via **UART**, defaut _true_

##### UART
* `UART_INFO` displays UART variables and information
* `UART_DEVICE_ID` displays UART device id, _PWR-null_ id no device connected
* `UART_BAUD` displays UART baud speed

##### WiFi
* `WIFI_INFO` displays WiFi variables and status
* `WIFI_SSID` _=<~str>_ SSID to connect to
* `WIFI_PASS` _=<~str>_ WiFi password
* `WIFI_TRY_CONNECT` _=<0|1>_ enables WiFi initial connection tries, defaut _true_
* `WIFI_CONNECTED` _=<0|1>_ displays WiFi connection status

##### MQTT
* `MQTT_INFO` displays WiFi variables and status
* `MQTT_HOST` _=<~str>_ host
* `MQTT_PORT` _=<~integer>_ port
* `MQTT_USER` _=<~str>_ username
* `MQTT_PASS` _=<~str>_ password
* `MQTT_SERVICE` _=<~str>_ service id
* `MQTT_CUSTOMER` _=<~str>_ customer id
* `MQTT_ADDRESS` _=<~str>_ Device location address, use pattern: `<country code #2>-<City code #2>-<postal code>-<street code #2>-<building no.>-<apt. no.>`
* `MQTT_CONNECT` _=<0|1>_ enables communication with MQTTserver, defaut _true_
* `MQTT_CONNECTED` _=<0|1>_ displays MQTT connection status


## MQTT Topics
### Bridge Itself
##### Publishes
* `<service>/<customer>/<address>/PWR-BMU-1/init` — responds with init information on startup: \<attached device name\>, \<datetime\> 
* `<service>/<customer>/<address>/PWR-BMU-1/health` — responds with _'OK'_ on health check message
##### Subscribes
* `<service>/<customer>/<address>/PWR-BMU-1/#` see **Mode** for details on subsequent path
######Mode
* `<exec-at>` — silently executes AT commands, passed in payload, limited to 512 bytes 
* `<ota>` — tries to download binary update from https url, passed in payload; updates sketch on success; restarts 
* `<health>`  — health check input trigger
### On Behalf of Attached Poweroid Device
##### Publishes
* `<service>/<customer>/<address>/<device>/raw-out` — output of raw incoming UART data
* `<service>/<customer>/<address>/<device>/<mode>/<path...>` — parsed responses from UART Poweroid device
##### Subscribes
* `<service>/<customer>/<address>/<device>/#`  see **Mode** for details on subsequent path
######Mode
`raw-in`— direct input of Poweroid commands, is passed to UART
`cmd`— execute command (_set_ or _get_ actions), subsequent path and payload is parsed in order to form Poweroid command and pass it to UART
