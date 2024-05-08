#include "mbed.h"
#include "adc.h"
#include "board_freedom.h"
#include "oled_ssd1322.h"

#include <cstdint>
#include <cstdio>

// TODO
# define HEATING_TEMP 27
# define MAX_HEATING_TEMP 60
# define POLICE_LIGHT_THRESHOLD 40000

void display_message(char* message) {
    u8g2_int_t x = 30;
    u8g2_int_t y = 20;
    u8g2_ClearBuffer(&oled);
    u8g2_DrawUTF8(&oled, x, y, message);
    u8g2_SendBuffer(&oled);
}

// Conditional functions
float get_temp() {
    uint16_t analog_in_value = adc_read(0);
    float voltage = analog_in_value * 3 / 65535.0 ;
    return (voltage * 1000 - 400) / 19.5 ;
}

bool police_arrived() {
    DigitalIn car_sensor(PTE25);
    uint16_t outdoor_light_sensor = adc_read(1);
    printf("light level: %d\n", outdoor_light_sensor);
    return !car_sensor && outdoor_light_sensor >= POLICE_LIGHT_THRESHOLD;
}

// Output functions
void blink_light(PwmOut *inside_light) {
    *inside_light = 1;
    ThisThread::sleep_for(33ms);
    *inside_light = 0;
}

void blink_led(DigitalOut *led) {
    *led = 1;
    ThisThread::sleep_for(33ms);
    *led = 0;
}

void heat_or_cool(PwmOut *heater, DigitalOut *ventilator, DigitalOut *mux1, DigitalOut *mux2) {
    float outside_temp= get_temp();
    *mux1 = 0;
    *mux2 = 1;

    *mux1 = 0;
    *mux2 = 1;
    float heater_temp = get_temp();

    *mux1 = 1;
    *mux2 = 0;

    printf("heater temp %d\n", (int) heater_temp);
    if (outside_temp >= HEATING_TEMP && heater_temp <= MAX_HEATING_TEMP) {
        *heater = 1;
        *ventilator = 0;
    }
    else {
        *heater = 0;
        *ventilator = 1;
    }
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

    // OUTPUT
    DigitalOut front_door(PTE24);
    DigitalOut garage_door(PTB11);
    DigitalOut curtain(PTC7);
    PwmOut inside_light(PTC2);
    PwmOut garage_light(PTA6);

    // Freedom controller LED
    DigitalOut red_led(PTB2);

    PwmOut heater(PTA7);
    DigitalOut ventilator(PTC12);

    // Use outside temperature sensor
    DigitalOut mux1(PTA8);
    DigitalOut mux2(PTA9);
    mux1 = 1;
    mux2 = 0;

    bool emergency = false;
    // front_door = 0;
    // garage_door = 1;
    // curtain = 0;
    // inside_light = 0;
    // heater = 0;
    // ventilator = 0;
    



    while (true) {
        if (!emergency_button_released && !police_arrived()) emergency = true;
        
        if (police_arrived()) {
            char msg[] = "Police arrived!";
            printf("%s\n", msg);
            display_message(msg);

            emergency = false;
            front_door = 0;
            garage_door = 1;
            curtain = 0;
            inside_light = 0;
            heater = 0;
            ventilator = 0;
        } else if (emergency) {
            float temp = get_temp();
            char msg[100];

            if (temp >= HEATING_TEMP) {
                sprintf(msg, "EMERGENCY!!! HEATING MODE");
                printf("EMERGENCY!!! HEATING MODE, temp: %d\n", (int) temp);
            }
            else {
                sprintf(msg, "EMERGENCY!!! COOLING MODE");
                printf("EMERGENCY!!! COOLING MODE, temp: %d\n", (int) temp);
            }
            printf("EMERGENCY!!! \n");
            display_message(msg);
            
            front_door = 1;
            garage_door = 0;
            curtain = 1;
            blink_light(&inside_light);
            blink_light(&garage_light);
            blink_led(&red_led);
            heat_or_cool(&heater, &ventilator, &mux1, &mux2);
            // heater = 1;
            // ventilator = 0;
        }
        else {
            char msg[] = "No police and no emergency :)";
            
            printf("%s\n", msg);
            display_message(msg);

            // front_door = 0;
            // garage_door = 1;
            // curtain = 0;
            // inside_light = 0;
            // heater = 0;
            // ventilator = 0;
            
        }
    }
}
