# SEN66 component

POC based on the existing SEN5x component. **Only works with SEN66**; will be removed when the official component is released.

## SEN66 vs SEN5x (per Sensirion embedded-i2c-sen66)

- **Read measurement (0x0300)**: Returns **cumulative** mass concentrations (same as SEN5x): PM ≤1µm, ≤2.5µm, ≤4µm, ≤10µm [µg/m³] = value/10. Values are monotonic (PM1 ≤ PM2.5 ≤ PM4 ≤ PM10). We report them as-is; no incremental bins.
- **Get Product Name** (0xD014): Often returns 0 bytes on SEN66. Treated as optional; if read fails or is empty, device is assumed SEN66.
- **RHT/Temp acceleration**: SEN5x used 0x60F7 (3 modes). SEN66 uses 0x6100 with fine params (K, P, T1, T2); not implemented here.
- **Auto clean**: Removed on SEN66; use `sen6x.start_fan_autoclean` action to trigger fan cleaning from the outside.
- **Invalid values**: Humidity/temperature/VOC/NOx use int16 and signal invalid with **0x7FFF** (not 0xFFFF). PM and CO2 use 0xFFFF for invalid.

If PM or other values look wrong: ensure the sensor has run for at least ~1 min after start, and that no other I2C device conflicts on the bus. ESPHome sensirion_common uses CRC per word; 9 words are read (27 bytes on wire).