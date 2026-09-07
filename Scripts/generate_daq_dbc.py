#!/usr/bin/env python3
"""Generate the DAQ Firmware DBC file.

The message names, signal layouts, units, scaling, and value tables are copied
from the DAQ sections of master.dbc. Firmware raw CAN IDs are kept alongside the
DBC IDs so this script can verify the master definitions still match the IDs in
Inc/boards/*.h and the status ID formula in Inc/managers/can_manager.h.
"""

from __future__ import annotations

import os


EXTENDED_FRAME_FLAG = 0x80000000


def dbc_id(raw_can_id: int) -> int:
	"""Return the DBC representation for an extended CAN ID."""
	return raw_can_id | EXTENDED_FRAME_FLAG


# Raw extended CAN IDs from the DAQ firmware headers.
FIRMWARE_CAN_IDS = {
	"DBF_Status": 0x0DA00000,
	"DBF_WSPD_FL": 0x0DA10000,
	"DBF_WSPD_FR": 0x0DA11000,
	"DBF_WSPD_Debug": 0x0DA100DB,
	"DBF_Shock_FL": 0x0DA20000,
	"DBF_Shock_FR": 0x0DA21000,
	"DBF_Steering_Angle": 0x0DA30000,
	"DBF_Pitot": 0x0DA40000,
	"DBF_Accel_Timer_Cmd": 0x0DA000C3,
	"DBF_Accel_Timer": 0x0DA80000,
	"DBF_Accel_Timer_Distance": 0x0DA81000,
	"DBF_ODO_LR": 0x0DA90000,
	"DBF_Lidar": 0x0DAA0000,
	"DBL_Status": 0x0DB00000,
	"DBL_WSPD_BL": 0x0DB10000,
	"DBL_Shock_BL": 0x0DB20000,
	"DBL_CT_Swirl": 0x0DB50000,
	"DBL_CT_Rad1": 0x0DB51000,
	"DBL_CT_Rad2": 0x0DB52000,
	"DBL_Tach_L": 0x0DB60000,
	"DBR_Status": 0x0DC00000,
	"DBR_WSPD_BR": 0x0DC10000,
	"DBR_Shock_BR": 0x0DC20000,
	"DBR_CT_Motor": 0x0DC50000,
	"DBR_CT_Inv": 0x0DC51000,
	"DBR_Tach_R": 0x0DC60000,
}


