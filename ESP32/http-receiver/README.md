# ESP32 I2T Receiver --  HTTP Protocol

This code will allow you to ***collect information/data from the Tangle (Streams Channel)*** that was or is being sent from a *sending device* and then ***decode the json data*** and *save it in variables*. This will allow the feeder to ***drop food and water***.

**Currently the code is optimized to work by requesting data to *Keepy*.** <https://github.com/iot2tangle/Keepy>

A simple example to illustrate the idea: *Your paying iotas to the feeder address, the iota_check_balance.py getting the updated balance and sends that data to Tangle, the feeder will reading that json data from tangle with the updated balance and then drop the food and water.*

# Setting up your ESP32

*5 GPIO ESP32 pins* have been defined for the *outputs*. The *outputs* are **Boolean: 1 or 0 (3.3V or 0V in voltage)**.

The default *ESP32 Outputs GPIOs* are:
**Four for the step by step motor: Output/Relay 1 -> GPIO18  --  Output/Relay 2 -> GPIO18  --  Output/Relay 3 -> GPIO5  --  Output/Relay 4 -> GPIO17**
**One for the water pumo: peripherals/gpio WATER_PUMP -> GPIO23**

![ESP32 receiver Hardware connections](https://raw.githubusercontent.com/lukinias/iot_pet_feeder/main/images_and_videos/ESPReceiver.png)

**IMPORTANT:** The moment you start is when, ***for your safety, you should pay close attention to their electrical connections***. Whenever you are going to make changes to the connection make sure you have disconnected the AC connector, otherwise there may be a risk of electrocution. The following link from *Panasonic* company has safety tips when handling Relays. [Relays Cautions for Use - Panasonic](https://www3.panasonic.biz/ac/e/control/relay/cautions_use/index.jsp)

# Download Firmware on ESP32
This repository uses the ***Iot2Tangle C Core receiver devices*** adapted for ***ESP32-FreeRTOS*** offered in the official *Espressif Toolchain ESP-IDF SDK*. Once the SDK is installed you will have all the tools to compile and download the program on your ESP32.

Remember that for ESP32 to be taking data **you must have a Keepy instance running on some endpoint that ESP32 can reach**.

## 1) Install ESP-IDF SDK:
### Windows:
The easiest way to install ESP-IDF and their prerequisites is to download the ESP-IDF Tools installer from this URL:
<https://dl.espressif.com/dl/esp-idf-tools-setup-2.3.exe>

Just follow the steps and you will have the official *Espressif SDK Toolchain* installed on your computer.

To check other methods, the following page is suggested: 
<https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/windows-setup.html>

### Linux and macOS:
Prerequisites of ESP-IDF SDK:
```
sudo apt update
sudo apt install git wget flex bison gperf python python-pip python-setuptools cmake ninja-build ccache libffi-dev libssl-dev
```
It is recommended to install the stable version: ESP-IDF v4.1, you can download it from here:
```
git clone -b v4.1 --recursive https://github.com/espressif/esp-idf.git
```
Now install the SDK, this may take a while:
```
cd ~/esp-idf
./install.sh
. ./export.sh
```
After doing this last step do not close the shell, as we will compile and flash from here. If you close the shell you will have to do the previous step again.

### 2) Download the Pet Feeder ESP32 Repository and go to the 'http-receiver' folder:
You can download the repository directly from Github, or from shell or Command Prompt with the following command:
```
git clone https://github.com/lukinias/iot_pet_feeder.git
cd iot_pet_feeder/ESP32/http-receiver
```
### 3) Edit the file config.h
The *config.h* file must be opened and modified, this file is in the directory *'ESP32/http-receiver/main'* of the repository.

This step is very important if you want to make a connection to the gateway. Your *WiFi Credentials*, the *address* and *port* that will have the *I2T Keepy Host* running, the *Device Id*, and others configurations. The *Id Name Device* you define here must be between the devices you set in on the *Gateway configuration file*. 
```
/* Device */
const char* id_name = "ESP32-Receiver";

/* Network Configuration */
const char* ssid_WiFi = "mySSID";
const char* pass_WiFi = "myPASS";

/* HTTP Endpoint Configuration */
const char* address = "YOUR_KEEPY_HOST/messages/last";  /* Endpoint address (HTTP), must NOT include 'http://xxx' or 'tcp://xxx', and must include '/messages/last' for using in I2T Keepy*/
int port = 3002;

/* Enable Relays */
bool isEnable_Relay_1 = true;
bool isEnable_Relay_2 = true;	/*			true: Enable  --  false: Disable			*/
bool isEnable_Relay_3 = true;
bool isEnable_Relay_4 = true;

/* Interval of time */
long interval = 15;    /* Time in seconds between */
```
### 4) Compile and Download the Firmware:
Remembering to have the ***ESP-IDF Toolchain*** open, and you make sure you are at the root of the *http-receiver* folder run the following command:
```
idf.py build
```
If the compilation was correct it should read: *Project build complete*.

Now make sure you have ESP32 connected to your computer, and know what COM port it is connected to. (You can see this in 'Device Manager'). 

Then run the following command that will start flashing the firmware. (You will probably have to press the reset button on your ESP32 development board, even several times for it to recognize the board.)
```
idf.py -p COM1 flash    # COM1 is an Windows port example, you must put your port. In Linux /dev/ttyUSB0 is an example, and in macOS: '/dev/cu' 
```
Upon completion, the firmware is downloaded to your ESP32. If the *Keepy* is configured correctly ***you will reading data from Tangle via Streams and executing actions from there***.


# Debugging
## Open Serial Monitor
If configured correctly, *ESP32* should be getting data from ***Keepy*** automatically. However, you may want to verify that it is running on *ESP32*.

The code continuously sends information out the **serial port**, so it can read the serial port to see what is happening and detect errors.

You can use the 'Arduino Serial Monitor' for this, but we recommend using the following software:
### Windows:
Open *Command Prompt*.

Configure the Baud Rate of the port to 115200 bps:
```
mode COM1: baud=115200
```
Read the serial port:
```
copy COM1: con:
```
### Linux and macOS:
Install *cu Monitor*. It is an excellent shell monitor and very useful.
```
sudo apt install cu
```
Run *cu Monitor*:
```
cu -l /dev/ttyUSB0 -s 115200
```
The following screenshot is a reading of the *Serial Port*, you should see something like this:

![ESP32 receiver Serial Monitor](https://raw.githubusercontent.com/lukinias/iot_pet_feeder/main/images_and_videos/ESPSerialPort.png)

## Send Harcoded Json to Keepy to see ESP32 reactions

You can run the available examples quickly by sending the following json:

```curl --location --request POST 'YOUR-KEEPY-HOST:3002/messages' --header 'Content-Type: application/json' --data-raw '{"iot2tangle":[{"sensor":"Food","data":[{"cash":"1"}]}],"device": "ESP32-PETFEEDER","timestamp": "1601653408"}'```

# Json Accepted example

```{"iot2tangle":[{"sensor":"Food","data":[{"cash":"1"}]}],"device": "ESP32-PETFEEDER","timestamp": "1601653408"}```
