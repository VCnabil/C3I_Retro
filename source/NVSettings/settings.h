//------------------------------------------------------------------------------
// File:        settings.h
// Description: System settings.
//------------------------------------------------------------------------------

#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include "project.h"

/// @defgroup settings Settings Functions
/// Functions for handling the Settings.
/// @{

//------------------------------------------------------------------------------
// DEFINES
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// TYPES
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// PUBLIC FUNCTION PROTOTYPES
//------------------------------------------------------------------------------

/// Initialize the settings
void SettingsInit(void);

/// Function to Load the settings and apply them.
void SettingsLoad(void);

/// Function to Save the current settings.
void SettingsSave(void);

/// Get the currently used language.
/// @returns the language ID as a number
uint32_t SettingsGetLanguageIndex(void);

/// Sets the currently used language 
///@param languageIndex - Index number for the language to be used.
void SettingsSetLanguageIndex(uint32_t languageIndex);

/// Get the current state of the buzzer
///@returns TRUE if the buzzer is enabled, FALSE otherwise
BOOL SettingsIsBleepEnabled(void);

/// Toggles the Buzzer ON/OFF.
void SettingsToggleBleep(void);

/// Units related Functions
void SettingsUnitSetPressureUnits(uint8_t NewUnits);
void SettingsUnitSetTemperatureUnits(uint8_t NewUnits);
void SettingsUnitSetVolumeUnits(uint8_t NewUnits);
void SettingsUnitSetDistanceUnits(uint8_t NewUnits);
void SettingsUnitSetFuelRateUnits(uint8_t NewUnits);
void SettingsUnitSetSpeedUnits(uint8_t NewUnits);
void SettingsUnitSetDepthUnits(uint8_t NewUnits);
void SettingsUnitSetAngleUnits(uint8_t NewUnits);
void SettingsUnitSetBearingUnits(uint8_t NewUnits);
void SettingsUnitSetEconomyUnits(uint8_t NewUnits);
void SettingsUnitSetTimeUnits(uint8_t NewUnits);

void SettingsUnitsTogglePressure(void* userData = nullptr);
void SettingsUnitsToggleTemperature(void* userData = nullptr);
void SettingsUnitsToggleVolume(void* userData = nullptr);
void SettingsUnitsToggleDistance(void* userData = nullptr);
void SettingsUnitsToggleFuelRate(void* userData = nullptr);
void SettingsUnitsToggleSpeed(void* userData = nullptr);
void SettingsUnitsToggleDepth(void* userData = nullptr);
void SettingsUnitsToggleAngle(void* userData = nullptr);
void SettingsUnitsToggleBearing(void* userData = nullptr);
void SettingsUnitsToggleEconomy(void* userData = nullptr);
void SettingsUnitsToggleTime(void* userData = nullptr);

uint8_t SettingsUnitGetPressureUnits(void);
uint8_t SettingsUnitGetTemperatureUnits(void);
uint8_t SettingsUnitGetVolumeUnits(void);
uint8_t SettingsUnitGetDistanceUnits(void);
uint8_t SettingsUnitGetFuelRateUnits(void);
uint8_t SettingsUnitGetSpeedUnits(void);
uint8_t SettingsUnitGetDepthUnits(void);
uint8_t SettingsUnitGetAngleUnits(void);
uint8_t SettingsUnitGetBearingUnits(void);
uint8_t SettingsUnitGetEconomyUnits(void);
uint8_t SettingsUnitGetTimeUnits(void);
uint8_t SettingsUnitGetUnits(uint32_t UnitType);

uint8_t SettingsGetCANSystemInstance(uint32_t CANPort);
void SettingsSetCANSystemInstance(uint32_t CANPort, uint8_t systemInstance);
uint8_t SettingsGetCANDeviceInstance(uint32_t CANPort);
void SettingsSetCANDeviceInstance(uint32_t CANPort, uint8_t deviceInstance);

int32_t SettingsGetTestSave(void);
void SettingsSetTestSave(int32_t value);

// VECTOR  Prototypes for EEPROM Parameters

uint8_t SettingsGetPN_MAX(void);
void SettingsSetPN_MAX(uint8_t value);

uint8_t SettingsGetPN_MIN(void);
void SettingsSetPN_MIN(uint8_t value);

uint8_t SettingsGetSN_MAX(void);
void SettingsSetSN_MAX(uint8_t value);

uint8_t SettingsGetSN_MIN(void);
void SettingsSetSN_MIN(uint8_t value);

uint8_t SettingsGetPB_MAX(void);
void SettingsSetPB_MAX(uint8_t value);

uint8_t SettingsGetPB_MIN(void);
void SettingsSetPB_MIN(uint8_t value);

uint8_t SettingsGetSB_MAX(void);
void SettingsSetSB_MAX(uint8_t value);

uint8_t SettingsGetSB_MIN(void);
void SettingsSetSB_MIN(uint8_t value);

uint8_t SettingsGetPT_MAX(void);
void SettingsSetPT_MAX(uint8_t value);

uint8_t SettingsGetPT_MIN(void);
void SettingsSetPT_MIN(uint8_t value);

uint8_t SettingsGetST_MAX(void);
void SettingsSetST_MAX(uint8_t value);

uint8_t SettingsGetST_MIN(void);
void SettingsSetST_MIN(uint8_t value);

uint8_t SettingsGetPB_NEUTRAL_THRUST(void);
void SettingsSetPB_NEUTRAL_THRUST(uint8_t value);

uint8_t SettingsGetSB_NEUTRAL_THRUST(void);
void SettingsSetSB_NEUTRAL_THRUST(uint8_t value);

uint8_t SettingsGetSTATION_TYPE(void);
void SettingsSetSTATION_TYPE(uint8_t value);

uint8_t SettingsGetDATA_MODE(void);
void SettingsSetDATA_MODE(uint8_t value);

uint8_t SettingsGetNOZZLEMAP_FLIP(void);
void SettingsSetNOZZLEMAP_FLIP(uint8_t value);

uint8_t SettingsGetBUCKETMAP_FLIP(void);
void SettingsSetBUCKETMAP_FLIP(uint8_t value);

uint8_t SettingsGetTABMAP_FLIP(void);
void SettingsSetTABMAP_FLIP(uint8_t value);

uint8_t SettingsGetINTMAP_FLIP(void);
void SettingsSetINTMAP_FLIP(uint8_t value);

uint8_t SettingsGetCOMMS_MODE(void);
void SettingsSetCOMMS_MODE(uint8_t value);

uint8_t SettingsGetSERIALXMIT_FlIP(void);
void SettingsSetSERIALXMIT_FlIP(uint8_t value);

 
uint8_t SettingsGetINTSTEER_FLIP(void);
void SettingsSetINTSTEER_FLIP(uint8_t value);


/// Load default settings and force a save
void load_eeprom_defaults(void);

/// @} // endgroup settings

#endif  // #ifndef __SETTINGS_H__
