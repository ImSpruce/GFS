/* 
 * RGB LED Control with IR Remote - Josua Meier - Noah Gerhold
 *  
 * This program allows you to control two single-color LEDs and an RGB LED using an IR remote.
 * You can toggle the LEDs and adjust the RGB color channels via remote buttons.
 * 
 */

/*
 * TODO:
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

#define POTENTIOMETER_PIN A0

#define LED1_RED_PIN D12    
#define LED2_BLUE_PIN D13

#define RGB_LED_BLUE_PIN  D11
#define RGB_LED_GREEN_PIN D10
#define RGB_LED_RED_PIN   D9 

// Define remote button codes with Remote_Button_XY format
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


bool led1_red_state = false;
bool led2_blue_state = false;

uint8_t blue =  0; 
uint8_t green = 0; 
uint8_t red =   0;

bool playSequence = false;
uint8_t currentStep = 0;
uint32_t lastStepTime = 0;
uint8_t currentSequenceIndex = 0;

float fxSpeed = 1;

struct SequenceStep {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    int16_t buzzerTone;  // -1 = no change
    int8_t led1;         // -1 = no change
    int8_t led2;         // -1 = no change
    uint16_t duration;   // in ms
};


// Sequence 1: Color Circle
SequenceStep rainbowSequence[] = {
    // Red, Green, Blue, Buzzer Tone, LED1 State, LED2 State, Duration (ms)
    
    // From Red to Green
    {255,   0,   0,  0, -1, -1, 80},
    {255,  64,   0,  0, -1, -1, 80},
    {255, 128,   0,  0, -1, -1, 80},
    {255, 192,   0,  0, -1, -1, 80},
    {255, 255,   0,  0, -1, -1, 80},  // Yellow
    {192, 255,   0,  0, -1, -1, 80},
    {128, 255,   0,  0, -1, -1, 80},
    { 64, 255,   0,  0, -1, -1, 80},
    {  0, 255,   0,  0, -1, -1, 80},  // Green

    // From Green to Blue
    {  0, 255,  64,  0, -1, -1, 80},
    {  0, 255, 128,  0, -1, -1, 80},
    {  0, 255, 192,  0, -1, -1, 80},
    {  0, 255, 255,  0, -1, -1, 80},  // Cyan
    {  0, 192, 255,  0, -1, -1, 80},
    {  0, 128, 255,  0, -1, -1, 80},
    {  0,  64, 255,  0, -1, -1, 80},
    {  0,   0, 255,  0, -1, -1, 80},  // Blue

    // From Blue to Red
    { 64,   0, 255,  0, -1, -1, 80},
    {128,   0, 255,  0, -1, -1, 80},
    {192,   0, 255,  0, -1, -1, 80},
    {255,   0, 255,  0, -1, -1, 80},  // Magenta
    {255,   0, 192,  0, -1, -1, 80},
    {255,   0, 128,  0, -1, -1, 80},
    {255,   0,  64,  0, -1, -1, 80},
    {255,   0,   0,  0, -1, -1, 80}   // Back to Red
};

// Sequence 2: Police Lights
SequenceStep policeSequence[] = {
    {255,   0,   0, 800,  1, -1, 200},  // Red flash + tone
    {  0,   0,   0,  -1, -1, -1, 100},  // Off
    {255,   0,   0, 800,  1, -1, 200},  // Red flash + tone
    {  0,   0,   0,   0, -1, -1, 300},  // Off + silence
    
    {  0,   0, 255, 600, -1,  1, 200},  // Blue flash + different tone
    {  0,   0,   0,  -1, -1, -1, 100},  // Off
    {  0,   0, 255, 600, -1,  1, 200},  // Blue flash + tone
    {  0,   0,   0,   0, -1, -1, 300}   // Off + silence
};

// Sequence 3: Fire Effect
SequenceStep fireSequence[] = {
    {255,  80,   0,  -1, -1, -1, 120},  // Orange
    {255, 120,  10,  -1, -1, -1, 100},  // Light orange
    {255,  60,   0,  -1, -1, -1, 150},  // Dark orange
    {255, 100,  20,  -1, -1, -1,  80},  // Yellow-orange
    {255,  40,   0,  -1, -1, -1, 200},  // Deep orange
    {255, 150,  30,  -1, -1, -1,  90},  // Bright orange
    {255,  70,   5,  -1, -1, -1, 110},  // Red-orange
    {255, 110,  15,  -1, -1, -1, 130}   // Medium orange
};

// Sequence 4: Ocean Waves
SequenceStep oceanSequence[] = {
    {  0,  50, 100,  -1, -1, -1, 300},  // Deep blue
    {  0,  80, 150,  -1, -1, -1, 250},  // Medium blue  
    {  0, 120, 200,  -1, -1, -1, 200},  // Light blue
    {  0, 150, 255,  -1, -1, -1, 150},  // Bright blue
    { 20, 180, 255,  -1, -1, -1, 100},  // Sky blue
    {  0, 150, 255,  -1, -1, -1, 150},  // Bright blue
    {  0, 120, 200,  -1, -1, -1, 200},  // Light blue
    {  0,  80, 150,  -1, -1, -1, 250},  // Medium blue
    {  0,  50, 100,  -1, -1, -1, 300}   // Deep blue
};

// Sequence 5: Disco Strobe
SequenceStep discoSequence[] = {
    {255,   0,   0, 1000,  1,  0,  80},  // Red strobe
    {  0,   0,   0,   -1, -1, -1,  40},  // Off
    {  0, 255,   0, 1200,  0,  1,  80},  // Green strobe
    {  0,   0,   0,   -1, -1, -1,  40},  // Off
    {  0,   0, 255, 1400,  1,  0,  80},  // Blue strobe
    {  0,   0,   0,   -1, -1, -1,  40},  // Off
    {255, 255,   0, 1600,  0,  1,  80},  // Yellow strobe
    {  0,   0,   0,   -1, -1, -1,  40},  // Off
    {255,   0, 255, 1800,  1,  1,  80},  // Magenta strobe
    {  0,   0,   0,   -1, -1, -1,  40},  // Off
    {  0, 255, 255, 2000,  0,  0,  80},  // Cyan strobe
    {  0,   0,   0,   -1, -1, -1,  40}   // Off
};


// Array of sequence pointers and their lengths
struct SequenceInfo {
    SequenceStep* steps;
    uint8_t length;
    const char* name;
};


SequenceInfo sequences[] = {
    {rainbowSequence, sizeof(rainbowSequence) / sizeof(SequenceStep)},
    {policeSequence, sizeof(policeSequence) / sizeof(SequenceStep)},
    {fireSequence, sizeof(fireSequence) / sizeof(SequenceStep)},
    {oceanSequence, sizeof(oceanSequence) / sizeof(SequenceStep)},
    {discoSequence, sizeof(discoSequence) / sizeof(SequenceStep)}
};

const uint8_t numSequences = sizeof(sequences) / sizeof(SequenceInfo);

// Function declarations
void software_PWM_Update(uint8_t pin, uint8_t duty);
void smartDelay(uint32_t ms);
void updateSequence();

void setup() {    
    Serial.begin(115200);

    Serial.print("\n \n \n"); 
    Serial.println("         _________________________________________________________________________________________       _________________ ");
    Serial.println("        | ButtonMapping: (XY)                                                                     |     |      Remote     |");
    Serial.println("        |                                                                                         |     |                 |");
    Serial.println("        | Button 1/1: - LED1 TOGGLE   | Button 2/1: - RGB +10 Blue  | Button 3/1: Start Party    |     | CH- | CH  | CH+ |");
    Serial.println("        | Button 1/2: - LED2 TOGGLE   | Button 2/2: - RGB -10 Blue  | Button 3/2: Stop Party     |     | |<< | >>| | >|| |");
    Serial.println("        | Button 1/3:                 | Button 2/3: - RGB +10 Red   | Button 3/3:                 |     |  -  |  +  | EQ  |");
    Serial.println("        | Button 1/4:                 | Button 2/4: - RGB -10 Red   | Button 3/4:                 |     |  0  | 100+| 200+|");
    Serial.println("        | Button 1/5:                 | Button 2/5: - RGB +10 Green | Button 3/5:                 |     |  1  |  2  |  3  |");
    Serial.println("        | Button 1/6:                 | Button 2/6: - RGB -10 Green | Button 3/6:                 |     |  4  |  5  |  6  |");
    Serial.println("        | Button 1/7:                 | Button 2/7:                 | Button 3/7:                 |     |  7  |  8  |  9  |");
    Serial.println("        |_____________________________|_____________________________|_____________________________|     |_________________|");
    Serial.print("\n \n");

    pinMode(LED1_RED_PIN, OUTPUT);
    pinMode(LED2_BLUE_PIN, OUTPUT);

    pinMode(RGB_LED_BLUE_PIN, OUTPUT);
    pinMode(RGB_LED_GREEN_PIN, OUTPUT);
    pinMode(RGB_LED_RED_PIN, OUTPUT);

    // Initialize RGB LED
    software_PWM_Update(RGB_LED_BLUE_PIN, blue);
    software_PWM_Update(RGB_LED_GREEN_PIN, green);
    software_PWM_Update(RGB_LED_RED_PIN, red);

    IrReceiver.begin(IR_RECEIVE_PIN);
}


void loop() {
    // Update RGB LED PWM
    software_PWM_Update(RGB_LED_BLUE_PIN, blue);
    software_PWM_Update(RGB_LED_GREEN_PIN, green);
    software_PWM_Update(RGB_LED_RED_PIN, red);

    fxSpeed = map(analogRead(POTENTIOMETER_PIN), 0, 1023, 500, 3000) / 1000;

    // Update sequence if active
    updateSequence();

    if (IrReceiver.decode()) {
        if (IrReceiver.decodedIRData.protocol == NEC && !(IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT)) {
            uint8_t command = IrReceiver.decodedIRData.command;
            Serial.printf("Received command: 0x%02X -> ", command);

            switch (command) {
                // ---------- RED LED ----------
                case REMOTE_BUTTON_11:
                    led1_red_state = !led1_red_state;
                    Serial.printf("Button 1/1: LED1 %s\n", led1_red_state ? "ON" : "OFF");
                    digitalWrite(LED1_RED_PIN, led1_red_state);
                    break;

                // ---------- BLUE LED ----------
                case REMOTE_BUTTON_12:
                    led2_blue_state = !led2_blue_state;
                    Serial.printf("Button 1/2: LED2 %s\n", led2_blue_state ? "ON" : "OFF");
                    digitalWrite(LED2_BLUE_PIN, led2_blue_state);
                    break;

                // ---------- RGB LED BLUE ---------- 
                case REMOTE_BUTTON_21:
                    Serial.println("Button 2/1: RGB +10 Blue");
                    playSequence = false;
                    blue = constrain(blue + 10, 0, 255);
                    Serial.printf("RGB: %03d, %03d, %03d\n", red, green, blue);
                    break;
                
                case REMOTE_BUTTON_22:
                    Serial.println("Button 2/2: RGB -10 Blue");
                    playSequence = false;
                    blue = constrain(blue - 10, 0, 255);
                    Serial.printf("RGB: %03d, %03d, %03d\n", red, green, blue);
                    break;

                // ---------- RGB LED RED ----------
                case REMOTE_BUTTON_23:
                    Serial.println("Button 2/3: RGB +10 Red");
                    playSequence = false;
                    red = constrain(red + 10, 0, 255);
                    Serial.printf("RGB: %03d, %03d, %03d\n", red, green, blue);
                    break;

                case REMOTE_BUTTON_24:
                    Serial.println("Button 2/4: RGB -10 Red");
                    playSequence = false;
                    red = constrain(red - 10, 0, 255);
                    Serial.printf("RGB: %03d, %03d, %03d\n", red, green, blue);
                    break;

                // ---------- RGB LED GREEN ----------
                case REMOTE_BUTTON_25:
                    Serial.println("Button 2/5: RGB +10 Green");
                    playSequence = false;
                    green = constrain(green + 10, 0, 255);
                    Serial.printf("RGB: %03d, %03d, %03d\n", red, green, blue);
                    break;
                
                case REMOTE_BUTTON_26:
                    Serial.println("Button 2/6: RGB -10 Green");
                    playSequence = false;
                    green = constrain(green - 10, 0, 255);
                    Serial.printf("RGB: %03d, %03d, %03d\n", red, green, blue);
                    break;

                // Start / Stop sequence
                case REMOTE_BUTTON_31:
                    Serial.printf("Button 3/1: %s Sequence\n", playSequence ? "Stop" : "Start");
                    playSequence = !playSequence;
                    noTone(BUZZER_PIN);
                    if (playSequence) {
                        currentStep = 0;
                        lastStepTime = millis();
                        Serial.printf("Playing sequence: %d\n", currentSequenceIndex + 1);
                    }
                    break;

                case REMOTE_BUTTON_32:
                    Serial.println("Button 3/2: Next Sequence");
                    noTone(BUZZER_PIN);
                    blue = false;
                    red = false;
                    currentSequenceIndex = (currentSequenceIndex + 1) % numSequences;
                    currentStep = 0;
                    break;

                case REMOTE_BUTTON_33:
                    Serial.println("Button 3/3: Previous Sequence");
                    noTone(BUZZER_PIN);
                    blue = false;
                    red = false;
                    currentSequenceIndex = (currentSequenceIndex - 1 + numSequences) % numSequences;
                    currentStep = 0;
                    break;

                // Unknown command
                default:
                    Serial.printf("Unknown command: 0x%02X\n", command);
                    break;
            }
        }
        IrReceiver.resume();
    }
}



// -------------------- Function definitions -------------------- \\ 

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


// Function to update the color sequence
void updateSequence() {
    if (playSequence && (millis() - lastStepTime >= sequences[currentSequenceIndex].steps[currentStep].duration / fxSpeed)) {
        lastStepTime = millis();
        
        SequenceStep step = sequences[currentSequenceIndex].steps[currentStep];
        
        // Update RGB colors
        red = step.red;
        green = step.green;
        blue = step.blue;
        
        // Update LEDs if specified
        if (step.led1 >= 0) {
            led1_red_state = step.led1;
            digitalWrite(LED1_RED_PIN, led1_red_state);
        }
        if (step.led2 >= 0) {
            led2_blue_state = step.led2;
            digitalWrite(LED2_BLUE_PIN, led2_blue_state);
        }
        
        // Update buzzer if specified
        if (step.buzzerTone > 0) {
            tone(BUZZER_PIN, step.buzzerTone);
        } else if (step.buzzerTone == 0) {
            noTone(BUZZER_PIN);
        }
        
        // Move to next step
        currentStep++;
        if (currentStep >= sequences[currentSequenceIndex].length) {
            currentStep = 0;  // Loop back to beginning
        }
        
        Serial.printf("Sequence Step %d: RGB(%d,%d,%d)\n", currentStep, red, green, blue);
    }
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