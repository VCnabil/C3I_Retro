//------------------------------------------------------------------------------
//  TITLE :          settings.c
//------------------------------------------------------------------------------

#include "project.h"
static void _SettingsLoadDefaults(void);

#define CRC_START_VALUE 0x1D0F

// This is the structure for storing parameters in EEPROM.
typedef struct
{
	///////////////////////////////////////////////////////////////////////////
	// SYSTEM SECTION - DO NOT MODIFY - THESE 2 ENTRIES _MUST_ BE FIRST!
	///////////////////////////////////////////////////////////////////////////
	uint32_t settingsCRC;
	uint32_t settingsSaveCount;

	///////////////////////////////////////////////////////////////////////////
	// Default User Settings
	///////////////////////////////////////////////////////////////////////////
	// Selected language
	uint32_t language;

	// Key bleep on/off
	BOOL isBuzzerMuted;

	///////////////////////////////////////////////////////////////////////////
	// You _MUST_ add any new settings to be saved _AFTER_ this comment...
	///////////////////////////////////////////////////////////////////////////

	// Units
	int8_t	PressureUnits;
	int8_t	TemperatureUnits;
	int8_t	VolumeUnits;
	int8_t	DistanceUnits;
	int8_t	FuelRateUnits;
	int8_t	SpeedUnits;
	int8_t	DepthUnits;
	int8_t	AngleUnits;
	int8_t	BearingUnits;
	int8_t	EconomyUnits;
	int8_t	TimeUnits;

	// Device and System Instance found in the Name PGN
	uint8_t CANSystemInstance[CAN_MAX_PORTS];
	uint8_t CANDeviceInstance[CAN_MAX_PORTS];

	int32_t testSave;

	 //VECTOR EEPROM PARAMETERS..........................................................
	uint8_t PN_MAX;  //port nozzle max sensor value
	uint8_t PN_MIN;  //port nozzle min sensor value
	uint8_t SN_MAX;  //stbd nozzle max sensor value
	uint8_t SN_MIN;  //stbd nozzle min sensor value
	uint8_t PB_MAX;  //port bucket max sensor value
	uint8_t PB_MIN;  //port bucket min sensor value
	uint8_t SB_MAX;  //stbd bucket max sensor value
	uint8_t SB_MIN;  //stbd bucket min sensor value
	uint8_t PT_MAX;  //port trimtab max sensor value
	uint8_t PT_MIN;  //port trimtab min sensor value
	uint8_t ST_MAX;  //stbd trimtab max sensor value
	uint8_t ST_MIN;  //stbd trimtab min sensor value
	uint8_t PB_NEUTRAL_THRUST;  //port bucket neutral thrust value
	uint8_t SB_NEUTRAL_THRUST;  //stbd bucket neutral thrust value

	uint8_t DATA_MODE;
	uint8_t SERIALXMIT_FlIP;
	uint8_t NOZZLEMAP_FLIP;
	uint8_t BUCKETMAP_FLIP;
	uint8_t INTMAP_FLIP;
	uint8_t INTSTEER_FLIP;
	uint8_t STATION_TYPE;
	uint8_t COMMS_MODE;                        
	uint8_t TABMAP_FLIP;


} SETTINGS_T;

//------------------------------------------------------------------------------
// MODULE GLOBALS
//------------------------------------------------------------------------------

// Instances of settings structure
static SETTINGS_T m_settings;
static const void * m_crcStartingPoint = (char*)&m_settings.settingsCRC + sizeof(m_settings.settingsCRC);
static const uint32_t m_crcLength = sizeof(m_settings) - sizeof(m_settings.settingsCRC);

//------------------------------------------------------------------------------
// LOCAL FUNCTIONS
//------------------------------------------------------------------------------

static void SettingsSaveCallback(void* pUserData = nullptr);

static void SettingsSaveCallback(void* pUserData)
{
	(void)pUserData;
	SettingsSave();
}

//------------------------------------------------------------------------------
// PUBLIC FUNCTIONS
//------------------------------------------------------------------------------

