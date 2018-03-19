/*******************************************************************************************************************
** INA class method definitions for INA226 Library.                                                               **
**                                                                                                                **
** See the INA226.h header file comments for version information. Detailed documentation for the library can be   **
** found on the GitHub Wiki pages at https://github.com/SV-Zanshin/INA226/wiki                                    **
**                                                                                                                **
** This program is free software: you can redistribute it and/or modify it under the terms of the GNU General     **
** Public License as published by the Free Software Foundation, either version 3 of the License, or (at your      **
** option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY     **
** WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the   **
** GNU General Public License for more details. You should have received a copy of the GNU General Public License **
** along with this program.  If not, see <http://www.gnu.org/licenses/>.                                          **
**                                                                                                                **
*******************************************************************************************************************/
#include "INA226.h"                                                           // Include the header definition    //
#include <Wire.h>                                                             // I2C Library definition           //
//#include <EEPROM.h>                                                           // Include the EEPROM library       //
INA226_Class::INA226_Class()  {}                                              // Class constructor                //
INA226_Class::~INA226_Class() {}                                              // Unused class destructor          //
/*******************************************************************************************************************
** Method begin() sets the INA226 Configuration details, without which meaningful readings cannot be made. If it  **
** is called without the option deviceNumber parameter then the settings are applied to all devices, otherwise    **
** just that specific device is targeted.                                                                         **
*******************************************************************************************************************/
uint8_t INA226_Class::begin(uint8_t address,uint8_t maxBusAmps,uint32_t microOhmR) {
  ina226_i2caddr = address;
  ina226_operatingMode = B111;                                           // Default to continuous mode       //
  ina226_current_LSB = (uint64_t)maxBusAmps*1000000000/32767;                    // Get the best possible LSB in nA  //
  ina226_calibration = (uint64_t)51200000 / ((uint64_t)ina226_current_LSB * (uint64_t)microOhmR / (uint64_t)100); // using 64 bit numbers throughout  //
  ina226_power_LSB   = (uint32_t)25*ina226_current_LSB;
  //begin();
                                                        
}

void INA226_Class::begin(void) {
    ina226_i2caddr = 0x4C;
  ina226_operatingMode = B111;                                           // Default to continuous mode       //
  ina226_current_LSB = (uint64_t)1*1000000000/32767;                    // Get the best possible LSB in nA  //
  ina226_calibration = (uint64_t)51200000 / ((uint64_t)ina226_current_LSB *(uint64_t)100 / (uint64_t)100);
  ina226_power_LSB   = (uint32_t)25*ina226_current_LSB;                                  // Store device address             //
}
 // of method begin()                                                        //                                  //
