# ACAN23
The ACAN is used to read analog and PWM signals and push them onto the CAN bus.
## Scanning Mode
The scanning mode determines the signal being read on from the input channels.
- Analog - Values are read directly by the ADC. (0V => 0ADC, 5V => 1024ADC)
- Frequency - Values are read as a PWM signal. The output signal is given in Hertz.

## CAN Message
The CAN message give the output of each channel. 4 16-bit unsigned integers are used in packed in the message.
- Byte 0 - Channel 1 Reading LO Byte
- Byte 1 - Channel 1 Reading HI Byte
- Byte 2 - Channel 2 Reading LO Byte
- Byte 3 - Channel 2 Reading HI Byte
- Byte 4 - Channel 3 Reading LO Byte
- Byte 5 - Channel 3 Reading HI Byte
- Byte 6 - Channel 4 Reading LO Byte
- Byte 7 - Channel 4 Reading HI Byte

The ID of the CAN message indicates the contents and interpretation of the message. The ID is configured in the global_constants.h header.
