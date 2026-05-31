# DAQ Firmware Dashboard Agent Spec

This document is a handoff for an AI agent building a GUI dashboard for the Terps Racing EV DAQ firmware. Use this markdown together with `DAQ-Firmware.dbc`. The DBC is the source of truth for telemetry message names, signal layouts, scaling, units, and value tables. This document explains the intended dashboard behavior, control messages, page structure, and what data needs to be visible.

## Product Goal

Build an operational dashboard for monitoring and controlling all DAQ boards on the car:

- DBF: front DAQ board, board ID `0x0A`.
- DBL: back-left DAQ board, board ID `0x0B`.
- DBR: back-right DAQ board, board ID `0x0C`.

The dashboard should be a quiet engineering tool, not a marketing page. It should prioritize dense live data, clear health state, fast controls, and data logging. The first screen should be the usable dashboard.

## CAN And DBC Requirements

Use `DAQ-Firmware.dbc` to decode all telemetry frames. The firmware uses extended 29-bit CAN IDs. In the DBC, extended IDs are represented by OR-ing the raw ID with `0x80000000`; the GUI transport layer should transmit and receive the raw extended IDs on the bus.

The firmware sends 8-byte CAN data frames. All current DBC messages are 8 bytes. Use little-endian signal packing as described by the DBC.

The dashboard must support:

- Loading the DBC from file.
- Receiving live CAN frames from the selected CAN interface.
- Decoding every signal in the DBC.
- Transmitting control frames that are not all represented as telemetry signals.
- Showing stale/offline state when expected board heartbeat or sensor frames stop arriving.
- Logging raw frames and decoded signal values with timestamps.

## Board Identity And Command ID Pattern

Board IDs are encoded in bits 20:23 of command IDs. The firmware checks whether a received command targets its current board ID before processing it.

Raw command bases:

| Command | Base ID | Targeted raw ID formula | Payload |
| --- | --- | --- | --- |
| Set board ID | `0x0D0000C1` | `0x0D0000C1 | (targetBoardId << 20)` | `data[0] = newBoardId` |
| Reset board | `0x0D0000C2` | `0x0D0000C2 | (targetBoardId << 20)` | payload ignored |
| DBF acceleration timer command | `0x0D0000C3` | `0x0D0000C3 | (0x0A << 20)` = `0x0DA000C3` | `data[0] = command` |

Board-specific command IDs:

| Board | Board ID | Set ID command | Reset command |
| --- | --- | --- | --- |
| Unconfigured/default | `0x00` | `0x0D0000C1` | `0x0D0000C2` |
| DBF | `0x0A` | `0x0DA000C1` | `0x0DA000C2` |
| DBL | `0x0B` | `0x0DB000C1` | `0x0DB000C2` |
| DBR | `0x0C` | `0x0DC000C1` | `0x0DC000C2` |

For setting board ID, the GUI should allow selecting the target board's current ID and the desired new ID. Valid configured board IDs are `0x0A`, `0x0B`, and `0x0C`. Also allow targeting `0x00` for a board that is still using the firmware default ID.

After a successful set-board-ID command, firmware writes flash and resets the microcontroller. The dashboard should expect the board to disappear briefly, then reappear under the new board ID.

## Required Dashboard Areas

Build these primary areas:

1. Connection And Logging
2. Board Overview
3. Front Board Detail
4. Left Board Detail
5. Right Board Detail
6. Acceleration Timer Control
7. Configuration And Commands
8. Raw CAN / Diagnostics

A good layout is a left navigation rail or top tab bar plus a persistent connection/status header. The dashboard should remain useful on a laptop screen during testing in a garage or at the track.

## Connection And Logging

The dashboard should provide:

- CAN interface selection.
- Connect/disconnect control.
- Bus status: connected, disconnected, error, last frame time.
- RX frame rate and TX frame rate.
- Raw frame count.
- Decoded signal count.
- Start/stop logging.
- Log output format options: CSV for decoded values and a raw CAN log format if supported by the chosen stack.
- Clear session data.

All outgoing command frames should be shown in a command history with timestamp, raw ID, payload, and decoded meaning.

## Board Overview

The overview should show one compact panel per board:

- Board name: DBF, DBL, DBR.
- Board ID: `0x0A`, `0x0B`, `0x0C`.
- Online/offline state based on heartbeat/status messages.
- Last heartbeat timestamp from the board status frame.
- Age of last received frame.
- Sensor health summary: valid, timeout, ADC error, out of range.
- Most important live values for that board.
- Reset button for each board with confirmation.

