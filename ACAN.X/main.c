// Includes -------------------------------------------------------------------------------------------------------------------
// Primative Libraries
#include <stdint.h>
#include <stdbool.h>

// Constants
#include "global_constants.h"

// Libraries
#include <libpic30.h>

// MCC Libraries
#include "mcc_generated_files/system.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/adc1.h"
#include "mcc_generated_files/can1.h"
#include "mcc_generated_files/tmr1.h"

// Globals --------------------------------------------------------------------------------------------------------------------
// Sensor Data
uint16_t sensor1_reading = 0;
uint32_t sensor1_period  = 0;
bool     sensor1_state   = true;

uint16_t sensor2_reading = 0;
uint32_t sensor2_period  = 0;
bool     sensor2_state   = true;

uint16_t sensor3_reading = 0;
uint32_t sensor3_period  = 0;
bool     sensor3_state   = true;

uint16_t sensor4_reading = 0;
uint32_t sensor4_period  = 0;
bool     sensor4_state   = true;

// CAN Data
uCAN_MSG can_message;

// Interrupt Service Data
uint16_t interrupt_modulus_counter = 0;

// Function Prototypes --------------------------------------------------------------------------------------------------------
// Initializers
void initialize_can();
void initialize_adc();
void initialize_timers();

// ADC Scanners
void scan_sensor_frequency(ADC1_CHANNEL channel, uint32_t* period, uint16_t* frequency, bool* previous_state);
void scan_sensor_analog(ADC1_CHANNEL channel, uint16_t* reading);

// Interrupt Service
void interrupt_service();

// Function Definitions -------------------------------------------------------------------------------------------------------
// Initializers
void initialize_can()
{
    // Instance Message Object
    can_message.frame.idType = CAN_FRAME_STD;

    // Initialize Object
    can_message.frame.id = CAN_ID;
    can_message.frame.dlc = 8;
    can_message.frame.data0 = 0; 
    can_message.frame.data1 = 0;     
    can_message.frame.data2 = 0; 
    can_message.frame.data3 = 0; 
    can_message.frame.data4 = 0; 
    can_message.frame.data5 = 0;
    can_message.frame.data6 = 0;
    can_message.frame.data7 = 0;
    
    // Enable Transceiver
    CAN1_TransmitEnable();
    CAN_STBY_SetLow();
}

void initialize_adc()
{
    ADC1_Initialize();
}

void initialize_timers()
{
    TMR1_SetInterruptHandler(&interrupt_service);
}

// Interrupt Service
void interrupt_service()
{
    ++interrupt_modulus_counter;
    
    // Scan ADC -----------------------------------------------
    ADC_LED_D6_Toggle();
    
    // PWM Frequency Scanning
    if(SCANNING_MODE == FREQUENCY)
    {
        scan_sensor_frequency(ADC1_SENSOR1, &sensor1_period, &sensor1_reading, &sensor1_state);
        scan_sensor_frequency(ADC1_SENSOR2, &sensor2_period, &sensor2_reading, &sensor2_state);
        scan_sensor_frequency(ADC1_SENSOR3, &sensor3_period, &sensor3_reading, &sensor3_state);
        scan_sensor_frequency(ADC1_SENSOR4, &sensor4_period, &sensor4_reading, &sensor4_state);
    }
    
    // Analog Value Scanning
    if(SCANNING_MODE == ANALOG)
    {
        scan_sensor_analog(ADC1_SENSOR1, &sensor1_reading);
        scan_sensor_analog(ADC1_SENSOR2, &sensor2_reading);
        scan_sensor_analog(ADC1_SENSOR3, &sensor3_reading);
        scan_sensor_analog(ADC1_SENSOR4, &sensor4_reading);
    }
    
    // Send CAN Message ---------------------------------------
    if(interrupt_modulus_counter > CAN_INTERRUPT_MODULUS)
    {
        interrupt_modulus_counter = 0;
        
        can_message.frame.data0 =  sensor1_reading       & 0xFF;
        can_message.frame.data1 = (sensor1_reading >> 8) & 0xFF;
        can_message.frame.data2 =  sensor2_reading       & 0xFF;
        can_message.frame.data3 = (sensor2_reading >> 8) & 0xFF;
        can_message.frame.data4 =  sensor3_reading       & 0xFF;
        can_message.frame.data5 = (sensor3_reading >> 8) & 0xFF;
        can_message.frame.data6 =  sensor4_reading       & 0xFF;
        can_message.frame.data7 = (sensor4_reading >> 8) & 0xFF;
        
        CAN1_transmit(CAN_PRIORITY_MEDIUM, &can_message);
        
        CAN_LED_D5_Toggle();
    }
}

// ADC Scanners
void scan_sensor_frequency(ADC1_CHANNEL channel, uint32_t* period, uint16_t* frequency, bool* previous_state)
{
    // Get Sensor Reading
    ADC1_ChannelSelectSet(channel);
    ADC1_SamplingStart();
    __delay_us(1);
    ADC1_SamplingStop();
    
    while(!ADC1_IsConversionComplete());
    bool sensor_state = (ADC1_Channel0ConversionResultGet() > 512);
    
    // Increment Period
    ++(*period);
    
    // Detect Rising Edge
    if(sensor_state && !(*previous_state))
    {
        // Calculate Frequency (ISR Frequency / Sensor Period)
        *frequency = ADC_INTERRUPT_FREQUENCY / *period;
        *period = 0;
    }
    
    // Store State
    *previous_state = sensor_state;
}

void scan_sensor_analog(ADC1_CHANNEL channel, uint16_t* reading)
{
    // Get Sensor Reading
    ADC1_ChannelSelectSet(channel);
    ADC1_SamplingStart();
    __delay_us(1);
    ADC1_SamplingStop();
    
    while(!ADC1_IsConversionComplete());
    *reading = ADC1_Channel0ConversionResultGet();
}

// Main
int main(void)
{
    // MCC Initialization
    SYSTEM_Initialize();
    initialize_can();
    initialize_adc();
    initialize_timers();
    
    // Heartbeat Loop
    while(true)
    {       
        HEARTBEAT_LED_D7_Toggle();
        __delay_ms(100);
    }
    
    // Error Code
    return 1; 
}