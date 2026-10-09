
#ifndef EEPROM_BINDER_H
#define EEPROM_BINDER_H

#include <Arduino.h>
#include <EEPROM.h>

template <typename T> class Binder;//Forward declare
class EepromBinder{
protected:
  static EepromBinder* headInstance;
  static unsigned long lastChanged;
  static const unsigned long waitDuration = 20000;//Wait this long for other changes, to commit them together
  static const unsigned long checkInterval = 1000;
  static unsigned long lastChecked;
  uint16_t address;
  bool valueStaged = false;//When it has staged a change to EEPROM, but it hasn't been committed yet

  static void init();
  static bool initialized;
  virtual void check(unsigned long ms) =0;
public:
  static int unsavedChanges;
  static void checkAll();
  static void checkAll(unsigned long ms);
  static void checkAllIfDue();
  static void checkAllIfDue(unsigned long ms);
  static void factoryResetAll(void* ctx = nullptr);
  EepromBinder* nextInstance = nullptr;
  EepromBinder(uint16_t address_);
  void registerSelf();
  virtual void factoryReset() = 0;
  template <typename T>
  static EepromBinder* bind(T& value_, uint16_t address_, typename std::remove_volatile<T>::type factoryValue_) {
    return new Binder<T>(value_, address_, factoryValue_);
  }
};

//           EepromBinder             EepromBinder             EepromBinder
//           IntBinder                IntBinder                IntBinder                        

template <typename T>
class Binder : public EepromBinder{
private:
  using PlainT = typename std::remove_volatile<T>::type;
  PlainT registeredValue;//The one staged for the EEPROM
  PlainT factoryValue;
public:
  T& value;
  Binder(T& value_, uint16_t address_, PlainT factoryValue_): Binder(value_, address_){
    factoryValue = factoryValue_;
  }
  Binder(T& value_, uint16_t address_): value(value_), EepromBinder(address_){
    EEPROM.get(address, registeredValue);
    value = registeredValue;//update it from memory
    Serial.print("Bound variable to EEPROM address 0x");
    Serial.print(address, HEX);
    Serial.println(". current value: " + String(value) + ", stored value: " + String(registeredValue)/* + "  (factory: " + String(factoryValue) + ")"*/);
  }
  void check(unsigned long ms) override{
    if(registeredValue != value){
      Serial.print("Change detected for address 0x");
      Serial.print(address, HEX);
      Serial.println(", from " + String(registeredValue) + " to " + String(value));
      if(!valueStaged)unsavedChanges++;
      lastChanged = ms;
      registeredValue = value;
      EEPROM.put(address, registeredValue);
      valueStaged = true;
    }
  }

  void factoryReset() override{
    if(value != factoryValue){
      Serial.print("At address 0x");
      Serial.print(address, HEX);
      Serial.println(", factory reset from " + String(value) + " to " + String(factoryValue));
      value = factoryValue;
      check(millis());
    }
  }
};

//           IntBinder                IntBinder                IntBinder                        
//           IntBinder                IntBinder                IntBinder                        

#endif //EEPROM_BINDER_H