void SettingsInit(void)
{
	// Register a 1 minute timer to automatically save the settings
	timer_register(SettingsSaveCallback, nullptr, 1000 * 60, 1000 * 60);

	// Load the settings
	SettingsLoad();
}

void SettingsLoad(void)
{
	if (eepromRead(EEPROMSimulated, 0, (uint8_t *)&m_settings, sizeof(m_settings)) == 0)
	{
		m_settings.settingsSaveCount = 0;
		load_eeprom_defaults();
	}
	else
	{
		if (m_settings.settingsCRC != crc16(CRC_START_VALUE, m_crcStartingPoint, m_crcLength))
		{
			// CRC Failed.
			m_settings.settingsSaveCount = 0;
			load_eeprom_defaults();
		}
	}

	// Initialise the key press beep state from settings
	buzzer_mute(m_settings.isBuzzerMuted);
}

void SettingsSave(void)
{
	uint16_t settingsCRC = crc16(CRC_START_VALUE, m_crcStartingPoint, m_crcLength);
	uint16_t savedDataCRC = 0;

	SETTINGS_T currentSavedData;

	uint32_t bytesRead = eepromRead(EEPROMSimulated, 0, (uint8_t *)&currentSavedData, sizeof(currentSavedData));

	if (bytesRead != 0)
	{
		savedDataCRC = currentSavedData.settingsCRC;
	}

	if (savedDataCRC != settingsCRC || bytesRead == 0)
	{
		// CRC's are different, save the data.
		m_settings.settingsSaveCount++;
		m_settings.settingsCRC = crc16(CRC_START_VALUE, m_crcStartingPoint, m_crcLength);
		eepromWrite(EEPROMSimulated, 0, (uint8_t *)&m_settings, sizeof(m_settings));
	}
}

uint32_t SettingsGetLanguageIndex(void)
{
	return m_settings.language;
}

void SettingsSetLanguageIndex(uint32_t languageIndex)
{
	// Changed?
	if (languageIndex != m_settings.language)
	{
		// Yes. Save
		m_settings.language = languageIndex;

		SettingsSave();
	}
}

BOOL SettingsIsBleepEnabled(void)
{
	return m_settings.isBuzzerMuted;
}

void SettingsToggleBleep(void)
{
	if (m_settings.isBuzzerMuted)
	{
		m_settings.isBuzzerMuted = FALSE;
	}
	else
	{
		m_settings.isBuzzerMuted = TRUE;
	}
	buzzer_mute(m_settings.isBuzzerMuted);

	SettingsSave();
}

void SettingsUnitSetPressureUnits(uint8_t NewUnits)
{
	if (NewUnits <= UnitGetMaxValid(DBUNITS_PRESSURE))
	{
		m_settings.PressureUnits = NewUnits;
		SettingsSave();
	}
}

void SettingsUnitSetTemperatureUnits(uint8_t NewUnits)
{
	if (NewUnits <= UnitGetMaxValid(DBUNITS_TEMPERATURE))
	{
		m_settings.TemperatureUnits = NewUnits;
		SettingsSave();
	}
}

void SettingsUnitSetVolumeUnits(uint8_t NewUnits)
{
	if (NewUnits <= UnitGetMaxValid(DBUNITS_VOLUME))
	{
		m_settings.VolumeUnits = NewUnits;
		SettingsSave();
	}
}

void SettingsUnitSetDistanceUnits(uint8_t NewUnits)
{
	if (NewUnits <= UnitGetMaxValid(DBUNITS_DISTANCE))
	{
		m_settings.DistanceUnits = NewUnits;
		SettingsSave();
	}
}

void SettingsUnitSetFuelRateUnits(uint8_t NewUnits)
{
	if (NewUnits <= UnitGetMaxValid(DBUNITS_FLOW_RATE))
	{
		m_settings.FuelRateUnits = NewUnits;
		SettingsSave();
	}
}

void SettingsUnitSetSpeedUnits(uint8_t NewUnits)
{
	if (NewUnits <= UnitGetMaxValid(DBUNITS_SPEED))
	{
		m_settings.SpeedUnits = NewUnits;
		SettingsSave();
	}
}

