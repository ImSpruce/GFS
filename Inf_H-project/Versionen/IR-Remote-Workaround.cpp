/*
* Code from Claude AI
*
* If **GREEN** RGB LED has to be changed the IR receiver must be **STOPPED**
* to prevent **TIMER CONFLICTS** with the PWM signal.
* After changing the green RGB LED the IR receiver must be **RESTARTED**.
* This is a **WORKAROUND** for the STM32 Nucleo L152RE board.
* This is necessary because the IR receiver uses the **SAME TIMER** as the PWM signal.
*
>>>>>>>>>>     GREEN LED AND IR RECEIVER SAME TIMER CONFLICT
*/




#include <Arduino.h>
#include <IRremote.hpp>

// Keep original D-notation pin definitions since they were working
#define IR_RECEIVE_PIN D6

#define LED1_RED_PIN D12    
#define LED2_BLUE_PIN D13

#define RGB_LED_BLUE_PIN D11
#define RGB_LED_GREEN_PIN D10
#define RGB_LED_RED_PIN D9

int blue = 128; // Variable to hold the blue LED brightness
int green = 128; // Variable to hold the green LED brightness
int red = 128; // Variable to hold the red LED brightness

// Define remote button codes
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

// Add recovery mechanisms
unsigned long lastValidIRTime = 0;
unsigned long corruptedSignalCount = 0;
const unsigned long IR_TIMEOUT = 3000; // 3 seconds timeout
const unsigned long MAX_CORRUPTED_SIGNALS = 3;

void resetIRReceiver() {
    Serial.println("*** IR Receiver appears stuck - resetting ***");
    IrReceiver.stop();
    delay(100);
    IrReceiver.begin(IR_RECEIVE_PIN);
    corruptedSignalCount = 0;
    Serial.println("IR Receiver reset complete");
}

void setup() {
    Serial.begin(115200);
    
    // Add delay for serial connection
    delay(2000);

    Serial.println("STM32 Nucleo L152RE IR LED Controller");
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
         
    // Initialize pins
    pinMode(LED1_RED_PIN, OUTPUT);
    pinMode(LED2_BLUE_PIN, OUTPUT);
    pinMode(RGB_LED_BLUE_PIN, OUTPUT);
    pinMode(RGB_LED_GREEN_PIN, OUTPUT);
    pinMode(RGB_LED_RED_PIN, OUTPUT);

    // Set initial RGB values
    analogWrite(RGB_LED_BLUE_PIN, blue);
    analogWrite(RGB_LED_GREEN_PIN, green);
    analogWrite(RGB_LED_RED_PIN, red);

    // Initialize IR receiver
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
    
    Serial.println("IR Receiver initialized on pin D6");
    Serial.println("Ready to receive IR commands...");
    
    
    lastValidIRTime = millis();
}