MESSAGES = [
	{
		"name": "DBF_Status",
		"node": "DBF",
		"raw_id": FIRMWARE_CAN_IDS["DBF_Status"],
		"signals": [
			' SG_ DBF_Status_Timestamp : 0|32@1+ (1.0,0.0) [0.0|4294967295.0] "ms" Vector__XXX',
		],
	},
	{
		"name": "DBF_WSPD_FL",
		"node": "DBF",
		"raw_id": FIRMWARE_CAN_IDS["DBF_WSPD_FL"],
		"signals": [
			' SG_ DBF_WSPD_FL_Valid : 0|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBF_WSPD_FL_Timeout : 1|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBF_WSPD_FL_Avg_Delta : 8|32@1+ (1.0,0.0) [0.0|4294967295.0] "us" Vector__XXX',
			' SG_ DBF_WSPD_FL_RPM : 40|16@1- (1.0,0.0) [-6000.0|6000.0] "RPM" Vector__XXX',
			' SG_ DBF_WSPD_FL_MPH : 56|8@1+ (1.0,0.0) [0.0|255.0] "MPH" Vector__XXX',
		],
	},
	{
		"name": "DBF_WSPD_FR",
		"node": "DBF",
		"raw_id": FIRMWARE_CAN_IDS["DBF_WSPD_FR"],
		"signals": [
			' SG_ DBF_WSPD_FR_Valid : 0|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBF_WSPD_FR_Timeout : 1|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBF_WSPD_FR_Avg_Delta : 8|32@1+ (1.0,0.0) [0.0|4294967295.0] "us" Vector__XXX',
			' SG_ DBF_WSPD_FR_RPM : 40|16@1- (1.0,0.0) [-6000.0|6000.0] "RPM" Vector__XXX',
			' SG_ DBF_WSPD_FR_MPH : 56|8@1+ (1.0,0.0) [0.0|255.0] "MPH" Vector__XXX',
		],
	},
	{
		"name": "DBF_WSPD_Debug",
		"node": "DBF",
		"raw_id": FIRMWARE_CAN_IDS["DBF_WSPD_Debug"],
		"signals": [
			' SG_ DBF_WSPD_Debug_FL_Cycle_Ticks : 0|8@1+ (1.0,0.0) [0.0|255.0] "" Vector__XXX',
			' SG_ DBF_WSPD_Debug_FR_Cycle_Ticks : 8|8@1+ (1.0,0.0) [0.0|255.0] "" Vector__XXX',
			' SG_ DBF_WSPD_Debug_FL_Min_Delta : 16|16@1+ (1.0,0.0) [0.0|65535.0] "us" Vector__XXX',
			' SG_ DBF_WSPD_Debug_FR_Min_Delta : 32|16@1+ (1.0,0.0) [0.0|65535.0] "us" Vector__XXX',
		],
	},
	{
		"name": "DBF_Shock_FL",
		"node": "DBF",
		"raw_id": FIRMWARE_CAN_IDS["DBF_Shock_FL"],
		"signals": [
			' SG_ DBF_Shock_FL_Valid : 0|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBF_Shock_FL_ADC_Err : 1|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBF_Shock_FL_Raw_mV : 8|16@1- (0.001,0.0) [-1000.0|1000.0] "V" Vector__XXX',
			' SG_ DBF_Shock_FL_Filt_mV : 24|16@1- (0.001,0.0) [-1000.0|1000.0] "V" Vector__XXX',
			' SG_ DBF_Shock_FL_mm : 40|16@1+ (0.1,0.0) [0.0|500.0] "mm" Vector__XXX',
		],
	},
	{
		"name": "DBF_Shock_FR",
		"node": "DBF",
		"raw_id": FIRMWARE_CAN_IDS["DBF_Shock_FR"],
		"signals": [
			' SG_ DBF_Shock_FR_Valid : 0|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBF_Shock_FR_ADC_Err : 1|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBF_Shock_FR_Raw_mV : 8|16@1- (0.001,0.0) [-1000.0|1000.0] "V" Vector__XXX',
			' SG_ DBF_Shock_FR_Filt_mV : 24|16@1- (0.001,0.0) [-1000.0|1000.0] "V" Vector__XXX',
			' SG_ DBF_Shock_FR_mm : 40|16@1+ (0.1,0.0) [0.0|500.0] "mm" Vector__XXX',
		],
	},
	{
		"name": "DBF_Steering_Angle",
		"node": "DBF",
		"raw_id": FIRMWARE_CAN_IDS["DBF_Steering_Angle"],
		"signals": [
			' SG_ DBF_Steering_Angle_Valid : 0|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBF_Steering_Angle_ADC_Err : 1|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBF_Steering_Angle_Raw_mV : 8|16@1- (0.001,0.0) [-1000.0|1000.0] "V" Vector__XXX',
			' SG_ DBF_Steering_Angle_Filt_mV : 24|16@1- (0.001,0.0) [-1000.0|1000.0] "V" Vector__XXX',
			' SG_ DBF_Steering_Angle_Deg : 40|16@1- (0.1,0.0) [-360.0|360.0] "deg" Vector__XXX',
		],
	},
	{
		"name": "DBF_Pitot",
		"node": "DBF",
		"raw_id": FIRMWARE_CAN_IDS["DBF_Pitot"],
		"signals": [
			' SG_ DBF_Pitot_Valid : 0|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBF_Pitot_ADC_Err : 1|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBF_Pitot_Raw_mV : 8|16@1- (0.001,0.0) [-1000.0|1000.0] "V" Vector__XXX',
			' SG_ DBF_Pitot_Filt_mV : 24|16@1- (0.001,0.0) [-1000.0|1000.0] "V" Vector__XXX',
			' SG_ DBF_Pitot_MPH : 40|16@1+ (0.01,0.0) [0.0|1000.0] "MPH" Vector__XXX',
		],
	},
	{
		"name": "DBF_Accel_Timer_Cmd",
		"node": "Vector__XXX",
		"raw_id": FIRMWARE_CAN_IDS["DBF_Accel_Timer_Cmd"],
		"signals": [
			' SG_ DBF_Accel_Timer_Command : 0|8@1+ (1.0,0.0) [0.0|2.0] "" DBF',
		],
	},
	{
		"name": "DBF_Accel_Timer",
		"node": "DBF",
		"raw_id": FIRMWARE_CAN_IDS["DBF_Accel_Timer"],
		"signals": [
			' SG_ DBF_Accel_Timer_Enabled : 0|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBF_Accel_Timer_Running : 1|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBF_Accel_Timer_Complete : 2|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBF_Accel_Timer_FL_Complete : 3|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBF_Accel_Timer_FR_Complete : 4|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBF_Accel_Timer_Avg_Complete : 5|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBF_Accel_Timer_FL_Time : 16|16@1+ (1.0,0.0) [0.0|65535.0] "ms" Vector__XXX',
			' SG_ DBF_Accel_Timer_FR_Time : 32|16@1+ (1.0,0.0) [0.0|65535.0] "ms" Vector__XXX',
			' SG_ DBF_Accel_Timer_Avg_Time : 48|16@1+ (1.0,0.0) [0.0|65535.0] "ms" Vector__XXX',
		],
	},
	{
		"name": "DBF_Accel_Timer_Distance",
		"node": "DBF",
		"raw_id": FIRMWARE_CAN_IDS["DBF_Accel_Timer_Distance"],
		"signals": [
			' SG_ DBF_Accel_Timer_Dist_Enabled : 0|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBF_Accel_Timer_Dist_Running : 1|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBF_Accel_Timer_Dist_Complete : 2|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBF_Accel_Timer_FL_Distance : 16|16@1+ (0.01,0.0) [0.0|655.35] "ft" Vector__XXX',
			' SG_ DBF_Accel_Timer_FR_Distance : 32|16@1+ (0.01,0.0) [0.0|655.35] "ft" Vector__XXX',
			' SG_ DBF_Accel_Timer_Avg_Distance : 48|16@1+ (0.01,0.0) [0.0|655.35] "ft" Vector__XXX',
		],
	},
	{
		"name": "DBF_ODO_LR",
		"node": "DBF",
		"raw_id": FIRMWARE_CAN_IDS["DBF_ODO_LR"],
		"signals": [
			' SG_ FR_Odo : 0|32@1+ (1.0,0.0) [0.0|4294967295.0] "" Vector__XXX',
			' SG_ FL_Odo : 32|32@1+ (1.0,0.0) [0.0|4294967295.0] "" Vector__XXX',
		],
	},
	{
		"name": "DBF_Lidar",
		"node": "DBF",
		"raw_id": FIRMWARE_CAN_IDS["DBF_Lidar"],
		"signals": [
			' SG_ Lidar_Height : 0|16@1+ (1.0,0.0) [0.0|65535.0] "" Vector__XXX',
			' SG_ Lidar_Strength : 16|16@1+ (1.0,0.0) [0.0|65535.0] "" Vector__XXX',
			' SG_ Lidar_Temp : 32|16@1+ (1.0,0.0) [0.0|65535.0] "" Vector__XXX',
		],
	},
	{
		"name": "DBL_Status",
		"node": "DBL",
		"raw_id": FIRMWARE_CAN_IDS["DBL_Status"],
		"signals": [
			' SG_ DBL_Status_Timestamp : 0|32@1+ (1.0,0.0) [0.0|4294967295.0] "ms" Vector__XXX',
		],
	},
	{
		"name": "DBL_WSPD_BL",
		"node": "DBL",
		"raw_id": FIRMWARE_CAN_IDS["DBL_WSPD_BL"],
		"signals": [
			' SG_ DBL_WSPD_BL_Valid : 0|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBL_WSPD_BL_Timeout : 1|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBL_WSPD_BL_Avg_Delta : 8|32@1+ (1.0,0.0) [0.0|4294967295.0] "us" Vector__XXX',
			' SG_ DBL_WSPD_BL_RPM : 40|16@1- (1.0,0.0) [-6000.0|6000.0] "RPM" Vector__XXX',
			' SG_ DBL_WSPD_BL_MPH : 56|8@1+ (1.0,0.0) [0.0|255.0] "MPH" Vector__XXX',
		],
	},
	{
		"name": "DBL_Shock_BL",
		"node": "DBL",
		"raw_id": FIRMWARE_CAN_IDS["DBL_Shock_BL"],
		"signals": [
			' SG_ DBL_Shock_BL_Valid : 0|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBL_Shock_BL_ADC_Err : 1|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBL_Shock_BL_Raw_mV : 8|16@1- (0.001,0.0) [-1000.0|1000.0] "V" Vector__XXX',
			' SG_ DBL_Shock_BL_Filt_mV : 24|16@1- (0.001,0.0) [-1000.0|1000.0] "V" Vector__XXX',
			' SG_ DBL_Shock_BL_mm : 40|16@1+ (0.1,0.0) [0.0|500.0] "mm" Vector__XXX',
		],
	},
	{
		"name": "DBL_CT_Swirl",
		"node": "DBL",
		"raw_id": FIRMWARE_CAN_IDS["DBL_CT_Swirl"],
		"signals": [
			' SG_ DBL_CT_Swirl_Valid : 0|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBL_CT_Swirl_ADC_Err : 1|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBL_CT_Swirl_Out_of_Range : 2|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBL_CT_Swirl_Raw_mV : 8|16@1- (0.001,0.0) [-1000.0|1000.0] "V" Vector__XXX',
			' SG_ DBL_CT_Swirl_Filt_mV : 24|16@1- (0.001,0.0) [-1000.0|1000.0] "V" Vector__XXX',
			' SG_ DBL_CT_Swirl_Deg_C : 40|8@1- (1.0,0.0) [-40.0|120.0] "C" Vector__XXX',
		],
	},
	{
		"name": "DBL_CT_Rad1",
		"node": "DBL",
		"raw_id": FIRMWARE_CAN_IDS["DBL_CT_Rad1"],
		"signals": [
			' SG_ DBL_CT_Rad1_Valid : 0|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBL_CT_Rad1_ADC_Err : 1|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBL_CT_Rad1_Out_of_Range : 2|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBL_CT_Rad1_Raw_mV : 8|16@1- (0.001,0.0) [-1000.0|1000.0] "V" Vector__XXX',
			' SG_ DBL_CT_Rad1_Filt_mV : 24|16@1- (0.001,0.0) [-1000.0|1000.0] "V" Vector__XXX',
			' SG_ DBL_CT_Rad1_Deg_C : 40|8@1- (1.0,0.0) [-40.0|120.0] "C" Vector__XXX',
		],
	},
	{
		"name": "DBL_CT_Rad2",
		"node": "DBL",
		"raw_id": FIRMWARE_CAN_IDS["DBL_CT_Rad2"],
		"signals": [
			' SG_ DBL_CT_Rad2_Valid : 0|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBL_CT_Rad2_ADC_Err : 1|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBL_CT_Rad2_Out_of_Range : 2|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBL_CT_Rad2_Raw_mV : 8|16@1- (0.001,0.0) [-1000.0|1000.0] "V" Vector__XXX',
			' SG_ DBL_CT_Rad2_Filt_mV : 24|16@1- (0.001,0.0) [-1000.0|1000.0] "V" Vector__XXX',
			' SG_ DBL_CT_Rad2_Deg_C : 40|8@1- (1.0,0.0) [-40.0|120.0] "C" Vector__XXX',
		],
	},
	{
		"name": "DBL_Tach_L",
		"node": "DBL",
		"raw_id": FIRMWARE_CAN_IDS["DBL_Tach_L"],
		"signals": [
			' SG_ DBL_Tach_L_Valid : 0|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBL_Tach_L_Timeout : 1|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBL_Tach_L_Avg_Delta : 8|32@1+ (1.0,0.0) [0.0|4294967295.0] "us" Vector__XXX',
			' SG_ DBL_Tach_L_RPM : 40|16@1- (1.0,0.0) [-6000.0|6000.0] "RPM" Vector__XXX',
		],
	},
	{
		"name": "DBR_Status",
		"node": "DBR",
		"raw_id": FIRMWARE_CAN_IDS["DBR_Status"],
		"signals": [
			' SG_ DBR_Status_Timestamp : 0|32@1+ (1.0,0.0) [0.0|4294967295.0] "ms" Vector__XXX',
		],
	},
	{
		"name": "DBR_WSPD_BR",
		"node": "DBR",
		"raw_id": FIRMWARE_CAN_IDS["DBR_WSPD_BR"],
		"signals": [
			' SG_ DBR_WSPD_BR_Valid : 0|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBR_WSPD_BR_Timeout : 1|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBR_WSPD_BR_Avg_Delta : 8|32@1+ (1.0,0.0) [0.0|4294967295.0] "us" Vector__XXX',
			' SG_ DBR_WSPD_BR_RPM : 40|16@1- (1.0,0.0) [-6000.0|6000.0] "RPM" Vector__XXX',
			' SG_ DBR_WSPD_BR_MPH : 56|8@1+ (1.0,0.0) [0.0|255.0] "MPH" Vector__XXX',
		],
	},
	{
		"name": "DBR_Shock_BR",
		"node": "DBR",
		"raw_id": FIRMWARE_CAN_IDS["DBR_Shock_BR"],
		"signals": [
			' SG_ DBR_Shock_BR_Valid : 0|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBR_Shock_BR_ADC_Err : 1|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBR_Shock_BR_Raw_mV : 8|16@1- (0.001,0.0) [-1000.0|1000.0] "V" Vector__XXX',
			' SG_ DBR_Shock_BR_Filt_mV : 24|16@1- (0.001,0.0) [-1000.0|1000.0] "V" Vector__XXX',
			' SG_ DBR_Shock_BR_mm : 40|16@1+ (0.1,0.0) [0.0|500.0] "mm" Vector__XXX',
		],
	},
	{
		"name": "DBR_CT_Motor",
		"node": "DBR",
		"raw_id": FIRMWARE_CAN_IDS["DBR_CT_Motor"],
		"signals": [
			' SG_ DBL_CT_Motor_Valid : 0|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBL_CT_Motor_ADC_Err : 1|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBL_CT_Motor_Out_of_Range : 2|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBR_CT_Motor_Raw_mV : 8|16@1- (0.001,0.0) [-1000.0|1000.0] "V" Vector__XXX',
			' SG_ DBR_CT_Motor_Filt_mV : 24|16@1- (0.001,0.0) [-1000.0|1000.0] "V" Vector__XXX',
			' SG_ DBR_CT_Motor_Deg_C : 40|8@1- (1.0,0.0) [-40.0|120.0] "C" Vector__XXX',
		],
	},
	{
		"name": "DBR_CT_Inv",
		"node": "DBR",
		"raw_id": FIRMWARE_CAN_IDS["DBR_CT_Inv"],
		"signals": [
			' SG_ DBL_CT_Inv_Valid : 0|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBL_CT_Inv_ADC_Err : 1|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBL_CT_Inv_Out_of_Range : 2|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBR_CT_Inv_Raw_mV : 8|16@1- (0.001,0.0) [-1000.0|1000.0] "V" Vector__XXX',
			' SG_ DBR_CT_Inv_Filt_mV : 24|16@1- (0.001,0.0) [-1000.0|1000.0] "V" Vector__XXX',
			' SG_ DBR_CT_Inv_Deg_C : 40|8@1- (1.0,0.0) [-40.0|120.0] "C" Vector__XXX',
		],
	},
	{
		"name": "DBR_Tach_R",
		"node": "DBR",
		"raw_id": FIRMWARE_CAN_IDS["DBR_Tach_R"],
		"signals": [
			' SG_ DBR_Tach_R_Valid : 0|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBR_Tach_R_Timeout : 1|1@1+ (1.0,0.0) [0.0|1.0] "" Vector__XXX',
			' SG_ DBR_Tach_R_Avg_Delta : 8|32@1+ (1.0,0.0) [0.0|4294967295.0] "us" Vector__XXX',
			' SG_ DBR_Tach_R_RPM : 40|16@1- (1.0,0.0) [-6000.0|6000.0] "RPM" Vector__XXX',
		],
	},
]