void SettingsUnitSetDepthUnits(uint8_t NewUnits)
{
	if (NewUnits <= UnitGetMaxValid(DBUNITS_DEPTH))
	{
		m_settings.DepthUnits = NewUnits;
		SettingsSave();
	}
}

void SettingsUnitSetAngleUnits(uint8_t NewUnits)
{
	if (NewUnits <= UnitGetMaxValid(DBUNITS_ANGLE))
	{
		m_settings.AngleUnits = NewUnits;
		SettingsSave();
	}
}

void SettingsUnitSetBearingUnits(uint8_t NewUnits)
{
	if (NewUnits <= UnitGetMaxValid(DBUNITS_BEARING))
	{
		m_settings.BearingUnits = NewUnits;
		SettingsSave();
	}
}

void SettingsUnitSetEconomyUnits(uint8_t NewUnits)
{
	if (NewUnits <= UnitGetMaxValid(DBUNITS_ECONOMY))
	{
		m_settings.EconomyUnits = NewUnits;
		SettingsSave();
	}
}

void SettingsUnitSetTimeUnits(uint8_t NewUnits)
{
	if (NewUnits <= UnitGetMaxValid(DBUNITS_TIME))
	{
		m_settings.TimeUnits = NewUnits;
		SettingsSave();
	}
}

void SettingsUnitsTogglePressure(void* userData)
{
	if (SettingsUnitGetPressureUnits() == UnitGetMaxValid(DBUNITS_PRESSURE))
	{
		SettingsUnitSetPressureUnits(0);
	}
	else
	{
		uint8_t CurrentUnits = SettingsUnitGetPressureUnits();
		SettingsUnitSetPressureUnits(++CurrentUnits);
	}
}

void SettingsUnitsToggleTemperature(void* userData)
{
	if (SettingsUnitGetTemperatureUnits() == UnitGetMaxValid(DBUNITS_TEMPERATURE))
	{
		SettingsUnitSetTemperatureUnits(0);
	}
	else
	{
		uint8_t CurrentUnits = SettingsUnitGetTemperatureUnits();
		SettingsUnitSetTemperatureUnits(++CurrentUnits);
	}
}

void SettingsUnitsToggleVolume(void* userData)
{
	if (SettingsUnitGetVolumeUnits() == UnitGetMaxValid(DBUNITS_VOLUME))
	{
		SettingsUnitSetVolumeUnits(0);
	}
	else
	{
		uint8_t CurrentUnits = SettingsUnitGetVolumeUnits();
		SettingsUnitSetVolumeUnits(++CurrentUnits);
	}
}

void SettingsUnitsToggleDistance(void* userData)
{
	if (SettingsUnitGetDistanceUnits() == UnitGetMaxValid(DBUNITS_DISTANCE))
	{
		SettingsUnitSetDistanceUnits(0);
	}
	else
	{
		uint8_t CurrentUnits = SettingsUnitGetDistanceUnits();
		SettingsUnitSetDistanceUnits(++CurrentUnits);
	}
}

void SettingsUnitsToggleFuelRate(void* userData)
{
	if (SettingsUnitGetFuelRateUnits() == UnitGetMaxValid(DBUNITS_FLOW_RATE))
	{
		SettingsUnitSetFuelRateUnits(0);
	}
	else
	{
		uint8_t CurrentUnits = SettingsUnitGetFuelRateUnits();
		SettingsUnitSetFuelRateUnits(++CurrentUnits);
	}
}

void SettingsUnitsToggleSpeed(void* userData)
{
	if (SettingsUnitGetSpeedUnits() == UnitGetMaxValid(DBUNITS_SPEED))
	{
		SettingsUnitSetSpeedUnits(0);
	}
	else
	{
		uint8_t CurrentUnits = SettingsUnitGetSpeedUnits();
		SettingsUnitSetSpeedUnits(++CurrentUnits);
	}
}

void SettingsUnitsToggleDepth(void* userData)
{
	if (SettingsUnitGetDepthUnits() == UnitGetMaxValid(DBUNITS_DEPTH))
	{
		SettingsUnitSetDepthUnits(0);
	}
	else
	{
		uint8_t CurrentUnits = SettingsUnitGetDepthUnits();
		SettingsUnitSetDepthUnits(++CurrentUnits);
	}
}