void loop() {
    // Check for IR timeout and reset if needed
    if (millis() - lastValidIRTime > IR_TIMEOUT && corruptedSignalCount > 0) {
        resetIRReceiver();
        lastValidIRTime = millis();
    }
    
    if (IrReceiver.decode()) {
        
        // Print all received data for debugging
        Serial.printf("Protocol: %d, Address: 0x%04X, Command: 0x%02X, Flags: 0x%02X\n", 
                     IrReceiver.decodedIRData.protocol,
                     IrReceiver.decodedIRData.address,
                     IrReceiver.decodedIRData.command,
                     IrReceiver.decodedIRData.flags);
        
        // Check for corrupted signals (Protocol 0, Command 0x00)
        if (IrReceiver.decodedIRData.protocol == 0 && IrReceiver.decodedIRData.command == 0x00) {
            corruptedSignalCount++;
            Serial.printf("Corrupted signal detected (count: %lu)\n", corruptedSignalCount);
            
            // If too many corrupted signals, reset immediately
            if (corruptedSignalCount >= MAX_CORRUPTED_SIGNALS) {
                resetIRReceiver();
                lastValidIRTime = millis();
            }
            
            IrReceiver.resume();
            return; // Skip processing corrupted signals
        }
        
        // Reset corrupted signal count on valid signal
        corruptedSignalCount = 0;
        lastValidIRTime = millis();
        
        // More flexible protocol handling - accept NEC, Protocol 8, and unknown protocols
        if ((IrReceiver.decodedIRData.protocol == NEC || 
             IrReceiver.decodedIRData.protocol == 8 ||
             IrReceiver.decodedIRData.protocol == UNKNOWN) && 
            !(IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT)) {
            
            uint8_t command = IrReceiver.decodedIRData.command;

            Serial.printf("Processing command: 0x%02X -> ", command);

            switch (command) {
                case REMOTE_BUTTON_11:
                    Serial.println("Button 1/1: LED1 ON");
                    digitalWrite(LED1_RED_PIN, HIGH);
                    break;

                case REMOTE_BUTTON_12:
                    Serial.println("Button 1/2: LED1 OFF");
                    digitalWrite(LED1_RED_PIN, LOW);
                    break;

                case REMOTE_BUTTON_13:
                    Serial.println("Button 1/3: LED2 ON");
                    digitalWrite(LED2_BLUE_PIN, HIGH);
                    break;

                case REMOTE_BUTTON_14: 
                    Serial.println("Button 1/4: LED2 OFF");
                    digitalWrite(LED2_BLUE_PIN, LOW);
                    break;

                case REMOTE_BUTTON_21:
                    Serial.println("Button 2/1: RGB +10 Blue");
                    blue = constrain(blue + 10, 0, 255);
                    Serial.printf("RGB: %d, %d, %d\n\n", red, green, blue);
                    analogWrite(RGB_LED_BLUE_PIN, blue);
                    break;
                
                case REMOTE_BUTTON_22:
                    Serial.println("Button 2/2: RGB -10 Blue");
                    blue = constrain(blue - 10, 0, 255);
                    Serial.printf("RGB: %d, %d, %d\n\n", red, green, blue);
                    analogWrite(RGB_LED_BLUE_PIN, blue);
                    break;

                case REMOTE_BUTTON_23:
                    Serial.println("Button 2/3: RGB +10 Green");
                    green = constrain(green + 10, 0, 255);
                    Serial.printf("RGB: %d, %d, %d\n\n", red, green, blue);
                    Serial.println("About to write to green pin...");
                    
                    // Temporarily disable IR receiver to prevent timer conflicts
                    IrReceiver.stop();
                    delay(10);
                    analogWrite(RGB_LED_GREEN_PIN, green);
                    delay(100); // Allow PWM to stabilize
                    
                    // Restart IR receiver
                    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
                    Serial.println("Green pin write completed with IR restart");
                    break;

                case REMOTE_BUTTON_24:
                    Serial.println("Button 2/4: RGB -10 Green");
                    green = constrain(green - 10, 0, 255);
                    Serial.printf("RGB: %d, %d, %d\n\n", red, green, blue);
                    Serial.println("About to write to green pin...");
                    
                    // Temporarily disable IR receiver to prevent timer conflicts
                    IrReceiver.stop();
                    delay(10);
                    analogWrite(RGB_LED_GREEN_PIN, green);
                    delay(100); // Allow PWM to stabilize
                    
                    // Restart IR receiver
                    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
                    Serial.println("Green pin write completed with IR restart");
                    break;

                case REMOTE_BUTTON_25:
                    Serial.println("Button 2/5: RGB +10 Red");
                    red = constrain(red + 10, 0, 255);
                    Serial.printf("RGB: %d, %d, %d\n\n", red, green, blue);
                    analogWrite(RGB_LED_RED_PIN, red);
                    break;

                case REMOTE_BUTTON_26:
                    Serial.println("Button 2/6: RGB -10 Red");
                    red = constrain(red - 10, 0, 255);
                    Serial.printf("RGB: %d, %d, %d\n\n", red, green, blue);
                    analogWrite(RGB_LED_RED_PIN, red);
                    break;

                default:
                    Serial.printf("Unknown command: 0x%02X (ignoring safely)\n", command);
                    break;
            }
        } else if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) {
            Serial.println("Repeat signal ignored");
        } else {
            Serial.printf("Non-standard protocol %d received - ignoring safely\n", IrReceiver.decodedIRData.protocol);
        }
        
        // Always resume receiving - this is critical!
        IrReceiver.resume();
        
        // Small delay to prevent overwhelming the system
        delay(50);
    }

    delay(10);
}