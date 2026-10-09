//An example sketch that lets you experiement with the library
//It shows how to create Devices and how to bind variables to those Devices
//Has one variable of each supported type and multiple independent interfaces to interact with them



#include <VariableManager.h>
#include <SerialUI.h>//For human readable interface displayed on the USB serial port 
#include <EepromBinder.h>


const uint8_t SLAVE_ADDRESS = 0x1C;


uint8_t testUint8;
uint16_t testUint16;
uint32_t testUint32;
uint64_t testUint64;

int8_t testInt8;
int16_t testInt16;
int32_t testInt32;
int64_t testInt64;

char testChar;
bool testBool;
float testFloat;
double testDouble;
int testInt;
size_t testSize_t;

uint16_t ledOnDurationMs = 1000;
uint16_t ledOffDurationMs = 1000;
bool ledState = false;
long ledLastToggled = 0; 

//Create two Devices, that represent some functionality on the MCU
//LocalDevice myExampleDevice(5, "Dev w/ all typ");// ([example] Device with all [supported] types)
LocalDevice myLedDevice(6, "LED");

void setup(){
  Serial1.setFIFOSize(256);//Increase default 32 bytes to 256 buffer (important when master is batch transmitting, or Device is busy and can't process it in time)
  Serial1.begin(115200);//Used for SerialUplink
  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(5000);
  Serial.println("Test start");

  //These functions will automatically restore the value stored in the EEPROM and start tracking changes

  EepromBinder::bind(ledOnDurationMs, 0, 1000);// [variable], [address in eeprom], [factory value]
  EepromBinder::bind(ledOffDurationMs, 2, 1000);//It is necessary to manually assign a starting address for each variable. As OnDuration is 2 bytes long (uint_16), it occupies addresses 0..1, so the next variable can only be written starting address 2
  // !! Address in EEPROM is independent from VariableManager address/id !!

  myLedDevice.bind(ledOnDurationMs, 0x00, "onDurationMs");
  myLedDevice.bind(ledOffDurationMs, 0x01, "offDurationMs");
  myLedDevice.bind(ledState, 0x02, "state", rwFlag::R);
  myLedDevice.bind(ledLastToggled, 0x03, "lastToggled", rwFlag::R);

  SerialUI::printVariableList();
}

long lastUiCheck = 0;

void loop(){
  //Call uplink and SerialUI checks only every 300ms, to simulate MCU being busy. Otherwise there is no need to externally schedule these, you can call them as often as you want
  if(lastUiCheck + 300 < millis()){
    lastUiCheck = millis();
    SerialUI::tick();
    EepromBinder::checkAllIfDue(millis());//This line will manage all EepromBinder functionality automatically
  }

  //A simple blinker code sample.
  if(ledLastToggled + (ledState?ledOnDurationMs:ledOffDurationMs) < millis()){
    ledLastToggled = millis();
    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState);
  }
}