VALUE_TABLES = [
	'VAL_ 2376138752 DBF_WSPD_FL_Valid 0 "FALSE" 1 "TRUE" ;',
	'VAL_ 2376138752 DBF_WSPD_FL_Timeout 0 "FALSE" 1 "TRUE" ;',
	'VAL_ 2376142848 DBF_WSPD_FR_Valid 0 "FALSE" 1 "TRUE" ;',
	'VAL_ 2376142848 DBF_WSPD_FR_Timeout 0 "FALSE" 1 "TRUE" ;',
	'VAL_ 2376204288 DBF_Shock_FL_Valid 0 "FALSE" 1 "TRUE" ;',
	'VAL_ 2376204288 DBF_Shock_FL_ADC_Err 0 "FALSE" 1 "TRUE" ;',
	'VAL_ 2376208384 DBF_Shock_FR_Valid 0 "FALSE" 1 "TRUE" ;',
	'VAL_ 2376208384 DBF_Shock_FR_ADC_Err 0 "FALSE" 1 "TRUE" ;',
	'VAL_ 2376269824 DBF_Steering_Angle_Valid 0 "FALSE" 1 "TRUE" ;',
	'VAL_ 2376269824 DBF_Steering_Angle_ADC_Err 0 "FALSE" 1 "TRUE" ;',
	'VAL_ 2376335360 DBF_Pitot_Valid 0 "FALSE" 1 "TRUE" ;',
	'VAL_ 2376335360 DBF_Pitot_ADC_Err 0 "FALSE" 1 "TRUE" ;',
	f'VAL_ {dbc_id(FIRMWARE_CAN_IDS["DBF_Accel_Timer_Cmd"])} DBF_Accel_Timer_Command 0 "Disable" 1 "Enable" 2 "Reset" ;',
	f'VAL_ {dbc_id(FIRMWARE_CAN_IDS["DBF_Accel_Timer"])} DBF_Accel_Timer_Enabled 0 "FALSE" 1 "TRUE" ;',
	f'VAL_ {dbc_id(FIRMWARE_CAN_IDS["DBF_Accel_Timer"])} DBF_Accel_Timer_Running 0 "FALSE" 1 "TRUE" ;',
	f'VAL_ {dbc_id(FIRMWARE_CAN_IDS["DBF_Accel_Timer"])} DBF_Accel_Timer_Complete 0 "FALSE" 1 "TRUE" ;',
	f'VAL_ {dbc_id(FIRMWARE_CAN_IDS["DBF_Accel_Timer"])} DBF_Accel_Timer_FL_Complete 0 "FALSE" 1 "TRUE" ;',
	f'VAL_ {dbc_id(FIRMWARE_CAN_IDS["DBF_Accel_Timer"])} DBF_Accel_Timer_FR_Complete 0 "FALSE" 1 "TRUE" ;',
	f'VAL_ {dbc_id(FIRMWARE_CAN_IDS["DBF_Accel_Timer"])} DBF_Accel_Timer_Avg_Complete 0 "FALSE" 1 "TRUE" ;',
	f'VAL_ {dbc_id(FIRMWARE_CAN_IDS["DBF_Accel_Timer_Distance"])} DBF_Accel_Timer_Dist_Enabled 0 "FALSE" 1 "TRUE" ;',
	f'VAL_ {dbc_id(FIRMWARE_CAN_IDS["DBF_Accel_Timer_Distance"])} DBF_Accel_Timer_Dist_Running 0 "FALSE" 1 "TRUE" ;',
	f'VAL_ {dbc_id(FIRMWARE_CAN_IDS["DBF_Accel_Timer_Distance"])} DBF_Accel_Timer_Dist_Complete 0 "FALSE" 1 "TRUE" ;',
	'VAL_ 2377187328 DBL_WSPD_BL_Valid 0 "FALSE" 1 "TRUE" ;',
	'VAL_ 2377187328 DBL_WSPD_BL_Timeout 0 "FALSE" 1 "TRUE" ;',
	'VAL_ 2377252864 DBL_Shock_BL_Valid 0 "FALSE" 1 "TRUE" ;',
	'VAL_ 2377252864 DBL_Shock_BL_ADC_Err 0 "FALSE" 1 "TRUE" ;',
	'VAL_ 2377449472 DBL_CT_Swirl_Valid 0 "FALSE" 1 "TRUE" ;',
	'VAL_ 2377449472 DBL_CT_Swirl_ADC_Err 0 "FALSE" 1 "TRUE" ;',
	'VAL_ 2377449472 DBL_CT_Swirl_Out_of_Range 0 "FALSE" 1 "TRUE" ;',
	'VAL_ 2377453568 DBL_CT_Rad1_Valid 0 "FALSE" 1 "TRUE" ;',
	'VAL_ 2377453568 DBL_CT_Rad1_ADC_Err 0 "FALSE" 1 "TRUE" ;',
	'VAL_ 2377453568 DBL_CT_Rad1_Out_of_Range 0 "FALSE" 1 "TRUE" ;',
	'VAL_ 2377457664 DBL_CT_Rad2_Valid 0 "FALSE" 1 "TRUE" ;',
	'VAL_ 2377457664 DBL_CT_Rad2_ADC_Err 0 "FALSE" 1 "TRUE" ;',
	'VAL_ 2377457664 DBL_CT_Rad2_Out_of_Range 0 "FALSE" 1 "TRUE" ;',
	'VAL_ 2377515008 DBL_Tach_L_Valid 0 "FALSE" 1 "TRUE" ;',
	'VAL_ 2377515008 DBL_Tach_L_Timeout 0 "FALSE" 1 "TRUE" ;',
	'VAL_ 2378235904 DBR_WSPD_BR_Valid 0 "FALSE" 1 "TRUE" ;',
	'VAL_ 2378235904 DBR_WSPD_BR_Timeout 0 "FALSE" 1 "TRUE" ;',
	'VAL_ 2378301440 DBR_Shock_BR_Valid 0 "FALSE" 1 "TRUE" ;',
	'VAL_ 2378301440 DBR_Shock_BR_ADC_Err 0 "FALSE" 1 "TRUE" ;',
	'VAL_ 2378498048 DBL_CT_Motor_Valid 0 "FALSE" 1 "TRUE" ;',
	'VAL_ 2378498048 DBL_CT_Motor_ADC_Err 0 "FALSE" 1 "TRUE" ;',
	'VAL_ 2378498048 DBL_CT_Motor_Out_of_Range 0 "FALSE" 1 "TRUE" ;',
	'VAL_ 2378502144 DBL_CT_Inv_Valid 0 "FALSE" 1 "TRUE" ;',
	'VAL_ 2378502144 DBL_CT_Inv_ADC_Err 0 "FALSE" 1 "TRUE" ;',
	'VAL_ 2378502144 DBL_CT_Inv_Out_of_Range 0 "FALSE" 1 "TRUE" ;',
	'VAL_ 2378563584 DBR_Tach_R_Valid 0 "FALSE" 1 "TRUE" ;',
	'VAL_ 2378563584 DBR_Tach_R_Timeout 0 "FALSE" 1 "TRUE" ;',
]


