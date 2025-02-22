# Interpreting the Battery Value
## The battery level is encoded as a number from 0 to 8, where:

0x00 → Empty \
0x01 → Very Low \
0x02 → Low \
0x03 → Medium-Low \
0x04 → Medium \
0x05 → Medium-High \
0x06 → High \
0x07 → Full \
0x08 → Charging / Fully Charged

For a percentage-based display, you can map it as:
``` cpp
int percentage = (battery * 100) / 8;
```