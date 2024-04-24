#include "mbed.h"
#include "board_freedom.h"
 
  //smart-home
// main() runs in its own thread in the OS
int main()
{
    board_init();

    // Input
    DigitalIn car_sensor(PTE25);
    AnalogIn temp_sensor(ADC0_SE16);
    AnalogIn outside_light_sensor(ADC1_SE16);

    // Output
    DigitalOut Garage_Door (PTB11);
    
    
    
    // unsigned int pwm_min=580;


    
    pwm0.write (0.5);
    pwm0.period_ms(10);
    pwm1.write (0.5);
    pwm1.period_ms(10);  
    pwm2.write (0.5);
    pwm2.period_ms(10);
    Garden_Lamp = 1;


    while (true)
    {
        if (Car_Sensor == 0) {
            ThisThread::sleep_for(1s);
            Garage_Door = 1;
            ThisThread::sleep_for(5s);
            Garage_Door = 0; 
        }
    }
}
