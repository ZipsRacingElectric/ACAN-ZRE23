#ifndef GLOBAL_CONSTANTS_H
#define	GLOBAL_CONSTANTS_H

#ifdef	__cplusplus
extern "C" {
#endif // __cplusplus

#define FCY 40000000UL                   // Instruction Cycle Frequency - Required for <libpic30.h>

// Scanning Mode definitions
#define FREQUENCY     0                  // Scans for PWM Frequency from Channels
#define ANALOG        1                  // Reads Analog Values from Channels
    
#define SENSOR1_MODE ANALOG           // Sensor-1 Scanning Mode (Select from options above)
#define SENSOR2_MODE ANALOG           // Sensor-2 Scanning Mode
#define SENSOR3_MODE ANALOG           // Sensor-3 Scanning Mode
#define SENSOR4_MODE ANALOG           // Sensor-4 Scanning Mode

#define CAN_ID 0x755                     // Sensor Result CAN Message ID
 
#define ADC_INTERRUPT_FREQUENCY 25000    // Frequency of ADC Scanning
#define CAN_INTERRUPT_MODULUS   100      // Ratio of Interrupt Service and CAN Service

#ifdef	__cplusplus
}
#endif // __cplusplus

#endif // GLOBAL_CONSTANTS_H
