#include "mbed.h"
#include "adc.h"
#include "board_freedom.h"
#include "oled_ssd1322.h"

#include <cstdint>
#include <cstdio>

# define HEATING_TEMP 21
# define POLICE_LIGHT_THRESHOLD 0

void display_message(char* message) {
    //u8g2_ClearBuffer(&oled);
    u8g2_int_t x = 30;
    u8g2_int_t y = 20;
    u8g2_DrawUTF8(&oled, x, y, message);
    
    // Put all the changes in the buffer onto the screen.
    u8g2_SendBuffer(&oled);

}

float get_outside_temp() {
    uint16_t analog_in_value = adc_read(0);
    float voltage = analog_in_value * 3 / 65535.0 ;
    return (voltage * 1000 - 400) / 19.5 ;
}

bool police_arrived(DigitalIn car_sensor) {
    uint16_t outdoor_light_sensor = adc_read(1);
    return !car_sensor && outdoor_light_sensor >= POLICE_LIGHT_THRESHOLD;
}

bool heat() {
    return get_outside_temp() > HEATING_TEMP;
}



// main() runs in its own thread in the OS
int main()
{
    board_init();

    // DISPLAY setup
    // Clear the buffer (will clear the screen when sent)
    u8g2_ClearBuffer(&oled);
    // Ensure a font is set (only required once)
    u8g2_SetFont(&oled, u8g2_font_nokiafc22_tr);

    // INPUT
    DigitalIn emergency_button_released(PTB5);

    DigitalIn car_sensor(PTE25);
    //uint16_t temp_sensor = adc_read(0);
    //uint16_t outdoor_light_sensor = adc_read(1);

    // OUTPUT
    DigitalOut front_door(PTE24);
    DigitalOut garage_door(PTB11);
    DigitalOut curtain(PTC7);
    PwmOut inside_light(PTC2);

    // Freedom controller LED
    DigitalOut red_led(PTB2);
    // DigitalOut green_led(PTB3);
    // DigitalOut blue_led(PTB4);

    PwmOut heater(PTA7);
    DigitalOut ventilator(PTC12);

    
    // unsigned int pwm_min=580;

    // pwm0.write (0.5);
    // pwm0.period_ms(10);
    // pwm1.write (0.5);
    // pwm1.period_ms(10);  
    // pwm2.write (0.5);
    // pwm2.period_ms(10);
    bool emergency = false;

    while (true) {
        // printf("emergency %d\n", emergency);
        // printf("emergency_button_released %d\n", (int) emergency_button_released);
        // printf("police arrived %d\n", police_arrived(car_sensor));

        if (!emergency && !emergency_button_released && !police_arrived(car_sensor)) emergency = true;
        

        if (emergency) printf("emergency\n");
        else printf("no emergency\n");

        ThisThread::sleep_for(500ms);



    }
}