void SettingsUnitsToggleAngle(void* userData)
{
	if (SettingsUnitGetAngleUnits() == UnitGetMaxValid(DBUNITS_ANGLE))
	{
		SettingsUnitSetAngleUnits(0);
	}
	else
	{
		uint8_t CurrentUnits = SettingsUnitGetAngleUnits();
		SettingsUnitSetAngleUnits(++CurrentUnits);
	}
}

void SettingsUnitsToggleBearing(void* userData)
{
	if (SettingsUnitGetBearingUnits() == UnitGetMaxValid(DBUNITS_BEARING))
	{
		SettingsUnitSetBearingUnits(0);
	}
	else
	{
		uint8_t CurrentUnits = SettingsUnitGetBearingUnits();
		SettingsUnitSetBearingUnits(++CurrentUnits);
	}
}

void SettingsUnitsToggleEconomy(void* userData)
{
	if (SettingsUnitGetEconomyUnits() == UnitGetMaxValid(DBUNITS_ECONOMY))
	{
		SettingsUnitSetEconomyUnits(0);
	}
	else
	{
		uint8_t CurrentUnits = SettingsUnitGetEconomyUnits();
		SettingsUnitSetEconomyUnits(++CurrentUnits);
	}
}

void SettingsUnitsToggleTime(void* userData)
{
	if (SettingsUnitGetTimeUnits() == UnitGetMaxValid(DBUNITS_TIME))
	{
		SettingsUnitSetTimeUnits(0);
	}
	else
	{
		uint8_t CurrentUnits = SettingsUnitGetTimeUnits();
		SettingsUnitSetTimeUnits(++CurrentUnits);
	}
}

uint8_t SettingsUnitGetPressureUnits(void)
{
	return m_settings.PressureUnits;
}

uint8_t SettingsUnitGetTemperatureUnits(void)
{
	return m_settings.TemperatureUnits;
}

uint8_t SettingsUnitGetVolumeUnits(void)
{
	return m_settings.VolumeUnits;
}

uint8_t SettingsUnitGetDistanceUnits(void)
{
	return m_settings.DistanceUnits;
}

uint8_t SettingsUnitGetFuelRateUnits(void)
{
	return m_settings.FuelRateUnits;
}

uint8_t SettingsUnitGetSpeedUnits(void)
{
	return m_settings.SpeedUnits;
}

uint8_t SettingsUnitGetDepthUnits(void)
{
	return m_settings.DepthUnits;
}

uint8_t SettingsUnitGetAngleUnits(void)
{
	return m_settings.AngleUnits;
}

uint8_t SettingsUnitGetBearingUnits(void)
{
	return m_settings.BearingUnits;
}

uint8_t SettingsUnitGetEconomyUnits(void)
{
	return m_settings.EconomyUnits;
}

uint8_t SettingsUnitGetTimeUnits(void)
{
	return m_settings.TimeUnits;
}

uint8_t SettingsUnitGetUnits(uint32_t UnitType)
{
	int8_t ReturnValue = 0;

	switch (UnitType)
	{
	case DBUNITS_PRESSURE:
		ReturnValue = SettingsUnitGetPressureUnits();
		break;
	case DBUNITS_TEMPERATURE:
		ReturnValue = SettingsUnitGetTemperatureUnits();
		break;
	case DBUNITS_VOLUME:
		ReturnValue = SettingsUnitGetVolumeUnits();
		break;
	case DBUNITS_DISTANCE:
		ReturnValue = SettingsUnitGetDistanceUnits();
		break;
	case DBUNITS_FLOW_RATE:
		ReturnValue = SettingsUnitGetFuelRateUnits();
		break;
	case DBUNITS_SPEED:
		ReturnValue = SettingsUnitGetSpeedUnits();
		break;
	case DBUNITS_DEPTH:
		ReturnValue = SettingsUnitGetDepthUnits();
		break;
	case DBUNITS_BEARING:
		ReturnValue = SettingsUnitGetBearingUnits();
		break;
	case DBUNITS_ANGLE:
		ReturnValue = SettingsUnitGetAngleUnits();
		break;
	case DBUNITS_ECONOMY:
		ReturnValue = SettingsUnitGetEconomyUnits();
		break;
	case DBUNITS_TIME:
		ReturnValue = SettingsUnitGetTimeUnits();
		break;
	case DBUNITS_PERCENT:
	case DBUNITS_RPM:
	case DBUNITS_ELECTRICAL:
	case DBUNITS_COMMAND:
	case DBUNITS_CURRENT:
	case DBUNITS_FREQUENCY:
	case DBUNITS_REVS:
	case DBUNITS_NONE:
	case DBUNITS_RESISTANCE:
		ReturnValue = 0;
		break;

	case DBUNITS_END:
	default:
		assert("Missing Units!" && FALSE);
		break;
	}

	return ReturnValue;
}

