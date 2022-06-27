#ifndef _OBDII_PID_TABLE__
#define _OBDII_PID_TABLE__


#define BYTE 		unsigned char
#define WORD 		unsigned short

typedef struct {
	BYTE 		pid;
	char 		*EventID;
	char 		period;
	int 		sampleRate;
} OBD_PID_TABLE, *pOBD_PID_TABLE;

typedef enum {
	P_ONBOOT = 0x01,
	P_PERIODIC = 0x02,
	P_ONOCCURANCE = 0x03,

	P_UNKNOWN = 0x7f
} OBD_PERIOD_E;

typedef union {
	float 		fVer;
	char		bVer[sizeof(float)];
} OBD_FLT_CHAR;


const OBD_PID_TABLE 	mode12table[] = {
	{ 0x00, "Capa12_00", 									P_PERIODIC, 		0 },
	{ 0x01, "MILStatus", 									P_ONOCCURANCE, 		20000 },
	{ 0x02, NULL,											P_UNKNOWN, 			0 },
	{ 0x03, NULL,											P_UNKNOWN,			0 },
	{ 0x04, "CalculatedEngineLoadValue",					P_PERIODIC,			0 },
	{ 0x05, "EngineCoolantTemperature",						P_PERIODIC,			20000 },
	{ 0x06, NULL,											P_UNKNOWN,			0 },
	{ 0x07, NULL,											P_UNKNOWN,			0 },
	{ 0x08, NULL,											P_UNKNOWN,			0 },
	{ 0x09, NULL,											P_UNKNOWN,			0 },
	{ 0x0A, NULL,											P_UNKNOWN,			0 },
	{ 0x0B, NULL,											P_UNKNOWN,			0 },
	{ 0x0C, "EngineRPM",									P_PERIODIC,			20000 },
	{ 0x0D, "VehicleSpeed",									P_PERIODIC,			250 },
	{ 0x0E, NULL,											P_UNKNOWN,			0 },
	{ 0x0F, NULL,											P_UNKNOWN,			0 },
	{ 0x10, NULL,											P_UNKNOWN,			0 },
	{ 0x11, "AbsoluteThrottlePosition",						P_PERIODIC,			0 },
	{ 0x12, NULL,											P_UNKNOWN,			0 },
	{ 0x13, "TotalEngineRunTime",							P_PERIODIC,			3600000 },
	{ 0x14, NULL,											P_UNKNOWN,			0 },
	{ 0x15, NULL,											P_UNKNOWN,			0 },
	{ 0x16, NULL,											P_UNKNOWN,			0 },
	{ 0x17, NULL,											P_UNKNOWN,			0 },
	{ 0x18, NULL,											P_UNKNOWN,			0 },
	{ 0x19, NULL,											P_UNKNOWN,			0 },
	{ 0x1A, NULL,											P_UNKNOWN,			0 },
	{ 0x1B, NULL,											P_UNKNOWN,			0 },
	{ 0x1C, NULL,											P_UNKNOWN,			0 },
	{ 0x1D, NULL,											P_UNKNOWN,			0 },
	{ 0x1E, NULL,											P_UNKNOWN,			0 },
	{ 0x1F, "TimeSinceEngineStart",							P_PERIODIC,			60000 },
	{ 0x20, "Capa12_20",									P_PERIODIC,			0 },
	{ 0x21, "DistanceTravelledWhileMILisActivated",			P_PERIODIC,			60000 },
	{ 0x22, NULL,											P_UNKNOWN,			0 },
	{ 0x23, "FuelRailPressureGauge",						P_PERIODIC,			0 },
	{ 0x24, NULL,											P_UNKNOWN,			0 },
	{ 0x25, NULL,											P_UNKNOWN,			0 },
	{ 0x26, NULL,											P_UNKNOWN,			0 },
	{ 0x27, NULL,											P_UNKNOWN,			0 },
	{ 0x28, NULL,											P_UNKNOWN,			0 },
	{ 0x29, NULL,											P_UNKNOWN,			0 },
	{ 0x2A, NULL,											P_UNKNOWN,			0 },
	{ 0x2B, NULL,											P_UNKNOWN,			0 },
	{ 0x2C, "CommandedEGR",									P_PERIODIC,			0 },
	{ 0x2D, NULL,											P_UNKNOWN,			0 },
	{ 0x2E, NULL,											P_UNKNOWN,			0 },
	{ 0x2F, "FuelLevel",									P_PERIODIC,			60000 },
	{ 0x30, "WarmupSinceDTCCleared",						P_PERIODIC,			0 },
	{ 0x31, "DistancesinceDTCscleared",						P_PERIODIC,			0 },
	{ 0x32, NULL,											P_UNKNOWN,			0 },
	{ 0x33, "AbsoluteBarometricPressure",					P_PERIODIC,			0 },
	{ 0x34, NULL,											P_UNKNOWN,			0 },
	{ 0x35, NULL,											P_UNKNOWN,			0 },
	{ 0x36, NULL,											P_UNKNOWN,			0 },
	{ 0x37, NULL,											P_UNKNOWN,			0 },
	{ 0x38, NULL,											P_UNKNOWN,			0 },
	{ 0x39, NULL,											P_UNKNOWN,			0 },
	{ 0x3A, NULL,											P_UNKNOWN,			0 },
	{ 0x3B, NULL,											P_UNKNOWN,			0 },
	{ 0x3C, "CatalystTempB1S1",								P_PERIODIC,			0 },
	{ 0x3D, NULL,											P_UNKNOWN,			0 },
	{ 0x3E, "CatalystTempB1S2",								P_PERIODIC,			0 },
	{ 0x3F, NULL,											P_UNKNOWN,			0 },
	{ 0x40, "Capa12_40",									P_PERIODIC,			0 },
	{ 0x41, NULL,											P_UNKNOWN,			0 },
	{ 0x42, "ControlModuleVoltage",							P_PERIODIC,			0 },
	{ 0x43, NULL,											P_UNKNOWN,			0 },
	{ 0x44, NULL,											P_UNKNOWN,			0 },
	{ 0x45, "RelativeThrottlePosition",						P_PERIODIC,			0 },
	{ 0x46, "AmbientAirTemp",								P_PERIODIC,			900000 },
	{ 0x47, NULL,											P_UNKNOWN,			0 },
	{ 0x48, NULL,											P_UNKNOWN,			0 },
	{ 0x49, "AcceleratorPedalPositionD",					P_PERIODIC,			0 },
	{ 0x4A, "AcceleratorPedalPositionE",					P_PERIODIC,			0 },
	{ 0x4B, NULL,											P_UNKNOWN,			0 },
	{ 0x4C, NULL,											P_UNKNOWN,			0 },
	{ 0x4D, "TimeRunWithMILON",								P_PERIODIC,			0 },
	{ 0x4E, "TimesinceDTCscleared",							P_PERIODIC,			60000 },
	{ 0x4F, "MaxValueForFuel",								P_PERIODIC,			0 },
	{ 0x50, "MaxAirFlowRate",								P_PERIODIC,			0 },
	{ 0x51, "Typeoffuel",									P_ONBOOT,			0 },
	{ 0x52, NULL,											P_UNKNOWN,			0 },
	{ 0x53, NULL,											P_UNKNOWN,			0 },
	{ 0x54, NULL,											P_UNKNOWN,			0 },
	{ 0x55, NULL,											P_UNKNOWN,			0 },
	{ 0x56, NULL,											P_UNKNOWN,			0 },
	{ 0x57, NULL,											P_UNKNOWN,			0 },
	{ 0x58, NULL,											P_UNKNOWN,			0 },
	{ 0x59, NULL,											P_UNKNOWN,			0 },
	{ 0x5A, "RelativeAcceleratorPedalPosition",				P_PERIODIC,			0 },
	{ 0x5B, NULL,											P_UNKNOWN,			0 },
	{ 0x5C, "EngineOilTemperature",							P_PERIODIC,			60000 },
	{ 0x5D, NULL,											P_UNKNOWN,			0 },
	{ 0x5E, "EngineFuelRate",								P_PERIODIC,			20000 },
	{ 0x5F, NULL,											P_UNKNOWN,			0 },
	{ 0x60, "Capa12_60",									P_PERIODIC,			0 },
	{ 0x61, NULL,											P_UNKNOWN,			0 },
	{ 0x62, NULL,											P_UNKNOWN,			0 },
	{ 0x63, "EngineReferenceTorque",						P_PERIODIC,			0 },
	{ 0x64, NULL,											P_UNKNOWN,			0 },
	{ 0x65, NULL,											P_UNKNOWN,			0 },
	{ 0x66, "MassAirFlowSensors",							P_PERIODIC,			20000 },
	{ 0x67, NULL,											P_UNKNOWN,			0 },
	{ 0x68, "IntakeAirTemperature",							P_PERIODIC,			0 },
	{ 0x69, NULL,											P_UNKNOWN,			0 },
	{ 0x6A, NULL,											P_UNKNOWN,			0 },
	{ 0x6B, NULL,											P_UNKNOWN,			0 },
	{ 0x6C, "CommandedThrottleActuatorControl",				P_PERIODIC,			0 },
	{ 0x6D, NULL,											P_UNKNOWN,			0 },
	{ 0x6E, NULL,											P_UNKNOWN,			0 },
	{ 0x6F, NULL,											P_UNKNOWN,			0 },
	{ 0x70, NULL,											P_UNKNOWN,			0 },
	{ 0x71, NULL,											P_UNKNOWN,			0 },
	{ 0x72, NULL,											P_UNKNOWN,			0 },
	{ 0x73, NULL,											P_UNKNOWN,			0 },
	{ 0x74, NULL,											P_UNKNOWN,			0 },
	{ 0x75, NULL,											P_UNKNOWN,			0 },
	{ 0x76, NULL,											P_UNKNOWN,			0 },
	{ 0x77, NULL,											P_UNKNOWN,			0 },
	{ 0x78, NULL,											P_UNKNOWN,			0 },
	{ 0x79, NULL,											P_UNKNOWN,			0 },
	{ 0x7A, NULL,											P_UNKNOWN,			0 },
	{ 0x7B, NULL,											P_UNKNOWN,			0 },
	{ 0x7C, NULL,											P_UNKNOWN,			0 },
	{ 0x7D, NULL,											P_UNKNOWN,			0 },
	{ 0x7E, NULL,											P_UNKNOWN,			0 },
	{ 0x7F, NULL,											P_UNKNOWN,			0 },
	{ 0x80, "Capa12_80",									P_PERIODIC,			0 },
	{ 0x81, NULL,											P_UNKNOWN,			0 },
	{ 0x82, NULL,											P_UNKNOWN,			0 },
	{ 0x83, NULL,											P_UNKNOWN,			0 },
	{ 0x84, NULL,											P_UNKNOWN,			0 },
	{ 0x85, NULL,											P_UNKNOWN,			0 },
	{ 0x86, "PMSensorMassConcentration",					P_PERIODIC,			900000 },
	{ 0x87, "IntakeManifoldAbsolutePressureA",				P_PERIODIC,			0 },

	{ 0xFF, NULL,											P_UNKNOWN,			-1 },

};



const OBD_PID_TABLE 	mode9table[] = {
	{ 0x00, "Capa9_00", 									P_UNKNOWN, 			0 },
	{ 0x01, NULL, 											P_UNKNOWN, 			0 },
	{ 0x02, "VIN",											P_ONBOOT, 			0 },
	{ 0x03, NULL,											P_UNKNOWN,			0 },
	{ 0x04, NULL,											P_UNKNOWN,			0 },
	{ 0x05, NULL,											P_UNKNOWN,			0 },
	{ 0x06, NULL,											P_UNKNOWN,			0 },
	{ 0x07, NULL,											P_UNKNOWN,			0 },
	{ 0x08, NULL,											P_UNKNOWN,			0 },
	{ 0x09, NULL,											P_UNKNOWN,			0 },
	{ 0x0A, NULL,											P_UNKNOWN,			0 },
	{ 0x0B, NULL,											P_UNKNOWN,			0 },
	{ 0x0C, NULL,											P_UNKNOWN,			0 },

	{ 0xFF, NULL,											P_UNKNOWN,			-1 },
};

#endif // _OBDII_PID_TABLE__


