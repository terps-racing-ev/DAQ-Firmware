# Dashboard Update: Wheel-Speed MPH And Acceleration Timer Distance

Use this document alongside `DAQ-Dashboard-Agent-Spec.md` and the updated `DAQ-Firmware.dbc`. It highlights the dashboard changes needed for the latest firmware/DBC update.

## Summary

The firmware now exposes two new dashboard-facing data groups:

- Each wheel-speed telemetry frame includes an MPH signal based on the 17.66 in wheel diameter constant.
- The front-board acceleration timer publishes a new live distance frame for FL, FR, and averaged front-wheel distance in feet.

The dashboard must load the updated DBC and display these new signals wherever wheel speed and acceleration timer data are shown.

## Updated Wheel-Speed Frames

The existing wheel-speed frames keep their current health flags, average delta, and RPM signals. Byte 7 now carries rounded MPH as an unsigned 8-bit value.

| Board | DBC message | Raw ID | New signal | Unit | DBC scale |
| --- | --- | --- | --- | --- | --- |
| DBF | `DBF_WSPD_FL` | `0x0DA10000` | `DBF_WSPD_FL_MPH` | MPH | `1.0` |
| DBF | `DBF_WSPD_FR` | `0x0DA11000` | `DBF_WSPD_FR_MPH` | MPH | `1.0` |
| DBL | `DBL_WSPD_BL` | `0x0DB10000` | `DBL_WSPD_BL_MPH` | MPH | `1.0` |
| DBR | `DBR_WSPD_BR` | `0x0DC10000` | `DBR_WSPD_BR_MPH` | MPH | `1.0` |

Dashboard changes:

- Add MPH next to RPM in every wheel-speed card.
- Prefer showing MPH more prominently for vehicle speed readability, while retaining RPM for sensor/debug detail.
- Add MPH to overview tiles for front and rear wheel speeds.
- Add MPH to logs and export schemas.
- Do not derive MPH in the dashboard if the DBC signal is present; use the decoded DBC value.
- If supporting older DBCs, fall back gracefully when the `*_MPH` signals are absent.

## New Acceleration Timer Distance Frame

New DBC message: `DBF_Accel_Timer_Distance`.

Raw extended CAN ID: `0x0DA81000`.

This frame is sent by DBF along with the existing acceleration timer result/status frame.

Signals:

| Signal | Meaning | Unit | DBC scale |
| --- | --- | --- | --- |
| `DBF_Accel_Timer_Dist_Enabled` | Timer enabled/armed state mirror | boolean | `1.0` |
| `DBF_Accel_Timer_Dist_Running` | Timer running state mirror | boolean | `1.0` |
| `DBF_Accel_Timer_Dist_Complete` | Timer complete state mirror | boolean | `1.0` |
| `DBF_Accel_Timer_FL_Distance` | FL-only timer distance | ft | `0.01` |
| `DBF_Accel_Timer_FR_Distance` | FR-only timer distance | ft | `0.01` |
| `DBF_Accel_Timer_Avg_Distance` | Averaged front-wheel distance | ft | `0.01` |

Dashboard changes:

- Add a live distance section to the acceleration timer panel.
- Show FL, FR, and average distance in feet with one or two decimal places.
- Show progress toward the 246 ft target for all three distance values.
- The average distance is the primary progress value for the event; FL and FR distance are diagnostic values.
- Keep displaying the existing elapsed time result frame `DBF_Accel_Timer` for final times.
- During Armed / Waiting for launch, distances should be zero.
- During Running, distances should update in real time.
- After Complete, distances should hold final values until reset or re-arm.

Recommended acceleration timer UI update:

- State row: Disabled, Armed, Running, or Complete.
- Main progress: average distance toward 246 ft.
- Secondary progress: FL and FR distance bars.
- Final result row: FL time, FR time, average time.
- Slip/mismatch hint: warn when FL and FR distance diverge noticeably during a run.

## Existing Control Commands Are Unchanged

The acceleration timer command frame is unchanged.

DBC message: `DBF_Accel_Timer_Cmd`.

Raw extended CAN ID: `0x0DA000C3`.

Payload:

| Command | `data[0]` |
| --- | --- |
| Disable | `0` |
| Arm / Enable | `1` |
| Reset Results | `2` |

The dashboard does not need new transmit commands for distance. Distance is telemetry-only.

## Logging Changes

Add these fields to decoded data logs:

- `DBF_WSPD_FL_MPH`
- `DBF_WSPD_FR_MPH`
- `DBL_WSPD_BL_MPH`
- `DBR_WSPD_BR_MPH`
- `DBF_Accel_Timer_Dist_Enabled`
- `DBF_Accel_Timer_Dist_Running`
- `DBF_Accel_Timer_Dist_Complete`
- `DBF_Accel_Timer_FL_Distance`
- `DBF_Accel_Timer_FR_Distance`
- `DBF_Accel_Timer_Avg_Distance`

For acceleration timer run summaries, include:

- Start timestamp if the dashboard can infer it from state transition.
- Final FL time.
- Final FR time.
- Final average time.
- Final FL distance.
- Final FR distance.
- Final average distance.

## Backward Compatibility

The dashboard should tolerate both old and new DBCs:

- If the MPH signals are missing, hide MPH or mark it unavailable.
- If `DBF_Accel_Timer_Distance` is missing, keep the old acceleration timer UI with only status and final times.
- If the distance frame is present in the DBC but has not arrived on CAN, show `No distance data` or `Stale` rather than inventing values.

## Acceptance Checklist

The dashboard update is complete when:

- All four wheel-speed panels show RPM and MPH.
- Overview wheel-speed tiles include MPH.
- The acceleration timer panel shows live FL, FR, and average distance in feet.
- The acceleration timer panel still shows final FL, FR, and average elapsed times.
- Logs include the new MPH and distance signals.
- Raw CAN diagnostics can identify and decode raw ID `0x0DA81000` as `DBF_Accel_Timer_Distance`.
- The existing acceleration timer controls still transmit only `0x0DA000C3` with command values `0`, `1`, and `2`.
