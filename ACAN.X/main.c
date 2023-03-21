// Primative Libraries
#include <stdint.h>
#include <stdbool.h>

// Constants
#include "global_constants.h"

// Libraries
#include <libpic30.h>  // __delayXXX() functions macros defined here

// MCC Libraries
#include "mcc_generated_files/system.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/clock.h"
#include "mcc_generated_files/system.h"
#include "mcc_generated_files/system_types.h"
#include "mcc_generated_files/interrupt_manager.h"
#include "mcc_generated_files/traps.h"
#include "mcc_generated_files/adc1.h"
#include "mcc_generated_files/can1.h"
#include "mcc_generated_files/tmr1.h"

uint32_t sensor1_cum = 0;
uint16_t sensor2_cum = 0;
uint16_t sensor3_cum = 0;
uint16_t sensor4_cum = 0;

bool sensor1_high = false;
bool sensor2_high = false;
bool sensor3_high = false;
bool sensor4_high = false;

uint16_t sensor1 = 0;
uint16_t sensor2 = 0;
uint16_t sensor3 = 0;
uint16_t sensor4 = 0;

uCAN_MSG Pedal_data;

// Data
uint16_t freq_count = 1;

void timer1_interrupt();

int main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    
    Pedal_data.frame.idType = CAN_FRAME_STD;

    Pedal_data.frame.id = PEDAL_ID;
    Pedal_data.frame.dlc = 8;
    
    Pedal_data.frame.data0 = 0; 
    Pedal_data.frame.data1 = 0;     
    Pedal_data.frame.data2 = 0; 
    Pedal_data.frame.data3 = 0; 
    Pedal_data.frame.data4 = 0; 
    Pedal_data.frame.data5 = 0;
    Pedal_data.frame.data6 = 0;
    Pedal_data.frame.data7 = 0;
    
    CAN1_TransmitEnable();
    
    CAN_STBY_SetLow(); //enable CAN transceiver 
    
    ADC1_Initialize();
    
    while (1)
    {       
        D7_Toggle();
        __delay_ms(10);
    }
    
    return 1; 
}

void timer1_interrupt()
{
    D6_SetHigh();
    // SCAN CHANNEL
    
    // SEND MESSAGE
    if(freq_count > 100)
    {   
        //parse 10-bit ADC readings to hi and lo 8-bit bytes
        Pedal_data.frame.data0 = sensor1 & 0xFF;
        Pedal_data.frame.data1 = sensor1 >> 8;
        Pedal_data.frame.data2 = sensor2 & 0xFF;
        Pedal_data.frame.data3 = sensor2 >> 8;
        Pedal_data.frame.data4 = sensor3 & 0xFF;
        Pedal_data.frame.data5 = sensor3 >> 8;
        Pedal_data.frame.data6 = sensor4 & 0xFF;
        Pedal_data.frame.data7 = sensor4 >> 8;
    
        CAN1_transmit(CAN_PRIORITY_MEDIUM, &Pedal_data);
    }
    
    D5_Toggle();
    D6_SetLow();
}

//
//void __attribute__ ( ( interrupt, no_auto_psv ) ) _T1Interrupt (  )
//{
//    /* Check if the Timer Interrupt/Status is set */
//
//    //***User Area Begin
//    
//    //Read pedal inputs
//
//    D6_SetHigh();
//
//    ADC1_ChannelSelectSet(ADC1_APPS2);
//    ADC1_SamplingStart();
//    __delay_us(1);
//    ADC1_SamplingStop();
//    while(!ADC1_IsConversionComplete());
//    sensor2 = ADC1_Channel0ConversionResultGet();
//
//    ADC1_ChannelSelectSet(ADC1_BRK_F);
//    ADC1_SamplingStart();
//    __delay_us(1);
//    ADC1_SamplingStop();
//    while(!ADC1_IsConversionComplete());
//    sensor3 = ADC1_Channel0ConversionResultGet();
//
//    ADC1_ChannelSelectSet(ADC1_BRK_R);
//    ADC1_SamplingStart();
//    __delay_us(1);
//    ADC1_SamplingStop();
//    while(!ADC1_IsConversionComplete());
//    sensor4 = ADC1_Channel0ConversionResultGet();     
//
//    //parse 10-bit ADC readings to hi and lo 8-bit bytes
//    Pedal_data.frame.data0 = sensor1 & 0xff; //get lo byte
//    Pedal_data.frame.data1 = sensor1 >> 8;   //get hi byte  
//    Pedal_data.frame.data2 = sensor2 & 0xff; 
//    Pedal_data.frame.data3 = sensor2 >> 8;   
//    Pedal_data.frame.data4 = sensor3 & 0xff; 
//    Pedal_data.frame.data5 = sensor3 >> 8;   
//    Pedal_data.frame.data6 = sensor4 & 0xff;
//    Pedal_data.frame.data7 = sensor4 >> 8;  
//
//    CAN1_transmit(CAN_PRIORITY_MEDIUM, &Pedal_data); //send message
//
//    D5_Toggle();
//    D6_SetLow();
//
//    // ticker function call;
//    // ticker is 1 -> Callback function gets called everytime this ISR executes
//    //if(TMR1_InterruptHandler) 
//    //{ 
//    //       TMR1_InterruptHandler(); 
//    //}
//
//    //***User Area End
//
//    //tmr1_obj.count++;
//    //tmr1_obj.timerElapsed = true;
//    IFS0bits.T1IF = false;
//}
//
///**
// End of File
//*/
//
