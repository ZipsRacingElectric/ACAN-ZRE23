
# ACAN23
The ACAN is used to read analog and PWM signals and push them onto the CAN bus.

- [ACAN23](#acan23)
  - [ACAN Hardware Capabilities](#acan-hardware-capabilities)
    - [2x APPS Inputs](#2x-apps-inputs)
    - [2x Brake Pressure Inputs](#2x-brake-pressure-inputs)
- [Functional Description](#functional-description)
    - [Timer](#timer)
    - [ADC](#adc)
    - [CANbus](#canbus)
  - [Interrupt Service Routine](#interrupt-service-routine)
- [Firmware Configuration](#firmware-configuration)
  - [Scanning Mode](#scanning-mode)
  - [CAN Message](#can-message)


## ACAN Hardware Capabilities

![ACAN Schematic](./docs/ACAN-2023%20Schematic.png)

### 2x APPS Inputs
  - 5V Power / GND
  - 5V compatible analog input
  - 16 Hz Low Pass Filter on sensor input (-3 dB)
  - 1 MOhm pull-down on sensor input
### 2x Brake Pressure Inputs
  - 5V Power / GND
  - 5V compatible analog input
  - 159 Hz Low Pass Filter on sensor input (-3 dB)
  - 1 MOhm pull-down on sensor input

The ACAN PCB can be used for 4 generic analog inputs provided they are compatible with the hardware specifications listed above.

# Functional Description

The `main()` function initializes the CANbus, ADC, and timer peripherals. The current version of the firmware uses the MCC generated HAL libraries.

If all perihperals have been successfully initialized, a heartbeat LED is blinked at a 100ms rate in the main loop. All ADC and CANbus messaging gets handled in the hardware interrupt service routine.

### Timer

The timer is a non-blocking interrupt-based Timer1 peripheral witht the following hardware cofiguration:

### ADC

The ADC peripheral is initalized with the following configuration:

### CANbus

The CANbus peripheral is initalized with the following hardware configuration:

## Interrupt Service Routine

The ISR is triggerd by the hardware Timer1. The rate of the Timer1 triggering can be configured using MCC.

The ISR starts by:
1. Toggling the ADC LED
   
2. Reading FREQUENCY-selected inputs for PWM Frequency with the `scan_sensor_frequency(ADC1_CHANNEL channel, uint32_t* period, uint16_t* frequency, bool* previous_state)` function
   
3. Reading ANALOG-selected inputs for voltage value with the `scan_sensor_analog(ADC1_CHANNEL channel, uint16_t* reading)` function
   
4. Sending a CAN message. The CANbus LED is toggled.

# Firmware Configuration

The ACAN firmware has a number of options that must be configured for each device. No other pins are routed to the main connector.

## Scanning Mode

The scanning mode determines the how signal is being read on from the input channels. This setting can be changed in the global_constants.h header file. Each of the four sensor inputs can be changed individually.

- **ANALOG** - Values are read directly by the ADC. (0V => 0ADC, 5V => 1024ADC)
- **FREQUENCY** - Values are read as a PWM signal. The output signal is given in Hertz.
  
## CAN Message

The CAN message gives the output of each channel. 4 16-bit unsigned integers are used in packed in the message.
- Byte 0 - Channel 1 Reading LO Byte
- Byte 1 - Channel 1 Reading HI Byte
- Byte 2 - Channel 2 Reading LO Byte
- Byte 3 - Channel 2 Reading HI Byte
- Byte 4 - Channel 3 Reading LO Byte
- Byte 5 - Channel 3 Reading HI Byte
- Byte 6 - Channel 4 Reading LO Byte
- Byte 7 - Channel 4 Reading HI Byte

The ID of the CAN message indicates the contents and interpretation of the message. The ID is configured in the global_constants.h header.
