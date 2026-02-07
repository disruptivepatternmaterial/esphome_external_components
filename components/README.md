# SEN66 component

POC based on the existing SEN5x component. **Only works with SEN66**; will be removed when the official component is released.

## SEN66 vs SEN5x (per Sensirion embedded-i2c-sen66)

- **Read measurement**: 0x0300 returns **direct** mass concentrations (PM1.0, PM2.5, PM4.0, PM10.0), not cumulative like SEN5x. Parsing updated accordingly.
- **Get Product Name** (0xD014): Often returns 0 bytes on SEN66. Treated as optional; if read fails or is empty, device is assumed SEN66.
- **RHT/Temp acceleration**: SEN5x used 0x60F7 (3 modes). SEN66 uses 0x6100 with fine params (K, P, T1, T2); not implemented here.
- **Auto clean**: Removed on SEN66; use `sen6x.start_fan_autoclean` action to trigger fan cleaning from the outside.
- **Invalid values**: Humidity/temperature/VOC/NOx use int16 and signal invalid with **0x7FFF** (not 0xFFFF). PM and CO2 use 0xFFFF for invalid.