Heartbeat/status messages:

| Board | Raw ID | DBC message | Signal |
| --- | --- | --- | --- |
| DBF | `0x0DA00000` | `DBF_Status` | `DBF_Status_Timestamp` in ms |
| DBL | `0x0DB00000` | `DBL_Status` | `DBL_Status_Timestamp` in ms |
| DBR | `0x0DC00000` | `DBR_Status` | `DBR_Status_Timestamp` in ms |

The firmware sends heartbeat/status every 1000 ms. Consider a board stale if no heartbeat is seen for more than 2500 ms, and offline if no heartbeat is seen for more than 5000 ms. Make these thresholds configurable.

## Front Board Detail: DBF

Show all front board telemetry and health flags.

### Wheel Speed FL

DBC message: `DBF_WSPD_FL`, raw ID `0x0DA10000`.

Display:

- `DBF_WSPD_FL_RPM` in RPM.
- `DBF_WSPD_FL_MPH` in MPH.
- `DBF_WSPD_FL_Avg_Delta` in us.
- `DBF_WSPD_FL_Valid` as health state.
- `DBF_WSPD_FL_Timeout` as fault state.

### Wheel Speed FR

DBC message: `DBF_WSPD_FR`, raw ID `0x0DA11000`.

Display:

- `DBF_WSPD_FR_RPM` in RPM.
- `DBF_WSPD_FR_MPH` in MPH.
- `DBF_WSPD_FR_Avg_Delta` in us.
- `DBF_WSPD_FR_Valid` as health state.
- `DBF_WSPD_FR_Timeout` as fault state.

### Shocks FL And FR

DBC messages: `DBF_Shock_FL` and `DBF_Shock_FR`.

Display each side:

- Valid flag.
- ADC error flag.
- Raw voltage from `*_Raw_mV`; DBC scales this to V.
- Filtered voltage from `*_Filt_mV`; DBC scales this to V.
- Shock position from `*_mm` in mm.
- A small trend graph for shock position is useful.

### Steering Angle

DBC message: `DBF_Steering_Angle`, raw ID `0x0DA30000`.

Display:

- `DBF_Steering_Angle_Deg` in degrees.
- Raw and filtered voltage.
- Valid and ADC error flags.
- A gauge or centered bar is appropriate.

### Pitot Tube

DBC message: `DBF_Pitot`, raw ID `0x0DA40000`.

Display:

- `DBF_Pitot_MPH` in MPH.
- Raw and filtered voltage.
- Valid and ADC error flags.

### Odometer

DBC message: `DBF_ODO_LR`, raw ID `0x0DA90000`.

Display:

- `FL_Odo` tick count.
- `FR_Odo` tick count. Note: the current DBC signal name is `FR_Odo`; if any older generated DBC labels it differently, follow the DBC file given to the agent.
- Difference between front wheel tick counts.
- Optional reset in UI only for displayed session values; firmware does not currently expose a CAN command to reset odometer ticks without resetting the board.

## Acceleration Timer

The acceleration timer lives on the front board only. It times a 246 ft acceleration event using the front wheel-speed pulses.

Firmware constants and behavior:

- Front wheel diameter: 17.66 in.
- Wheel-speed spokes/pulses per revolution: 16.
- Event distance: 246 ft.
- Timer starts automatically on the first FL or FR wheel-speed pulse after it is enabled.
- Timer finishes when the computed 246 ft target is reached.
- Results are published as three individual elapsed times: FL-only, FR-only, and averaged front wheels.
- Live distance is published for FL, FR, and averaged front wheels in feet.

### Control Frame

DBC message: `DBF_Accel_Timer_Cmd`, raw extended ID `0x0DA000C3`.

Payload:

| Byte | Meaning |
| --- | --- |
| `data[0]` | command enum |
| `data[1]..data[7]` | send as zero |

Commands:

| Value | Meaning | UI command |
| --- | --- | --- |
| `0` | Disable timer | Disable |
| `1` | Enable timer, clear previous results, arm for next launch | Arm / Enable |
| `2` | Reset run data while preserving current enabled state | Reset Results |

The UI should make the armed state obvious. A recommended control panel:

- Large Arm button that sends command `1`.
- Disable button that sends command `0`.
- Reset Results button that sends command `2`.
- A confirmation or press-and-hold behavior for Disable if the timer is running.
- Show last command sent and whether the expected status bits changed afterward.

