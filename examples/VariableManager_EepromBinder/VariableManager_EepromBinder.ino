//An example sketch that lets you experiement with the library
//It shows how to create Devices and how to bind variables to those Devices
//Has one variable of each supported type and multiple independent interfaces to interact with them



#include <VariableManager.h>
#include <SerialUI.h>//For human readable interface displayed on the USB serial port 
#include <EepromBinder.h>


uint16_t ledOnDurationMs = 1000;
uint16_t ledOffDurationMs = 1000;
bool ledState = false;
long ledLastToggled = 0; 

//Create a VariableManager device, so we can change variables from Serial
LocalDevice myLedDevice(6, "LED");

void setup(){
  Serial1.setFIFOSize(256);//Increase default 32 bytes to 256 buffer (important when master is batch transmitting, or Device is busy and can't process it in time)
  Serial.begin(115200);
  Serial.println();

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  //These functions will automatically restore the value stored in the EEPROM and start tracking changes
  EepromBinder::bind(ledOnDurationMs, 0, 1000);// [variable], [address in eeprom], [factory value]
  EepromBinder::bind(ledOffDurationMs, 2, 1000);//It is necessary to manually assign a starting address for each variable. As OnDuration is 2 bytes long (uint_16), it occupies addresses 0..1, so the next variable can only be written starting address 2
  // !! Address in EEPROM is independent from VariableManager address/id !!

  // EepromBinder::factoryResetAll();//The first time after uploading to a new MCU (or adding new variables), you can call this function to load the default values instead of whatever junk was in the EEPROM

  //VariableManager stuff
  myLedDevice.bind(ledOnDurationMs, 0x00, "onDurationMs");
  myLedDevice.bind(ledOffDurationMs, 0x01, "offDurationMs");
  myLedDevice.bind(ledState, 0x02, "state", rwFlag::R);
  myLedDevice.bind(ledLastToggled, 0x03, "lastToggled", rwFlag::R);
  myLedDevice.bind(EepromBinder::unsavedChangesPending, nullptr, 0xA0, "eepromPending");
  myLedDevice.bind(nullptr, EepromBinder::factoryResetAllOnTrue, 0xA1, "factoryReset");
  SerialUI::printVariableList();
}

long lastUiCheck = 0;

void loop(){
  //Call EepromBinder & SerialUI checks only every 300ms, to simulate MCU being busy. Otherwise there is no need to externally schedule these, you can call them as often as you want
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