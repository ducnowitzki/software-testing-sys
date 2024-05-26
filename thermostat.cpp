#include "mbed.h"
#include "adc.h"
#include "oled_ssd1322.h"
#include "board_freedom.h"
#include "DigitalOut.h"
#include <cstdint>
#include <cstdio>


#define MESSAGE_MAX_SIZE 50
#define TEMPERATURE_LOW 30
#define TEMPERATURE_HIGH 35
#define TEMPERATURE_INVALID 20


float convert_value_to_temperature(uint16_t analog_in_value)
{
    float voltage = (analog_in_value  / 65535.0) * 3.0;
    return (voltage * 1000 - 400) / 19.5;
}


int main()
{
    // system init
    board_init();
    u8g2_ClearBuffer(&oled);
    u8g2_SetFont(&oled, u8g2_font_6x12_mr);
    u8g2_SendBuffer(&oled);

    PwmOut heater_power(PTA7);
    DigitalOut redLed(PTB2);
    DigitalOut greenLed(PTB3);
    DigitalOut greenLed2(PTE6);

    // turn on heater
    heater_power = 1;
    redLed = 1;

    // Ready a single reusable buffer for writing text to.
    char message[MESSAGE_MAX_SIZE + 1];
    message[MESSAGE_MAX_SIZE] = '\0';


    while (true)
    {
        // Read analog and convert
        uint16_t analog_in_value = adc_read(1);
        float temperature = convert_value_to_temperature(analog_in_value);

        // The temperature of the plate will be kept in the range [30-35] degrees Celsius.
        if (temperature < TEMPERATURE_INVALID) {
            // The system can detect the absence of a temperature sensor.
            heater_power = 0;
            greenLed = 0;
            redLed = 0;
            snprintf(message, MESSAGE_MAX_SIZE, "Check temperature sensor attachment\n");
        }
        else if (temperature < TEMPERATURE_LOW) {
            heater_power = 1;
            greenLed = 0;
            // When the heater is ON, a red LED should be turned ON.
            redLed = 1;
            snprintf(message, MESSAGE_MAX_SIZE, "temperature is %5.02f\n", temperature);
        }
        else if (temperature > TEMPERATURE_HIGH) {
            // Overheating will be prevented at all times.
            heater_power = 0;
            greenLed = 0;
            redLed = 0;
            snprintf(message, MESSAGE_MAX_SIZE, "temperature is %5.02f\n", temperature);
        }
        else {
            // When the temperature is in the specified range, a green LED will be turned ON.
            greenLed = 1;
            snprintf(message, MESSAGE_MAX_SIZE, "temperature is %5.02f\n", temperature);
        }
       


        // clear screen first
        u8g2_ClearBuffer(&oled);
        u8g2_DrawUTF8(&oled, 10, 10, message);
        u8g2_SendBuffer(&oled);
        
        // The temperature will be displayed in degrees Celsius on the LCD screen.
        printf("%s", message);

        ThisThread::sleep_for(1000ms);
    }
}
