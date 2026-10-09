# WIP

### Only tested on Raspberry Pi Pico (1&2)
AKA RP2040 & RP2350, using Earle F. Philhower's pico core

- As the EEPROMs on common microcontrollers can only be written so many times before degrading, this library is quite conservative as for when to save.

- As a default setting, it will only write to EEPROM after a 20s wait period after the last change detected. This is good for users changing parameters in a UI or similar, but might prevent saving anything if there is a value constantly updated.





TODO: automatic save on powerloss (are we fast enough to detect a power loss and commit to memory while running on some capacitors? Can an energy reserve circuit be built efficiently on custom designs?)

TODO: periodic saving even if a constantly updated variable would keep us from committing