COMMENTS = [
	f'CM_ BO_ {dbc_id(FIRMWARE_CAN_IDS["DBF_Accel_Timer_Cmd"])} "Front-board acceleration timer command. Send this extended frame to 0x0DA000C3 for DBF: command 0 disables the timer, 1 clears prior results and arms the timer, and 2 clears results while preserving the current enabled state.";',
	f'CM_ BO_ {dbc_id(FIRMWARE_CAN_IDS["DBF_Accel_Timer"])} "Front-board acceleration timer result/status. After enable, the first FL or FR wheel-speed pulse starts timing. The timer uses a 17.66 in front tire diameter, 16 wheel-speed pulses per revolution, and a 246 ft target. Completion bits indicate valid FL-only, FR-only, and averaged-wheel elapsed times in milliseconds.";',
	f'CM_ BO_ {dbc_id(FIRMWARE_CAN_IDS["DBF_Accel_Timer_Distance"])} "Front-board acceleration timer live distance. Distances use the 17.66 in front tire diameter and 16 wheel-speed pulses per revolution. FL, FR, and averaged front-wheel distances are reported in feet while the timer is armed/running and hold their final values after completion.";',
]


def verify_message_ids() -> None:
	"""Ensure every master-derived message ID still matches firmware IDs."""
	seen_names = set()

	for message in MESSAGES:
		name = message["name"]
		raw_id = message["raw_id"]
		expected_raw_id = FIRMWARE_CAN_IDS[name]

		if raw_id != expected_raw_id:
			raise ValueError(
				f"{name} raw CAN ID mismatch: message has 0x{raw_id:08X}, "
				f"firmware map has 0x{expected_raw_id:08X}"
			)

		seen_names.add(name)

	missing = set(FIRMWARE_CAN_IDS) - seen_names
	if missing:
		missing_names = ", ".join(sorted(missing))
		raise ValueError(f"Missing DBC message definitions for: {missing_names}")