uint8_t SettingsGetCANSystemInstance(uint32_t CANPort)
{
	if (CANPort < CAN_MAX_PORTS)
	{
		return m_settings.CANSystemInstance[CANPort];
	}
	else
	{
		return 0;
	}
}

void SettingsSetCANSystemInstance(uint32_t CANPort, uint8_t systemInstance)
{
	if (CANPort < CAN_MAX_PORTS)
	{
		m_settings.CANSystemInstance[CANPort] = systemInstance;
		SettingsSave();
	}
	else
	{
		assert("Index out of range!" && FALSE);
	}
}

uint8_t SettingsGetCANDeviceInstance(uint32_t CANPort)
{
	if (CANPort < CAN_MAX_PORTS)
	{
		return m_settings.CANDeviceInstance[CANPort];
	}
	else
	{
		return 0;
	}
}

void SettingsSetCANDeviceInstance(uint32_t CANPort, uint8_t deviceInstance)
{
	if (CANPort < CAN_MAX_PORTS)
	{
		m_settings.CANDeviceInstance[CANPort] = deviceInstance;
		SettingsSave();
	}
	else
	{
		assert("Index out of range!" && FALSE);
	}
}


int32_t SettingsGetTestSave(void)
{
	return m_settings.testSave;
}

void SettingsSetTestSave(int32_t value)
{
	if (m_settings.testSave != value)
	{
		m_settings.testSave = value;
		SettingsSave();
	}
}

 // setter and getters for VECTOR EEPROM PARAMETERS.......................................................... 
uint8_t SettingsGetPN_MAX(void)
{
	return m_settings.PN_MAX;
}

void SettingsSetPN_MAX(uint8_t value)
{
	if (m_settings.PN_MAX != value)
	{
		m_settings.PN_MAX = value;
		SettingsSave();
	}
}

uint8_t SettingsGetPN_MIN(void)
{
	return m_settings.PN_MIN;
}

void SettingsSetPN_MIN(uint8_t value)
{
	if (m_settings.PN_MIN != value)
	{
		m_settings.PN_MIN = value;
		SettingsSave();
	}
}

uint8_t SettingsGetSN_MAX(void)
{
	return m_settings.SN_MAX;
}

void SettingsSetSN_MAX(uint8_t value)
{
	if (m_settings.SN_MAX != value)
	{
		m_settings.SN_MAX = value;
		SettingsSave();
	}
}

uint8_t SettingsGetSN_MIN(void)
{
	return m_settings.SN_MIN;
}

void SettingsSetSN_MIN(uint8_t value)
{
	if (m_settings.SN_MIN != value)
	{
		m_settings.SN_MIN = value;
		SettingsSave();
	}
}

uint8_t SettingsGetPB_MAX(void)
{
	return m_settings.PB_MAX;
}

void SettingsSetPB_MAX(uint8_t value)
{
	if (m_settings.PB_MAX != value)
	{
		m_settings.PB_MAX = value;
		SettingsSave();
	}
}

uint8_t SettingsGetPB_MIN(void)
{
	return m_settings.PB_MIN;
}

void SettingsSetPB_MIN(uint8_t value)
{
	if (m_settings.PB_MIN != value)
	{
		m_settings.PB_MIN = value;
		SettingsSave();
	}
}

