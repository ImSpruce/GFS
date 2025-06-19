
/* 
 * RGB LED Control with IR Remote
 *  
*/



/*
 * TODO:
 * - Add support for more buttons 
 * - Add differt modes with color sequences
 * - Add support for potentiometer input for FX Speed
 * - Add support for buzzer output
 *
 *  Ideas:
 * - Reaction Game ??  
*/



#include <Arduino.h>
#include <IRremote.hpp>

#define IR_RECEIVE_PIN D6

#define BUZZER_PIN D5   // passive buzzer pin

//#define SWITCH1_PIN D2
//#define SWITCH2_PIN D3

//#define POTENTIOMETER_PIN A07

#define LED1_RED_PIN D12    
#define LED2_BLUE_PIN D13

#define RGB_LED_BLUE_PIN  D11
#define RGB_LED_GREEN_PIN D10
#define RGB_LED_RED_PIN   D9 

bool led1_red_state = false;
bool led2_blue_state = false;


int blue =  0; 
int green = 0; 
int red =   0; 


// Define remote button codes with
//      Remote_Button_XY format
#define REMOTE_BUTTON_11 0x45
#define REMOTE_BUTTON_12 0x44
#define REMOTE_BUTTON_13 0x07
#define REMOTE_BUTTON_14 0x16
#define REMOTE_BUTTON_15 0x0C
#define REMOTE_BUTTON_16 0x08
#define REMOTE_BUTTON_17 0x42

#define REMOTE_BUTTON_21 0x46
#define REMOTE_BUTTON_22 0x40
#define REMOTE_BUTTON_23 0x15
#define REMOTE_BUTTON_24 0x19
#define REMOTE_BUTTON_25 0x18
#define REMOTE_BUTTON_26 0x1C
#define REMOTE_BUTTON_27 0x52

#define REMOTE_BUTTON_31 0x47
#define REMOTE_BUTTON_32 0x43
#define REMOTE_BUTTON_33 0x09
#define REMOTE_BUTTON_34 0x0D
#define REMOTE_BUTTON_35 0x5E
#define REMOTE_BUTTON_36 0x5A
#define REMOTE_BUTTON_37 0x4A


/* Remote Button Mapping Adresses
 __________________________________________________
| C\R |       1      |       2      |       3      |
|-----|--------------|--------------|--------------|
|  1  | B01 0x45(69) | B02 0x46(70) | B03 0x47(71) |
|  2  | B04 0x44(68) | B05 0x40(64) | B06 0x43(67) |
|  3  | B07 0x07( 7) | B08 0x15(21) | B09 0x09( 9) |
|  4  | B10 0x16(22) | B11 0x19(25) | B12 0x0D(13) |
|  5  | B13 0x0C(12) | B14 0x18(24) | B15 0x5E(94) |
|  6  | B16 0x08( 8) | B17 0x1C(28) | B18 0x5A(90) |
|  7  | B19 0x42(66) | B20 0x52(82) | B21 0x4A(74) |
|__________________________________________________|
*/

/* Remote Button Mapping Symbols
 _________________
|      Remote     |    
| CH- | CH  | CH+ |
| |<< | >>| | >|| |
|  -  |  +  | EQ  |
|  0  | 100+| 200+|
|  1  |  2  |  3  |
|  4  |  5  |  6  |
|  7  |  8  |  9  |
|_________________|
*/

// Funktions Deklarationen

void software_PWM_Update(uint8_t pin, uint8_t duty);
void smartDelay(uint32_t ms);


void setup() {    
    Serial.begin(115200);



    Serial.print("\n \n \n"); 
    Serial.println("         _________________________________________________________________________________________       _________________ ");
    Serial.println("        | ButtonMapping: (XY)                                                                     |     |      Remote     |");
    Serial.println("        |                                                                                         |     |                 |");
    Serial.println("        | Button 1/1: - LED1 TOGGLE   | Button 2/1: - RGB +10 Blue  | Button 7/1:                 |     | CH- | CH  | CH+ |");
    Serial.println("        | Button 1/2: - LED2 TOGGLE   | Button 2/2: - RGB -10 Blue  | Button 7/2:                 |     | |<< | >>| | >|| |");
    Serial.println("        | Button 1/3:                 | Button 2/3: - RGB +10 Red   | Button 7/3:                 |     |  -  |  +  | EQ  |");
    Serial.println("        | Button 1/4:                 | Button 2/4: - RGB -10 Red   | Button 7/4:                 |     |  0  | 100+| 200+|");
    Serial.println("        | Button 1/5:                 | Button 2/5: - RGB +10 Green | Button 7/5:                 |     |  1  |  2  |  3  |");
    Serial.println("        | Button 1/6:                 | Button 2/6: - RGB -10 Green | Button 7/6:                 |     |  4  |  5  |  6  |");
    Serial.println("        | Button 1/7:                 | Button 2/7:                 | Button 7/7:                 |     |  7  |  8  |  9  |");
    Serial.println("        |_____________________________|_____________________________|_____________________________|     |_________________|");
    Serial.print("\n \n");


    pinMode(LED1_RED_PIN, OUTPUT);
    pinMode(LED2_BLUE_PIN, OUTPUT);

    pinMode(RGB_LED_BLUE_PIN, OUTPUT);
    pinMode(RGB_LED_GREEN_PIN, OUTPUT);
    pinMode(RGB_LED_RED_PIN, OUTPUT);

    software_PWM_Update(RGB_LED_BLUE_PIN, blue);
    software_PWM_Update(RGB_LED_GREEN_PIN, green);
    software_PWM_Update(RGB_LED_RED_PIN, red);





    IrReceiver.begin(IR_RECEIVE_PIN);
}

