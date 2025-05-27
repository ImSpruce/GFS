#include <Arduino.h>
#include <IRremote.hpp>

#define IR_RECEIVE_PIN 6  // Pin where IR receiver is connected

void setup() {
    Serial.begin(115200);
    while (!Serial);  // Wait for Serial Monitor
    
    Serial.println("IR Remote Analyzer Starting...");
    Serial.println("Point your remote at the receiver and press buttons");
    Serial.println("=====================================");
    
    // Initialize the IR receiver
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
    
    Serial.println("Ready - Press any button on your remote");
}

void loop() {
    if (IrReceiver.decode()) {
        
        // Print detailed information about received IR signal
        Serial.println("--- IR Signal Received ---");
        
        // Protocol information
        Serial.print("Protocol: ");
        Serial.println(getProtocolString(IrReceiver.decodedIRData.protocol));
        
        // Address (channel/device ID)
        Serial.print("Address: 0x");
        Serial.print(IrReceiver.decodedIRData.address, HEX);
        Serial.print(" (");
        Serial.print(IrReceiver.decodedIRData.address, DEC);
        Serial.println(")");
        
        // Command (button pressed)
        Serial.print("Command: 0x");
        Serial.print(IrReceiver.decodedIRData.command, HEX);
        Serial.print(" (");
        Serial.print(IrReceiver.decodedIRData.command, DEC);
        Serial.println(")");
        
        // Raw data
        Serial.print("Raw Data: 0x");
        Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
        
        // Additional flags
        if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) {
            Serial.println("Flag: REPEAT");
        }
        if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_WAS_OVERFLOW) {
            Serial.println("Flag: OVERFLOW");
        }
        
        // Number of bits
        Serial.print("Number of bits: ");
        Serial.println(IrReceiver.decodedIRData.numberOfBits);
        
        Serial.println("=====================================");
        Serial.println();
        
        // Resume receiving next signal
        IrReceiver.resume();
    }
    
    delay(50);  // Small delay to prevent overwhelming serial output
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