### Result Frame

DBC message: `DBF_Accel_Timer`, raw ID `0x0DA80000`.

Display these status bits:

- `DBF_Accel_Timer_Enabled`.
- `DBF_Accel_Timer_Running`.
- `DBF_Accel_Timer_Complete`.
- `DBF_Accel_Timer_FL_Complete`.
- `DBF_Accel_Timer_FR_Complete`.
- `DBF_Accel_Timer_Avg_Complete`.

Display these times:

- `DBF_Accel_Timer_FL_Time` in ms.
- `DBF_Accel_Timer_FR_Time` in ms.
- `DBF_Accel_Timer_Avg_Time` in ms.

### Distance Frame

DBC message: `DBF_Accel_Timer_Distance`, raw ID `0x0DA81000`.

Display these status bits:

- `DBF_Accel_Timer_Dist_Enabled`.
- `DBF_Accel_Timer_Dist_Running`.
- `DBF_Accel_Timer_Dist_Complete`.

Display these live distances:

- `DBF_Accel_Timer_FL_Distance` in ft.
- `DBF_Accel_Timer_FR_Distance` in ft.
- `DBF_Accel_Timer_Avg_Distance` in ft.

UI behavior:

- Before enable: show Disabled.
- After enable and before motion: show Armed / Waiting for launch.
- After first wheel pulse: show Running.
- During the run: show real-time FL, FR, and average distance toward 246 ft.
- Once each result completes: lock in and highlight that result.
- Once all completion bits are true: show Complete.
- Show times in seconds with millisecond precision, while preserving raw ms in tooltips or detail views. Example: `4.823 s` from `4823 ms`.
- Show a comparison between FL, FR, and average. If FL and FR differ significantly, show a warning that wheel slip or sensor mismatch may have affected the result.

## Left Board Detail: DBL

Show all back-left board telemetry and health flags.

### Wheel Speed BL

DBC message: `DBL_WSPD_BL`, raw ID `0x0DB10000`.

Display:

- `DBL_WSPD_BL_RPM`.
- `DBL_WSPD_BL_MPH`.
- `DBL_WSPD_BL_Avg_Delta` in us.
- Valid flag.
- Timeout flag.

### Shock BL

DBC message: `DBL_Shock_BL`, raw ID `0x0DB20000`.

Display:

- Valid flag.
- ADC error flag.
- Raw voltage.
- Filtered voltage.
- Position in mm.

### Coolant Temperatures

DBC messages:

| DBC message | Raw ID | Main temperature signal |
| --- | --- | --- |
| `DBL_CT_Swirl` | `0x0DB50000` | `DBL_CT_Swirl_Deg_C` |
| `DBL_CT_Rad1` | `0x0DB51000` | `DBL_CT_Rad1_Deg_C` |
| `DBL_CT_Rad2` | `0x0DB52000` | `DBL_CT_Rad2_Deg_C` |

Display each temperature channel:

- Temperature in C.
- Valid flag.
- ADC error flag.
- Out-of-range flag.
- Raw and filtered voltage.
- Trend graph and threshold highlighting.

### Tach L

DBC message: `DBL_Tach_L`, raw ID `0x0DB60000`.

Display:

- `DBL_Tach_L_RPM`.
- `DBL_Tach_L_Avg_Delta` in us.
- Valid flag.
- Timeout flag.

## Right Board Detail: DBR

Show all back-right board telemetry and health flags.

### Wheel Speed BR

DBC message: `DBR_WSPD_BR`, raw ID `0x0DC10000`.

Display:

- `DBR_WSPD_BR_RPM`.
- `DBR_WSPD_BR_MPH`.
- `DBR_WSPD_BR_Avg_Delta` in us.
- Valid flag.
- Timeout flag.

### Shock BR

DBC message: `DBR_Shock_BR`, raw ID `0x0DC20000`.

Display:

- Valid flag.
- ADC error flag.
- Raw voltage.
- Filtered voltage.
- Position in mm.

### Motor And Inverter Temperatures

DBC messages:

| DBC message | Raw ID | Main temperature signal |
| --- | --- | --- |
| `DBR_CT_Motor` | `0x0DC50000` | `DBR_CT_Motor_Deg_C` |
| `DBR_CT_Inv` | `0x0DC51000` | `DBR_CT_Inv_Deg_C` |

