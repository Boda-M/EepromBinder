#include "EepromBinder.h"


EepromBinder* EepromBinder::headInstance = nullptr;
unsigned long EepromBinder::lastChanged = 0;
int EepromBinder::unsavedChanges = 0;
unsigned long EepromBinder::lastChecked = 0;
bool EepromBinder::initialized = false;

/*static*/ void EepromBinder::init(){
  EEPROM.begin(256);
  initialized = true;
  DEBUG_PRINTLN("EEPROM binder initialized.");
}

/*static*/ void EepromBinder::checkAll(){checkAll(millis());}

/*static*/ void EepromBinder::checkAll(unsigned long ms){
  if(!initialized)init();
  EepromBinder* next = headInstance;
  int n = 0;
  while(next){
    next->check(ms);
    next = next->nextInstance;
  }
}

/*static*/ void EepromBinder::checkAllIfDue(){checkAllIfDue(millis());}

/*static*/ void EepromBinder::checkAllIfDue(unsigned long ms){
  if(checkInterval + lastChecked <= ms){
    if(!initialized)init();
    lastChecked = ms;
    checkAll(ms);
    if(unsavedChanges > 0 && waitDuration + lastChanged <= ms){
      DEBUG_PRINT(unsavedChanges);
      DEBUG_PRINTLN(" change(s) committed to EEPROM.");
      
      EEPROM.commit();
      unsavedChanges = 0;

      EepromBinder* next = headInstance;//Clear staged flags
      int n = 0;
      while(next){
        next->valueStaged = false;
        next = next->nextInstance;
      }
    }
  }
}

void EepromBinder::factoryResetAllOnTrue(bool confirm){
  if(confirm)factoryResetAll();
}

void EepromBinder::factoryResetAll(void* ctx){
  if(!initialized)init();
  EepromBinder* next = headInstance;
  int n = 0;
  while(next){
    next->factoryReset();
    next = next->nextInstance;
  }
}

bool EepromBinder::unsavedChangesPending(){
  return unsavedChanges > 0;
}

EepromBinder::EepromBinder(uint16_t address_)
  : address(address_){
  registerSelf();
}

void EepromBinder::registerSelf(){
  if(!initialized)init();
  nextInstance = headInstance;
  headInstance = this;
}

/*template <typename T>
EepromBinder* EepromBinder::bind(T& value_, uint16_t address_){
  return new Binder<T>(value_, address_);
}*/

template class Binder<int>;
template class Binder<unsigned int>;
template class Binder<float>;
template class Binder<double>;
template class Binder<byte>;
template class Binder<char>;
template class Binder<bool>;
//template class Binder<uint8_t>;//same as unsigned char
template class Binder<int8_t>;
template class Binder<uint16_t>;
template class Binder<int16_t>;
template class Binder<uint32_t>;
template class Binder<int32_t>;