/*******************************************************************************************************************
** Method readByte reads 1 byte from the specified address                                                        **
*******************************************************************************************************************/
uint8_t INA226_Class::readByte(const uint8_t addr,const uint8_t deviceAddr){  //                                  //
  Wire.beginTransmission(deviceAddr);                                         // Address the I2C device           //
  Wire.write(addr);                                                           // Send the register address to read//
  _TransmissionStatus = Wire.endTransmission();                               // Close transmission               //
  delayMicroseconds(I2C_DELAY);                                               // delay required for sync          //
  Wire.requestFrom(deviceAddr, (uint8_t)1);                                   // Request 1 byte of data           //
  return Wire.read();                                                         // read it and return it            //
} // of method readByte()                                                     //                                  //
/*******************************************************************************************************************
** Method readWord reads 2 bytes from the specified address                                                       **
*******************************************************************************************************************/
int16_t INA226_Class::readWord(const uint8_t addr,const uint8_t deviceAddr){  //                                  //
  int16_t returnData;                                                         // Store return value               //
  Wire.beginTransmission(deviceAddr);                                         // Address the I2C device           //
  Wire.write(addr);                                                           // Send the register address to read//
  _TransmissionStatus = Wire.endTransmission();                               // Close transmission               //
  delayMicroseconds(I2C_DELAY);                                               // delay required for sync          //
  Wire.requestFrom(deviceAddr, (uint8_t)2);                                   // Request 2 consecutive bytes      //
  returnData = Wire.read();                                                   // Read the msb                     //
  returnData = returnData<<8;                                                 // shift the data over              //
  returnData|= Wire.read();                                                   // Read the lsb                     //
  return returnData;                                                          // read it and return it            //
} // of method readWord()                                                     //                                  //
/*******************************************************************************************************************
** Method writeByte write 1 byte to the specified address                                                         **
*******************************************************************************************************************/
void INA226_Class::writeByte(const uint8_t addr, const uint8_t data,          //                                  //
                             const uint8_t deviceAddr) {                      //                                  //
  Wire.beginTransmission(deviceAddr);                                         // Address the I2C device           //
  Wire.write(addr);                                                           // Send register address to write   //
  Wire.write(data);                                                           // Send the data to write           //
  _TransmissionStatus = Wire.endTransmission();                               // Close transmission               //
} // of method writeByte()                                                    //                                  //
/*******************************************************************************************************************
** Method writeWord writes 2 byte to the specified address                                                        **
*******************************************************************************************************************/
void INA226_Class::writeWord(const uint8_t addr, const uint16_t data,         //                                  //
                            const uint8_t deviceAddr) {                       //                                  //
  Wire.beginTransmission(deviceAddr);                                         // Address the I2C device           //
  Wire.write(addr);                                                           // Send register address to write   //
  Wire.write((uint8_t)(data>>8));                                             // Write the first byte             //
  Wire.write((uint8_t)data);                                                  // and then the second              //
  _TransmissionStatus = Wire.endTransmission();                               // Close transmission               //
} // of method writeWord()                                                    //                                  //
/*******************************************************************************************************************
** Method getBusMilliVolts retrieves the bus voltage measurement                                                  **
*******************************************************************************************************************/
uint16_t INA226_Class::getBusMilliVolts(const bool waitSwitch,                //                                  //
                                        const uint8_t deviceNumber) {         //                                  //
  if (waitSwitch) waitForConversion();                                        // wait for conversion to complete  //
  uint16_t busVoltage = readWord(INA_BUS_VOLTAGE_REGISTER,ina226_i2caddr);       // Get the raw value and apply      //
  busVoltage = (uint32_t)busVoltage*INA_BUS_VOLTAGE_LSB/100;                  // conversion to get milliVolts     //
  if (!bitRead(ina226_operatingMode,2) && bitRead(ina226_operatingMode,1)) {        // If triggered mode and bus active //
    int16_t configRegister = readWord(INA_CONFIGURATION_REGISTER,ina226_i2caddr);// Get the current register         //
    writeWord(INA_CONFIGURATION_REGISTER,configRegister,ina226_i2caddr);         // Write back to trigger next       //
  } // of if-then triggered mode enabled                                      //                                  //
  return(busVoltage);                                                         // return computed milliVolts       //
} // of method getBusMilliVolts()                                             //                                  //
/*******************************************************************************************************************
** Method getShuntMicroVolts retrieves the shunt voltage measurement                                              **
*******************************************************************************************************************/
int16_t INA226_Class::getShuntMicroVolts(const bool waitSwitch,               //                                  //
                                         const uint8_t deviceNumber) {        //                                  //
                                                                   // Hold device details in structure //
                      // Read EEPROM values               //
  if (waitSwitch) waitForConversion();                                        // wait for conversion to complete  //
  int32_t shuntVoltage = readWord(INA_SHUNT_VOLTAGE_REGISTER,ina226_i2caddr);    // Get the raw value                //
  shuntVoltage = shuntVoltage*INA_SHUNT_VOLTAGE_LSB/10;                       // Convert to microvolts            //
  if (!bitRead(ina226_operatingMode,2) && bitRead(ina226_operatingMode,0)) {        // If triggered and shunt active    //
    int16_t configRegister = readWord(INA_CONFIGURATION_REGISTER,ina226_i2caddr);// Get the current register         //
    writeWord(INA_CONFIGURATION_REGISTER,configRegister,ina226_i2caddr);         // Write back to trigger next       //
  } // of if-then triggered mode enabled                                      //                                  //
  return((int16_t)shuntVoltage);                                              // return computed microvolts       //
} // of method getShuntMicroVolts()                                           //                                  //
/*******************************************************************************************************************
** Method getBusMicroAmps retrieves the computed current in microamps.                                            **
*******************************************************************************************************************/
int32_t INA226_Class::getBusMicroAmps(const uint8_t deviceNumber) {           //                                  //
                                                                   // Hold device details in structure //
                      // Read EEPROM values               //
  int32_t microAmps = readWord(INA_CURRENT_REGISTER,ina226_i2caddr);             // Get the raw value                //
          microAmps = (int64_t)microAmps*ina226_current_LSB/1000;                // Convert to microamps             //
  return(microAmps);                                                          // return computed microamps        //
} // of method getBusMicroAmps()                                              //                                  //
/*******************************************************************************************************************
** Method getBusMicroWatts retrieves the computed power in milliwatts                                             **
*******************************************************************************************************************/
int32_t INA226_Class::getBusMicroWatts(const uint8_t deviceNumber) {          //                                  //
                                                                   // Hold device details in structure //
                      // Read EEPROM values               //
  int32_t microWatts = readWord(INA_POWER_REGISTER,ina226_i2caddr);              // Get the raw value                //
          microWatts = (int64_t)microWatts*ina226_power_LSB/1000;                // Convert to milliwatts            //
  return(microWatts);                                                         // return computed milliwatts       //
} // of method getBusMicroWatts()                                             //                                  //
/*******************************************************************************************************************
** Method reset resets the INA226 using the first bit in the configuration register                               **
*******************************************************************************************************************/
void INA226_Class::reset(const uint8_t deviceNumber) {                        // Reset the INA226                 //
                                                                   // Hold device details in structure //
  int16_t configRegister;                                                     // Hold configuration register      //
      writeWord(INA_CONFIGURATION_REGISTER,0x8000,ina226_i2caddr);               // Set most significant bit         //
      delay(I2C_DELAY);                                                       // Let the INA226 reboot            //
} // of method reset                                                          //                                  //
/*******************************************************************************************************************
** Method setMode allows the various mode combinations to be set. If no parameter is given the system goes back   **
** to the default startup mode.                                                                                   **
*******************************************************************************************************************/
void INA226_Class::setMode(const uint8_t mode,const uint8_t deviceNumber ) {  // Set the monitoring mode          //
                                                                   // Hold device details in structure //
  int16_t configRegister;                                                     // Hold configuration register      //
      configRegister = readWord(INA_CONFIGURATION_REGISTER,ina226_i2caddr);      // Get the current register         //
      configRegister &= ~INA_CONFIG_MODE_MASK;                                // zero out the mode bits           //
      ina226_operatingMode = B00001111 & mode;                                   // Mask off unused bits             //
      configRegister |= ina226_operatingMode;                                    // shift in the mode settings       //
      writeWord(INA_CONFIGURATION_REGISTER,configRegister,ina226_i2caddr);       // Save new value                   //
} // of method setMode()                                                      //                                  //
/*******************************************************************************************************************
** Method setAveraging sets the hardware averaging for the different devices                                      **
*******************************************************************************************************************/
void INA226_Class::setAveraging(const uint16_t averages,                      // Set the number of averages taken //
                                const uint8_t deviceNumber ) {                //                                  //
  uint8_t averageIndex;                                                       // Store indexed value for register //
  int16_t configRegister;                                                     // Configuration register contents  //
      configRegister = readWord(INA_CONFIGURATION_REGISTER,ina226_i2caddr);      // Get the current register         //
      if      (averages>=1024) averageIndex = 7;                              // setting depending upon range     //
      else if (averages>= 512) averageIndex = 6;                              //                                  //
      else if (averages>= 256) averageIndex = 5;                              //                                  //
      else if (averages>= 128) averageIndex = 4;                              //                                  //
      else if (averages>=  64) averageIndex = 3;                              //                                  //
      else if (averages>=  16) averageIndex = 2;                              //                                  //
      else if (averages>=   4) averageIndex = 1;                              //                                  //
      else                     averageIndex = 0;                              //                                  //
      configRegister &= ~INA_CONFIG_AVG_MASK;                                 // zero out the averages part       //
      configRegister |= (uint16_t)averageIndex << 9;                          // shift in the averages to register//
      writeWord(INA_CONFIGURATION_REGISTER,configRegister,ina226_i2caddr);       // Save new value                   //
} // of method setAveraging()                                                 //                                  //
/*******************************************************************************************************************
** Method setBusConversion specifies the conversion rate (see datasheet for 8 distinct values) for the bus        **
*******************************************************************************************************************/
void INA226_Class::setBusConversion(uint8_t convTime,                         // Set timing for Bus conversions   //
                                    const uint8_t deviceNumber ) {            //                                  //
                                                                   // Hold device details in structure //
  int16_t configRegister;                                                     // Store configuration register     //
      if (convTime>7) convTime=7;                                             // Use maximum value allowed        //
      configRegister = readWord(INA_CONFIGURATION_REGISTER,ina226_i2caddr);      // Get the current register         //
      configRegister &= ~INA_CONFIG_BUS_TIME_MASK;                            // zero out the Bus conversion part //
      configRegister |= (uint16_t)convTime << 6;                              // shift in the averages to register//
      writeWord(INA_CONFIGURATION_REGISTER,configRegister,ina226_i2caddr);       // Save new value                   //
} // of method setBusConversion()                                             //                                  //
/*******************************************************************************************************************
** Method setShuntConversion specifies the conversion rate (see datasheet for 8 distinct values) for the shunt    **
*******************************************************************************************************************/
void INA226_Class::setShuntConversion(uint8_t convTime,                       // Set timing for Bus conversions   //
                                      const uint8_t deviceNumber ) {          //                                  //
                                                                   // Hold device details in structure //
  int16_t configRegister;                                                     // Store configuration register     //
      if (convTime>7) convTime=7;                                             // Use maximum value allowed        //
      configRegister = readWord(INA_CONFIGURATION_REGISTER,ina226_i2caddr);      // Get the current register         //
      configRegister &= ~INA_CONFIG_SHUNT_TIME_MASK;                          // zero out the Bus conversion part //
      configRegister |= (uint16_t)convTime << 3;                              // shift in the averages to register//
      writeWord(INA_CONFIGURATION_REGISTER,configRegister,ina226_i2caddr);       // Save new value                   //
} // of method setShuntConversion()                                           //                                  //
/*******************************************************************************************************************
** Method waitForConversion loops until the current conversion is marked as finished. If the conversion has       **
** completed already then the flag (and interrupt pin, if activated) is also reset.                               **
*******************************************************************************************************************/
void INA226_Class::waitForConversion(const uint8_t deviceNumber) {            // Wait for current conversion      //
  uint16_t conversionBits = 0;                                                //                                  //
                                                                   // Hold device details in structure //
      conversionBits = 0;                                                     //                                  //
      while(conversionBits==0) {                                              //                                  //
        conversionBits = readWord(INA_MASK_ENABLE_REGISTER,ina226_i2caddr)       //                                  //
                         &(uint16_t)8;                                        //                                  //
      } // of while the conversion hasn't finished                            //                                  //
} // of method waitForConversion()                                            //                                  //
/*******************************************************************************************************************
** Method setAlertPinOnConversion configure the INA226 to pull the ALERT pin low when a conversion is complete    **
*******************************************************************************************************************/
void INA226_Class::setAlertPinOnConversion(const bool alertState,             // Enable pin change on conversion  //
                                           const uint8_t deviceNumber ) {     //                                  //
                                                                   // Hold device details in structure //
  uint16_t alertRegister;                                                     // Hold the alert register          //
      alertRegister = readWord(INA_MASK_ENABLE_REGISTER,ina226_i2caddr);         // Get the current register         //
      if (!alertState) alertRegister &= ~((uint16_t)1<<10);                   // zero out the alert bit           //
                  else alertRegister |= (uint16_t)(1<<10);                    // turn on the alert bit            //
      writeWord(INA_MASK_ENABLE_REGISTER,alertRegister,ina226_i2caddr);          // Write register back to device    //
} // of method setAlertPinOnConversion                                        //                                  //
