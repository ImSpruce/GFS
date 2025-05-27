/*
  "+" LED 12 ON
  "1" LED 12 OFF

  "2" LED 13 ON
  "3" LED 13 OFF

  With the Car MP3 Remote
*/


#include <Arduino.h>
#include <IRremote.hpp>

#define IR_RECEIVE_PIN 6
#define LED1_PIN 12  // First LED
#define LED2_PIN 13  // Second LED (built-in LED)

// Define which remote buttons control which LEDs
// Change these values to match the buttons you want to use
#define BUTTON_LED1_ON  0x15   // Command 21 (0x15) - turn on LED1
#define BUTTON_LED1_OFF 0x0C   // Command 12 (0x0C) - turn off LED1
#define BUTTON_LED2_ON  0x18   // Command 24 (0x18) - turn on LED2
#define BUTTON_LED2_OFF 0x5E   // Command 94 (0x5E) - turn off LED2

void setup() {
    Serial.begin(115200);
    while (!Serial);
    
    // Initialize LED pins
    pinMode(LED1_PIN, OUTPUT);
    pinMode(LED2_PIN, OUTPUT);
    
    // Turn off both LEDs initially
    digitalWrite(LED1_PIN, LOW);
    digitalWrite(LED2_PIN, LOW);
    
    // Initialize IR receiver
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
    
    Serial.println("IR LED Controller Ready!");
    Serial.println("Button mapping:");
    Serial.println("Command 21 (0x15) -> LED D12 ON");
    Serial.println("Command 12 (0x0C) -> LED D12 OFF");
    Serial.println("Command 24 (0x18) -> LED D13 ON");
    Serial.println("Command 94 (0x5E) -> LED D13 OFF");
    Serial.println("=====================================");
}

void loop() {
    if (IrReceiver.decode()) {
        
        // Only process if it's NEC protocol and not a repeat
        if (IrReceiver.decodedIRData.protocol == NEC && 
            !(IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT)) {
            
            uint8_t command = IrReceiver.decodedIRData.command;
            
            Serial.print("Received command: 0x");
            Serial.print(command, HEX);
            Serial.print(" -> ");
            
            switch (command) {
                case BUTTON_LED1_ON:
                    digitalWrite(LED1_PIN, HIGH);
                    Serial.println("LED D12 ON");
                    break;
                    
                case BUTTON_LED1_OFF:
                    digitalWrite(LED1_PIN, LOW);
                    Serial.println("LED D12 OFF");
                    break;
                    
                case BUTTON_LED2_ON:
                    digitalWrite(LED2_PIN, HIGH);
                    Serial.println("LED D13 ON");
                    break;
                    
                case BUTTON_LED2_OFF:
                    digitalWrite(LED2_PIN, LOW);
                    Serial.println("LED D13 OFF");
                    break;
                    
                default:
                    Serial.println("Unknown button");
                    break;
            }
        }
        
        IrReceiver.resume();
    }
    
    delay(50);
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