def generate_dbc() -> str:
	verify_message_ids()

	lines = []

	lines.append('VERSION ""')
	lines.append('')
	lines.append('NS_ :')
	lines.append('')
	lines.append('BS_:')
	lines.append('')
	lines.append('BU_: DBF DBL DBR Vector__XXX')
	lines.append('')

	for message in MESSAGES:
		lines.append(
			f'BO_ {dbc_id(message["raw_id"])} {message["name"]}: 8 {message["node"]}'
		)
		lines.extend(message["signals"])
		lines.append('')

	lines.extend(VALUE_TABLES)
	lines.extend(COMMENTS)

	return '\n'.join(lines)


if __name__ == '__main__':
	dbc_content = generate_dbc()

	script_dir = os.path.dirname(os.path.abspath(__file__))
	parent_dir = os.path.dirname(script_dir)
	output_file = os.path.join(parent_dir, 'DAQ-Firmware.dbc')

	with open(output_file, 'w') as file:
		file.write(dbc_content)

	message_count = sum(1 for line in dbc_content.splitlines() if line.startswith('BO_ '))
	signal_count = sum(1 for line in dbc_content.splitlines() if line.startswith(' SG_ '))
	value_table_count = sum(1 for line in dbc_content.splitlines() if line.startswith('VAL_ '))

	print(f"Generated {output_file}")
	print(f"Total lines: {len(dbc_content.splitlines())}")
	print(f"Total messages: {message_count}")
	print(f"Total signals: {signal_count}")
	print(f"Total value tables: {value_table_count}")