void loop() {

    software_PWM_Update(RGB_LED_BLUE_PIN, blue);
    software_PWM_Update(RGB_LED_GREEN_PIN, green);
    software_PWM_Update(RGB_LED_RED_PIN, red);

    if (IrReceiver.decode()) {

        if (IrReceiver.decodedIRData.protocol == NEC && !(IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT)) 
        {
            uint8_t command = IrReceiver.decodedIRData.command;

            Serial.printf("Received command: 0x%02X -> ", command);
    

            switch (command) {
                // ---------- ---------- ---------- ---------- ---------- RED LED
                case REMOTE_BUTTON_11:
                    led1_red_state = !led1_red_state;
                    Serial.printf("Button 1/1: LED1 %s\n", led1_red_state ? "ON" : "OFF");
                    digitalWrite(LED1_RED_PIN, led1_red_state);
                    break;


                // ---------- ---------- ---------- ---------- ---------- BLUE LED
                case REMOTE_BUTTON_12:
                    led2_blue_state = !led2_blue_state;
                    Serial.printf("Button 1/3: LED2 %s\n", led2_blue_state ? "ON" : "OFF");
                    digitalWrite(LED2_BLUE_PIN, led2_blue_state);
                    break;

                    
                // ---------- ---------- ---------- ---------- ---------- RGB LED BLUE
                case REMOTE_BUTTON_21:
                    Serial.println("Button 2/1: RGB +10 Blue");
                    blue = constrain(blue + 10, 0, 255);
                    Serial.printf("RGB: %03d, %03d, %03d\n", red, green, blue);
                    break;
                
                case REMOTE_BUTTON_22:
                    Serial.println("Button 2/2: RGB -10 Blue");
                    blue = constrain(blue - 10, 0, 255);
                    Serial.printf("RGB: %03d, %03d, %03d\n", red, green, blue);
                    break;


                // ---------- ---------- ---------- ---------- ---------- RGB LED RED
                case REMOTE_BUTTON_23:
                    Serial.println("Button 2/3: RGB +10 Red");
                    red = constrain(red + 10, 0, 255);
                    Serial.printf("RGB: %03d, %03d, %03d\n", red, green, blue);
                    software_PWM_Update(RGB_LED_RED_PIN, red);
                    break;

                case REMOTE_BUTTON_24:
                    Serial.println("Button 2/4: RGB -10 Red");
                    red = constrain(red - 10, 0, 255);
                    Serial.printf("RGB: %03d, %03d, %03d\n", red, green, blue);
                    break;


                // ---------- ---------- ---------- ---------- ---------- RGB LED GREEN
                case REMOTE_BUTTON_25:
                    Serial.println("Button 2/5: RGB +10 Green");
                    green = constrain(green + 10, 0, 255);
                    Serial.printf("RGB: %03d, %03d, %03d\n", red, green, blue);
                    break;
                
                case REMOTE_BUTTON_26:
                    Serial.println("Button 2/6: RGB -10 Green");
                    green = constrain(green - 10, 0, 255);
                    Serial.printf("RGB: %03d, %03d, %03d\n", red, green, blue);
                    break;

                // ---------- ---------- ---------- ---------- ---------- Unknown command
                default:
                    Serial.printf(" Unknown command  %s", command);
                    break;
            }
        }
        IrReceiver.resume();
    }

    delay(0);

}


// Funktions definitionen

// Software PWM Update Function
// This function updates the state of a pin based on the duty cycle provided.
void software_PWM_Update(uint8_t pin, uint8_t duty) {
    if (micros() % 255 < duty) {
        digitalWrite(pin, HIGH);
    } else {
        digitalWrite(pin, LOW);
    }
}


// Smart Delay Function
// This function allows the software PWM to update while waiting for a specified time.
void smartDelay(uint32_t ms) {
    uint32_t start = millis();
    while (millis() - start < ms) {
        software_PWM_Update(RGB_LED_BLUE_PIN, blue);
        software_PWM_Update(RGB_LED_GREEN_PIN, green);
        software_PWM_Update(RGB_LED_RED_PIN, red);
    }
}




void playSequence() {

}


/* ; PlatformIO Project Configuration File  
;
;   Build options: build flags, source filter
;   Upload options: custom upload port, speed and extra flags
;   Library options: dependencies, extra library storages
;   Advanced options: extra scripting
;
; Please visit documentation for the other options and examples
; https://docs.platformio.org/page/projectconf.html

[env:nucleo_l152re]
platform = ststm32
board = nucleo_l152re
framework = arduino
monitor_speed = 115200
upload_protocol = stlink
lib_deps = 
    ArminJo/IRremote@^4.4.0

*/
