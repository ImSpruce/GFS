#include <Arduino.h>
#include <IRremote.hpp>

#define IR_RECEIVE_PIN D6

#define LED1_RED_PIN D12    
#define LED2_BLUE_PIN D13

#define RGB_LED_BLUE_PIN D11
#define RGB_LED_GREEN_PIN D10
#define RGB_LED_RED_PIN D9

int blue = 128; // Variable to hold the blue LED brightness
int green = 128; // Variable to hold the green LED brightness
int red = 128; // Variable to hold the red LED brightness

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

/* Remote Button Mapping
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



void setup() {
    Serial.begin(115200);
    delay(1000); // Wait for Serial Monitor to open

    Serial.println("        ______________________________");
    Serial.println("        | ButtonMapping: (XY)         |");
    Serial.println("        | Button 1/1: - LED1 ON       |");
    Serial.println("        | Button 1/2: - LED1 OFF      |");
    Serial.println("        | Button 1/3: - LED2 ON       |");
    Serial.println("        | Button 1/4: - LED2 OFF      |");
    Serial.println("        |-----------------------------|");
    Serial.println("        | Button 2/1: - RGB +10 Blue  |");
    Serial.println("        | Button 2/2: - RGB -10 Blue  |");
    Serial.println("        | Button 2/3: - RGB +10 Green |");
    Serial.println("        | Button 2/4: - RGB -10 Green |");
    Serial.println("        | Button 2/5: - RGB +10 Red   |");
    Serial.println("        | Button 2/6: - RGB -10 Red   |");
    Serial.println("        |_____________________________|");
    Serial.print("\n");
         

    pinMode(LED1_RED_PIN, OUTPUT);
    pinMode(LED2_BLUE_PIN, OUTPUT);

    pinMode(RGB_LED_BLUE_PIN, OUTPUT);
    pinMode(RGB_LED_GREEN_PIN, OUTPUT);
    pinMode(RGB_LED_RED_PIN, OUTPUT);

    analogWrite(RGB_LED_BLUE_PIN, blue);
    analogWrite(RGB_LED_GREEN_PIN, green);
    analogWrite(RGB_LED_RED_PIN, red);

    IrReceiver.begin(IR_RECEIVE_PIN);
}

void loop() {
    if (IrReceiver.decode()) {

        if (IrReceiver.decodedIRData.protocol == NEC && !(IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT)) 
        {
            uint8_t command = IrReceiver.decodedIRData.command;

            Serial.print("Received command: 0x");
            Serial.printf("Received command: 0x%02X -> ", command);
    

            switch (command) {
                case REMOTE_BUTTON_11:
                    Serial.println("Button 1/1: LED1  ON");
                    digitalWrite(LED1_RED_PIN, HIGH);
                    break;

                case REMOTE_BUTTON_12:
                    Serial.println("Button 1/2: LED1 OFF");
                    digitalWrite(LED1_RED_PIN, LOW);
                    break;

                case REMOTE_BUTTON_13:
                    Serial.println("Button 1/3: LED2  ON");
                    digitalWrite(LED2_BLUE_PIN, HIGH);
                    break;

                case REMOTE_BUTTON_14: 
                    Serial.println("Button 1/4: LED2 OFF");
                    digitalWrite(LED2_BLUE_PIN, LOW);
                    break;

                case REMOTE_BUTTON_21:
                    Serial.println("Button 2/1: RGB  +10 Blue");
                    blue += 10;
                    if (blue > 255) blue = 255; // Cap at 255
                    Serial.print("RGB: ");
                    Serial.println(String(red) + ", " + String(green) + ", " + String(blue));
                    analogWrite(RGB_LED_BLUE_PIN, blue);
                    break;
                
                case REMOTE_BUTTON_22:
                    Serial.println("Button 2/2: RGB -10 Blue");
                    blue -= 10;
                    if (blue < 0) blue = 0; // Cap at 0
                    Serial.print("RGB: ");
                    Serial.println(String(red) + ", " + String(green) + ", " + String(blue));
                    analogWrite(RGB_LED_BLUE_PIN, blue);
                    break;

                case REMOTE_BUTTON_23:
                    Serial.println("Button 2/3: RGB +10 Green");
                    green += 10;
                    if (green > 255) green = 255; // Cap at 255
                    Serial.print("RGB: ");
                    Serial.println(String(red) + ", " + String(green) + ", " + String(blue));
                    analogWrite(RGB_LED_GREEN_PIN, green);
                    break;

                case REMOTE_BUTTON_24:
                    Serial.println("Button 2/4: RGB -10 Green");
                    green -= 10;
                    if (green < 0) green = 0; // Cap at 0
                    Serial.print("RGB: ");
                    Serial.println(String(red) + ", " + String(green) + ", " + String(blue));
                    analogWrite(RGB_LED_GREEN_PIN, green);
                    break;

                case REMOTE_BUTTON_25:
                    Serial.println("Button 2/5: RGB +10 Red");
                    red += 10;
                    if (red > 255) red = 255; // Cap at 255
                    Serial.print("RGB: ");
                    Serial.println(String(red) + ", " + String(green) + ", " + String(blue));
                    analogWrite(RGB_LED_RED_PIN, red);
                    break;

                case REMOTE_BUTTON_26:
                    Serial.println("Button 2/6: RGB -10 Red");
                    red -= 10;
                    if (red < 0) red = 0; // Cap at 0
                    Serial.print("RGB: ");
                    Serial.println(String(red) + ", " + String(green) + ", " + String(blue));
                    analogWrite(RGB_LED_RED_PIN, red);
                    break;

                default:
                    Serial.println(" Unknown command N/A");
                    break;
            }
        }
        IrReceiver.resume();
    }

    delay(10);

}


/*

; PlatformIO Project Configuration File
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
