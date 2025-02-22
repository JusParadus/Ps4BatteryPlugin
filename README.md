# Retrieving Battery Level of a PS4 Controller Using `hidapi`

This guide explains how to retrieve the battery level of a PS4 controller using the `hidapi` library. The process differs slightly between **USB** and **Bluetooth** modes. Below, you'll find details on how to send feature reports, interpret responses, and handle both modes effectively.

---

## Table of Contents
1. [USB Mode](#usb-mode)
   - [Request Battery Level](#request-battery-level-usb)
   - [Response Format](#response-format-usb)
2. [Bluetooth Mode](#bluetooth-mode)
   - [Request Battery Level](#request-battery-level-bluetooth)
   - [Response Format](#response-format-bluetooth)
3. [Battery Level Parsing Logic](#battery-level-parsing-logic)
   - [Battery Level Bits](#battery-level-bits)
   - [Charging Status](#charging-status)
4. [Debugging Tips](#debugging-tips)
   - [Log the Raw Buffer](#log-the-raw-buffer)
   - [Check for Errors](#check-for-errors)
5. [Source of Information](#source-of-information)
6. [Why Your Code Works When Plugged In](#why-your-code-works-when-plugged-in)
   - [USB Mode Communication](#usb-mode-communication)
   - [Bluetooth Mode Differences](#bluetooth-mode-differences)
7. [Why It Might Not Work in Bluetooth Mode](#why-it-might-not-work-in-bluetooth-mode)
   - [Input Report Format](#input-report-format)
   - [Feature Reports](#feature-reports)
   - [Automatic Reporting](#automatic-reporting)

---

## USB Mode

### Request Battery Level
To request the battery level in USB mode, send a feature report to the controller with the following bytes:

```plaintext
0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
```

- `0x05` is the report ID.
- The remaining bytes are padding.

After sending the request, read the response (a feature report of 78 bytes). The battery level is contained in the response.

### Response Format
The response will look something like this:

```plaintext
0x05, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
```

- The battery level is in the **second byte** (`0x01` in this example).
- The value ranges from `0x00` to `0x0B` (0 to 11), where:
  - `0x00` = 0% (critical)
  - `0x0B` = 100% (fully charged)

---

## Bluetooth Mode

### Request Battery Level
To request the battery level in Bluetooth mode, send a feature report to the controller with the following bytes:

```plaintext
0x05, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
```

- `0x05` is the report ID.
- `0xFF` is the command to request battery status.

After sending the request, read the response (a feature report of 78 bytes). The battery level is contained in the response.

### Response Format
The response will look something like this:

```plaintext
0x05, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
```

- The battery level is in the **second byte** (`0x01` in this example).
- The value ranges from `0x00` to `0x0B` (0 to 11), where:
  - `0x00` = 0% (critical)
  - `0x0B` = 100% (fully charged)

---

## Battery Level Parsing Logic

### Battery Level Bits
- The lower 4 bits (`0x0F`) of byte 12 represent the battery level.
- The values typically range from `0x00` to `0x0B` (0 to 11), where:
  - `0x00` = 0% (critical)
  - `0x0B` = 100% (fully charged)

### Charging Status
- The upper 4 bits (`0xF0`) of byte 12 indicate the charging status.
- If the controller is charging, the value will be `0x10` (or `0x20` in some cases).

---

## Debugging Tips

### Log the Raw Buffer
Add a debug log to print the entire buffer to verify the exact format of the input report:

```cpp
std::string bufferStr;
for (int i = 0; i < res; i++) {
    bufferStr += std::to_string((int)buffer[i]) + " ";
}
cvarManager->log("Raw Buffer: " + bufferStr);
```

### Check for Errors
If `hid_read` fails, log the error using `hid_error`:

```cpp
const wchar_t* error = hid_error(controllerHandle);
if (error) {
    cvarManager->log(L"hid_read failed: " + std::wstring(error));
}
```

---

## Source of Information
The information about the PS4 controller's battery level reporting comes from a combination of:

1. **Reverse Engineering**:
   - Many developers and enthusiasts have reverse-engineered the PS4 controller's communication protocol over USB and Bluetooth.

2. **Community Documentation**:
   - Open-source projects like [DS4Windows](https://github.com/Ryochan7/DS4Windows) and Linux kernel drivers provide insights into the controller's behavior.

3. **Personal Experience**:
   - Hands-on experience with HID devices, including PS4 controllers.

---

## Why Your Code Works When Plugged In

### USB Mode Communication
- In USB mode, the PS4 controller sends input reports automatically at regular intervals.
- Your code reads these reports using `hid_read`, which works seamlessly in USB mode.

### Bluetooth Mode Differences
- In Bluetooth mode, the controller behaves differently. It doesn't send input reports as frequently or in the same format as in USB mode.
- Your current code doesn't handle Bluetooth mode explicitly, which is why it might not work when the controller is connected via Bluetooth.

---

## Why It Might Not Work in Bluetooth Mode

### Input Report Format
- The input report format in Bluetooth mode is slightly different from USB mode.

### Feature Reports
- In Bluetooth mode, you often need to send a feature report to request specific data (e.g., battery level).

### Automatic Reporting
- In Bluetooth mode, the controller doesn't send input reports as frequently as in USB mode.