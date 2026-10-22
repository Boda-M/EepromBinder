//An example sketch that shows how you can use the library

//core library methods:
//#include <EepromBinder.h> - Include the library, the library should be cloned to Documents/Arduino/libraries
//EepromBinder::bind(ledOnDurationMs, 0, 1000); - Bind a variable to an address of the EEPROM, with a factory value set
//   This command will automatically restore the value stored in EEPROM
//EepromBinder::checkAllIfDue(millis()); - Call this periodically to check for changes, than commit them to EEPROM when due (default 20s after last change detected)

//This is all that's needed for reugular operation

//EepromBinder::factoryResetAll() - can be used after flashing on a new MCU or adding new variables to to restore the user defined defaults instead of whatever was stored on the EEPROM before





//Demo sketch requires VariableManager library, so user inputs can be done on a serial UI


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
  myLedDevice.bind(EepromBinder::unsavedChangesPending, nullptr, 0xF0, "eepromPending");
  myLedDevice.bind(nullptr, EepromBinder::factoryResetAllOnTrue, 0xF1, "factoryReset");
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