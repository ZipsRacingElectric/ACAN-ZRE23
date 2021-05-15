/**
  Section: Included Files
*/
#include "mcc_generated_files/system.h"

#include <stdint.h>

#define FCY 40000000UL // Instruction cycle frequency, Hz - required for __delayXXX() to work
#include <libpic30.h>        // __delayXXX() functions macros defined here


#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/clock.h"
#include "mcc_generated_files/system.h"
#include "mcc_generated_files/system_types.h"
#include "mcc_generated_files/interrupt_manager.h"
#include "mcc_generated_files/traps.h"
#include "mcc_generated_files/adc1.h"
#include "mcc_generated_files/can1.h"

#define PEDAL_ID                0x005 //id of pedal data message from A/CAN board
#define PEDAL_MSG_SIZE            8 //pedal heartbeat message is 8 bytes

uint16_t APPS1_result = 0;
uint16_t APPS2_result = 0;
uint16_t BRK_F_result = 0;
uint16_t BRK_R_result = 0;

uCAN_MSG Pedal_data;

void __attribute__ ( ( interrupt, no_auto_psv ) ) _T1Interrupt (  );
/*
                         Main application
 */
int main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    
    Pedal_data.frame.idType = CAN_FRAME_STD;

    Pedal_data.frame.id = PEDAL_ID;
    Pedal_data.frame.dlc = PEDAL_MSG_SIZE;
    
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
        __delay_ms(200);        
    }
    
    return 1; 
}

void __attribute__ ( ( interrupt, no_auto_psv ) ) _T1Interrupt (  )
{
    /* Check if the Timer Interrupt/Status is set */

    //***User Area Begin
    
    //Read pedal inputs

    D6_SetHigh();
    
    ADC1_ChannelSelectSet(ADC1_APPS1);
    ADC1_SamplingStart();
    __delay_us(1); //delay for sampling capacitor
    //datasheet requires sample time of 2Tad (2*75 ns)
    ADC1_SamplingStop();
    while(!ADC1_IsConversionComplete());
    APPS1_result = ADC1_Channel0ConversionResultGet();

    ADC1_ChannelSelectSet(ADC1_APPS2);
    ADC1_SamplingStart();
    __delay_us(1);
    ADC1_SamplingStop();
    while(!ADC1_IsConversionComplete());
    APPS2_result = ADC1_Channel0ConversionResultGet();

    ADC1_ChannelSelectSet(ADC1_BRK_F);
    ADC1_SamplingStart();
    __delay_us(1);
    ADC1_SamplingStop();
    while(!ADC1_IsConversionComplete());
    BRK_F_result = ADC1_Channel0ConversionResultGet();

    ADC1_ChannelSelectSet(ADC1_BRK_R);
    ADC1_SamplingStart();
    __delay_us(1);
    ADC1_SamplingStop();
    while(!ADC1_IsConversionComplete());
    BRK_R_result = ADC1_Channel0ConversionResultGet();     

    //parse 10-bit ADC readings to hi and lo 8-bit bytes
    Pedal_data.frame.data0 = APPS1_result & 0xff; //get lo byte
    Pedal_data.frame.data1 = APPS1_result >> 8;   //get hi byte  
    Pedal_data.frame.data2 = APPS2_result & 0xff; 
    Pedal_data.frame.data3 = APPS2_result >> 8;   
    Pedal_data.frame.data4 = BRK_F_result & 0xff; 
    Pedal_data.frame.data5 = BRK_F_result >> 8;   
    Pedal_data.frame.data6 = BRK_R_result & 0xff;
    Pedal_data.frame.data7 = BRK_R_result >> 8;  

    CAN1_transmit(CAN_PRIORITY_MEDIUM, &Pedal_data); //send message

    D5_Toggle();
    D6_SetLow();

    // ticker function call;
    // ticker is 1 -> Callback function gets called everytime this ISR executes
    //if(TMR1_InterruptHandler) 
    //{ 
    //       TMR1_InterruptHandler(); 
    //}

    //***User Area End

    //tmr1_obj.count++;
    //tmr1_obj.timerElapsed = true;
    IFS0bits.T1IF = false;
}

/**
 End of File
*/