uint8_t SettingsGetSB_MAX(void)
{
	return m_settings.SB_MAX;
}

void SettingsSetSB_MAX(uint8_t value)
{
	if (m_settings.SB_MAX != value)
	{
		m_settings.SB_MAX = value;
		SettingsSave();
	}
}

uint8_t SettingsGetSB_MIN(void)
{
	return m_settings.SB_MIN;
}

void SettingsSetSB_MIN(uint8_t value)
{
	if (m_settings.SB_MIN != value)
	{
		m_settings.SB_MIN = value;
		SettingsSave();
	}
}

uint8_t SettingsGetPT_MAX(void)
{
	return m_settings.PT_MAX;
}

void SettingsSetPT_MAX(uint8_t value)
{
	if (m_settings.PT_MAX != value)
	{
		m_settings.PT_MAX = value;
		SettingsSave();
	}
}

uint8_t SettingsGetPT_MIN(void)
{
	return m_settings.PT_MIN;
}

void SettingsSetPT_MIN(uint8_t value)
{
	if (m_settings.PT_MIN != value)
	{
		m_settings.PT_MIN = value;
		SettingsSave();
	}
}

uint8_t SettingsGetST_MAX(void)
{
	return m_settings.ST_MAX;
}
	
void SettingsSetST_MAX(uint8_t value)
{
	if (m_settings.ST_MAX != value)
	{
		m_settings.ST_MAX = value;
		SettingsSave();
	}
}

uint8_t SettingsGetST_MIN(void)
{
	return m_settings.ST_MIN;
}

void SettingsSetST_MIN(uint8_t value)
{
	if (m_settings.ST_MIN != value)
	{
		m_settings.ST_MIN = value;
		SettingsSave();
	}
}

uint8_t SettingsGetPB_NEUTRAL_THRUST(void)
{
	return m_settings.PB_NEUTRAL_THRUST;
}

void SettingsSetPB_NEUTRAL_THRUST(uint8_t value)
{
	if (m_settings.PB_NEUTRAL_THRUST != value)
	{
		m_settings.PB_NEUTRAL_THRUST = value;
		SettingsSave();
	}
}

uint8_t SettingsGetSB_NEUTRAL_THRUST(void)
{
	return m_settings.SB_NEUTRAL_THRUST; 
}

void SettingsSetSB_NEUTRAL_THRUST(uint8_t value)
{
	if (m_settings.SB_NEUTRAL_THRUST != value)
	{
		m_settings.SB_NEUTRAL_THRUST = value;
		SettingsSave();
	}
}


//lcd settings for the vector

uint8_t SettingsGetDATA_MODE(void)
{
	return m_settings.DATA_MODE;
}

void SettingsSetDATA_MODE(uint8_t value)
{
	if (m_settings.DATA_MODE != value)
	{
		m_settings.DATA_MODE = value;
		SettingsSave();
	}
}

uint8_t SettingsGetSERIALXMIT_FlIP(void)
{
	return m_settings.SERIALXMIT_FlIP;
}

void SettingsSetSERIALXMIT_FlIP(uint8_t value)
{
	if (m_settings.SERIALXMIT_FlIP != value)
	{
		m_settings.SERIALXMIT_FlIP = value;
		SettingsSave();
	}
}

uint8_t SettingsGetNOZZLEMAP_FLIP(void)
{
	return m_settings.NOZZLEMAP_FLIP;
}

void SettingsSetNOZZLEMAP_FLIP(uint8_t value)
{
	if (m_settings.NOZZLEMAP_FLIP != value)
	{
		m_settings.NOZZLEMAP_FLIP = value;
		SettingsSave();
	}
}

uint8_t SettingsGetBUCKETMAP_FLIP(void)
{
	return m_settings.BUCKETMAP_FLIP;
}

void SettingsSetBUCKETMAP_FLIP(uint8_t value)
{
	if (m_settings.BUCKETMAP_FLIP != value)
	{
		m_settings.BUCKETMAP_FLIP = value;
		SettingsSave();
	}
}