Important naming note: the current DBC has some validity signals for these DBR temperature messages named with `DBL_` prefixes: `DBL_CT_Motor_Valid`, `DBL_CT_Motor_ADC_Err`, `DBL_CT_Motor_Out_of_Range`, `DBL_CT_Inv_Valid`, `DBL_CT_Inv_ADC_Err`, and `DBL_CT_Inv_Out_of_Range`. Use the exact signal names from the DBC, but label them in the UI as DBR motor and DBR inverter health.

Display each channel:

- Temperature in C.
- Valid flag.
- ADC error flag.
- Out-of-range flag.
- Raw and filtered voltage.
- Trend graph and threshold highlighting.

### Tach R

DBC message: `DBR_Tach_R`, raw ID `0x0DC60000`.

Display:

- `DBR_Tach_R_RPM`.
- `DBR_Tach_R_Avg_Delta` in us.
- Valid flag.
- Timeout flag.

## Configuration And Commands

Create a configuration page with protected controls. These controls can change board behavior or reboot boards, so the UI should make accidental activation unlikely.

Required controls:

- Set board ID.
- Reset DBF.
- Reset DBL.
- Reset DBR.
- Send acceleration timer command.
- Optional custom raw command sender for advanced debugging.

### Set Board ID Control

Fields:

- Target current board ID: `0x00`, `0x0A`, `0x0B`, `0x0C`.
- New board ID: `0x0A`, `0x0B`, `0x0C`.
- Send button.
- Confirmation text summarizing the raw command ID and payload.

Frame construction:

- Raw extended ID: `0x0D0000C1 | (targetCurrentBoardId << 20)`.
- DLC: 8.
- `data[0] = newBoardId`.
- `data[1..7] = 0`.

After sending, expect the target board to reset and resume heartbeat with the new board ID.

### Reset Board Control

Frame construction:

- Raw extended ID: `0x0D0000C2 | (targetBoardId << 20)`.
- DLC: 8.
- `data[0..7] = 0`.

After sending, expect heartbeat to stop briefly and resume.

## Data Presentation Requirements

For every decoded sensor frame, show:

- Current decoded value.
- Unit.
- Last update age.
- Health flags.
- Min/max observed in current session.
- Optional sparkline or trend.

For boolean health fields:

- Valid true should look healthy.
- Timeout true should look faulted.
- ADC error true should look faulted.
- Out-of-range true should look warning or faulted.

For missing frames:

- If a frame has never been seen, show `No data`.
- If a frame was seen but is stale, show `Stale` with age.
- Do not continue presenting stale values as if they are live.

## Suggested Overview Metrics

Useful top-level metrics:

- All board online count: `0/3` through `3/3`.
- Total active faults.
- Front wheel speeds FL/FR in RPM and MPH.
- Rear wheel speeds BL/BR in RPM and MPH.
- Steering angle.
- Pitot speed.
- Highest coolant temperature.
- Tach L/R.
- Acceleration timer state, live distance, and best/average result.

## Raw CAN / Diagnostics Page

Include a diagnostics page for development:

- Raw frame table: timestamp, ID, DLC, bytes, decoded message name if known.
- Filters by board, ID, DBC message, signal name, and fault state.
- Pause/resume table updates.
- Copy frame as hex.
- Manual transmit form with raw extended ID and 8 payload bytes.
- Decode inspector for the selected frame.

## UX Notes

The intended users are race-team engineers and firmware developers. The dashboard should be fast to scan during bench testing and track operation.

Recommended design approach:

- Use compact panels and tables.
- Use status chips or icons for online, stale, valid, timeout, ADC error, and out-of-range.
- Use tabs or navigation for board detail pages.
- Keep controls near the data they affect: acceleration timer controls should live with acceleration timer status, and board reset/config controls should live on the configuration page.
- Use clear confirmation for destructive or disruptive actions such as reset and board ID changes.
- Make units visible everywhere.
- Preserve raw values in a details view even when showing formatted engineering values.

## Implementation Notes For The Agent

- Do not hard-code telemetry signal bit positions if a DBC parser is available. Load and decode the DBC.
- It is acceptable to hard-code the command frames documented here because not all configuration commands are represented in the DBC.
- Treat the DBC signal names as canonical even if some names are imperfect.
- Use raw extended CAN IDs on the bus. Do not transmit the DBC decimal IDs with the `0x80000000` extended-frame flag included.
- All transmit frames should be 8 bytes.
- Log outgoing commands and incoming decoded data.
- Build the actual dashboard as the first screen, not a landing page.