uint8_t SettingsGetINTMAP_FLIP(void)
{
	return m_settings.INTMAP_FLIP;
}

void SettingsSetINTMAP_FLIP(uint8_t value)
{
	if (m_settings.INTMAP_FLIP != value)
	{
		m_settings.INTMAP_FLIP = value;
		SettingsSave();
	}
}

uint8_t SettingsGetINTSTEER_FLIP(void)
{
	return m_settings.INTSTEER_FLIP;
}

void SettingsSetINTSTEER_FLIP(uint8_t value)
{
	if (m_settings.INTSTEER_FLIP != value)
	{
		m_settings.INTSTEER_FLIP = value;
		SettingsSave();
	}
}

uint8_t SettingsGetSTATION_TYPE(void)
{
	return m_settings.STATION_TYPE;
}
	
void SettingsSetSTATION_TYPE(uint8_t value)
{
	if (m_settings.STATION_TYPE != value)
	{
		m_settings.STATION_TYPE = value;
		SettingsSave();
	}
}

uint8_t SettingsGetCOMMS_MODE(void)
{
	return m_settings.COMMS_MODE;
}

void SettingsSetCOMMS_MODE(uint8_t value)
{
	if (m_settings.COMMS_MODE != value)
	{
		m_settings.COMMS_MODE = value;
		SettingsSave();
	}
}


uint8_t SettingsGetTABMAP_FLIP(void)
{
	return m_settings.TABMAP_FLIP;
}

void SettingsSetTABMAP_FLIP(uint8_t value)
{
	if (m_settings.TABMAP_FLIP != value)
	{
		m_settings.TABMAP_FLIP = value;
		SettingsSave();
	}
}



// Load default settings to EEPROM and force a save
// This is called from syslib if the settings are being reset to defaults
void load_eeprom_defaults(void)
{
    // Set all the other defaults
    _SettingsLoadDefaults();

    // Force a save of the settings
    SettingsSave();

    // Make a 500ms beep
    buzzer_out_timed(500);
}

// Load settings defaults
static void _SettingsLoadDefaults(void)
{
	// Buzzer not muted
	m_settings.isBuzzerMuted = FALSE;                                      
	buzzer_mute(m_settings.isBuzzerMuted);

	m_settings.language = 0;

	m_settings.PressureUnits = 0;
	m_settings.TemperatureUnits = 0;
	m_settings.VolumeUnits = 0;
	m_settings.DistanceUnits = 0;
	m_settings.FuelRateUnits = 0;
	m_settings.SpeedUnits = 0;
	m_settings.DepthUnits = 0;
	m_settings.AngleUnits = 0;
	m_settings.BearingUnits = 0;
	m_settings.EconomyUnits = 0;
	m_settings.TimeUnits = 1;

	m_settings.CANDeviceInstance[CAN_PORT1] = 0;
	m_settings.CANSystemInstance[CAN_PORT1] = 0;
	m_settings.CANDeviceInstance[CAN_PORT2] = 0;
	m_settings.CANSystemInstance[CAN_PORT2] = 0;

	m_settings.testSave = 1;
	m_settings.PN_MAX = 0;
	m_settings.PN_MIN = 0;
	m_settings.SN_MAX = 0;
	m_settings.SN_MIN = 0;
	m_settings.PB_MAX = 0;
	m_settings.PB_MIN = 0;
	m_settings.SB_MAX = 0;
	m_settings.SB_MIN = 0;
	m_settings.PT_MAX = 0;
	m_settings.PT_MIN = 0;
	m_settings.ST_MAX = 0;
	m_settings.ST_MIN = 0;
	m_settings.PB_NEUTRAL_THRUST = 0;
	m_settings.SB_NEUTRAL_THRUST = 0;

	m_settings.DATA_MODE = 0;
	m_settings.NOZZLEMAP_FLIP = 0;
	m_settings.BUCKETMAP_FLIP = 0;
	m_settings.INTMAP_FLIP = 0;
	m_settings.INTSTEER_FLIP = 1;
	m_settings.STATION_TYPE = 1;
	m_settings.COMMS_MODE = 0;

	m_settings.TABMAP_FLIP = 0;
}
