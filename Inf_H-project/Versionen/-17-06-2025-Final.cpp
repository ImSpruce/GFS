/*
 * RGB LED Control with IR Remote - Josua Meier - Noah Gerhold
 *
 * This program allows you to control two single-color LEDs and an RGB LED using an IR remote.
 * You can toggle the LEDs and adjust the RGB color channels via remote buttons.
 * And play defined sequences with the LEDs and the Buzzer
 *
*/


/*
 * Future Improvements:
 * - Implement game melodies as sequences.
 * - Add more LED effects and patterns.
 * 
 * - Add User Sequence Builder
*/


#include <Arduino.h>
#include <IRremote.hpp>

#define IR_RECEIVE_PIN D6

#define BUZZER_PIN D5 // passive buzzer pin

#define BUTTON1_RED_PIN D3
#define BUTTON2_BLUE_PIN D2

#define POTENTIOMETER_PIN A0

#define LED1_RED_PIN D12
#define LED2_BLUE_PIN D13

#define RGB_LED_BLUE_PIN D11
#define RGB_LED_GREEN_PIN D10
#define RGB_LED_RED_PIN D9

#define FLOATING_SEED_PIN A3  // "free" pin to generate a random seed from interference

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

uint8_t blue = 0;
uint8_t green = 0;
uint8_t red = 0;

bool playSequence = false;
uint8_t currentStep = 0;
uint32_t lastStepTime = 0;
uint8_t currentSequenceIndex = 0;

float fxSpeed = 1.0;

struct SequenceStep
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    int16_t buzzerTone; // -1 = no change
    int8_t led1;        // -1 = no change
    int8_t led2;        // -1 = no change
    uint16_t duration;  // in ms
};

bool reactionGame = false;

int loserMelody[] = {262, 196, 196, 196};
int normalMelody[] = {330, 330, 294, 262};
int goodMelody[] = {392, 440, 494, 523};
int coolMelody[] = {523, 659, 784, 988}; 

// Sequence 1: Color Circle
// Red, Green, Blue, Buzzer Tone, LED1 Red State, LED2 Blue State, Duration (ms)
SequenceStep rainbowSequence[] = {
    // From Red to Green
    {255, 0, 0, 0, -1, -1, 80},
    {255, 64, 0, 0, -1, -1, 80},
    {255, 128, 0, 0, -1, -1, 80},
    {255, 192, 0, 0, -1, -1, 80},
    {255, 255, 0, 0, -1, -1, 80}, // Yellow
    {192, 255, 0, 0, -1, -1, 80},
    {128, 255, 0, 0, -1, -1, 80},
    {64, 255, 0, 0, -1, -1, 80},
    {0, 255, 0, 0, -1, -1, 80}, // Green

    // From Green to Blue
    {0, 255, 64, 0, -1, -1, 80},
    {0, 255, 128, 0, -1, -1, 80},
    {0, 255, 192, 0, -1, -1, 80},
    {0, 255, 255, 0, -1, -1, 80}, // Cyan
    {0, 192, 255, 0, -1, -1, 80},
    {0, 128, 255, 0, -1, -1, 80},
    {0, 64, 255, 0, -1, -1, 80},
    {0, 0, 255, 0, -1, -1, 80}, // Blue

    // From Blue to Red
    {64, 0, 255, 0, -1, -1, 80},
    {128, 0, 255, 0, -1, -1, 80},
    {192, 0, 255, 0, -1, -1, 80},
    {255, 0, 255, 0, -1, -1, 80}, // Magenta
    {255, 0, 192, 0, -1, -1, 80},
    {255, 0, 128, 0, -1, -1, 80},
    {255, 0, 64, 0, -1, -1, 80},
    {255, 0, 0, 0, -1, -1, 80} // Back to Red
};

// Sequence 2: Police Lights
SequenceStep policeSequence[] = {
    {255, 0, 0, 800, 0, 0, 200}, // Red flash + tone
    {0, 0, 0, -1, 0, 1, 100},    // Off
    {255, 0, 0, 800, 0, 0, 200}, // Red flash + tone
    {0, 0, 0, 0, 0, 1, 300},     // Off + silence

    {0, 0, 255, 600, 0, 0, 200}, // Blue flash + different tone
    {0, 0, 0, -1, 1, 0, 100},    // Off
    {0, 0, 255, 600, 0, 0, 200}, // Blue flash + tone
    {0, 0, 0, 0, 1, 0, 300}      // Off + silence
};

// Sequence 3: Fire Effect
SequenceStep fireSequence[] = {
    {255, 80, 0, -1, -1, -1, 120},   // Orange
    {255, 120, 10, -1, -1, -1, 100}, // Light orange
    {255, 60, 0, -1, -1, -1, 150},   // Dark orange
    {255, 100, 20, -1, -1, -1, 80},  // Yellow-orange
    {255, 40, 0, -1, -1, -1, 200},   // Deep orange
    {255, 150, 30, -1, -1, -1, 90},  // Bright orange
    {255, 70, 5, -1, -1, -1, 110},   // Red-orange
    {255, 110, 15, -1, -1, -1, 130}  // Medium orange
};

// Sequence 4: Ocean Waves
SequenceStep oceanSequence[] = {
    {0, 50, 100, -1, -1, -1, 300},   // Deep blue
    {0, 80, 150, -1, -1, -1, 250},   // Medium blue
    {0, 120, 200, -1, -1, -1, 200},  // Light blue
    {0, 150, 255, -1, -1, -1, 150},  // Bright blue
    {20, 180, 255, -1, -1, -1, 100}, // Sky blue
    {0, 150, 255, -1, -1, -1, 150},  // Bright blue
    {0, 120, 200, -1, -1, -1, 200},  // Light blue
    {0, 80, 150, -1, -1, -1, 250},   // Medium blue
    {0, 50, 100, -1, -1, -1, 300}    // Deep blue
};

// Sequence 5: Disco Strobe
SequenceStep discoSequence[] = {
    {255, 0, 0, 1000, 1, 0, 80},   // Red strobe
    {0, 0, 0, -1, -1, -1, 40},     // Off
    {0, 255, 0, 1200, 0, 1, 80},   // Green strobe
    {0, 0, 0, -1, -1, -1, 40},     // Off
    {0, 0, 255, 1400, 1, 0, 80},   // Blue strobe
    {0, 0, 0, -1, -1, -1, 40},     // Off
    {255, 255, 0, 1600, 0, 1, 80}, // Yellow strobe
    {0, 0, 0, -1, -1, -1, 40},     // Off
    {255, 0, 255, 1800, 1, 1, 80}, // Magenta strobe
    {0, 0, 0, -1, -1, -1, 40},     // Off
    {0, 255, 255, 2000, 0, 0, 80}, // Cyan strobe
    {0, 0, 0, -1, -1, -1, 40}      // Off
};

// Array of sequence pointers and their lengths
struct SequenceInfo
{
    SequenceStep *steps;
    uint8_t length;
};

// Array of sequences with sequence steps Arrays and their lengths
SequenceInfo sequences[] = {
    {rainbowSequence, sizeof(rainbowSequence) / sizeof(SequenceStep)},
    {policeSequence, sizeof(policeSequence) / sizeof(SequenceStep)},
    {fireSequence, sizeof(fireSequence) / sizeof(SequenceStep)},
    {oceanSequence, sizeof(oceanSequence) / sizeof(SequenceStep)},
    {discoSequence, sizeof(discoSequence) / sizeof(SequenceStep)}};


const uint8_t numSequences = sizeof(sequences) / sizeof(SequenceInfo);

// ---------------------------------------- Function declarations ---------------------------------------- \\ 
// Reaction Game
void playMelody(int *melody, int len);
void checkIRstopGame();

// main Code functions
void software_PWM_Update(uint8_t pin, uint8_t duty);
void smartDelay(uint32_t ms);
void updateSequence();
void update(uint8_t red, uint8_t green, uint8_t blue);

void setup()
{
    Serial.begin(115200);

    Serial.print("\n\n\n");
    Serial.println("         _________________________________________________________________________________________       _________________ ");
    Serial.println("        | ButtonMapping: (XY)                                                                     |     |      Remote     |");
    Serial.println("        |                                                                                         |     |                 |");
    Serial.println("        | Button 1/1: - LED1 TOGGLE   | Button 2/1: - RGB +10 Blue  | Button 3/1: Start Party     |     | CH- | CH  | CH+ |");
    Serial.println("        | Button 1/2: - LED2 TOGGLE   | Button 2/2: - RGB -10 Blue  | Button 3/2: Stop Party      |     | |<< | >>| | >|| |");
    Serial.println("        | Button 1/3:                 | Button 2/3: - RGB +10 Red   | Button 3/3:                 |     |  -  |  +  | EQ  |");
    Serial.println("        | Button 1/4:                 | Button 2/4: - RGB -10 Red   | Button 3/4:                 |     |  0  | 100+| 200+|");
    Serial.println("        | Button 1/5:                 | Button 2/5: - RGB +10 Green | Button 3/5:                 |     |  1  |  2  |  3  |");
    Serial.println("        | Button 1/6:                 | Button 2/6: - RGB -10 Green | Button 3/6:                 |     |  4  |  5  |  6  |");
    Serial.println("        | Button 1/7:                 | Button 2/7:                 | Button 3/7: Toggle Game     |     |  7  |  8  |  9  |");
    Serial.println("        |_____________________________|_____________________________|_____________________________|     |_________________|");
    Serial.print("\n\n");

    // Initialize pins
    pinMode(LED1_RED_PIN, OUTPUT);
    pinMode(LED2_BLUE_PIN, OUTPUT);

    pinMode(BUTTON1_RED_PIN, INPUT_PULLUP);
    pinMode(BUTTON2_BLUE_PIN, INPUT_PULLUP);

    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(IR_RECEIVE_PIN, INPUT);
    pinMode(POTENTIOMETER_PIN, INPUT);

    pinMode(RGB_LED_BLUE_PIN, OUTPUT);
    pinMode(RGB_LED_GREEN_PIN, OUTPUT);
    pinMode(RGB_LED_RED_PIN, OUTPUT);

    // Initialize RGB LED
    update(red, green, blue);

    randomSeed(analogRead(FLOATING_SEED_PIN));

    IrReceiver.begin(IR_RECEIVE_PIN);
}

void loop()
{
    // Update RGB LED PWM
    update(red, green, blue);
    updateSequence();

    // Update the speed of the effects based on the potentiometer - only works with shields that use right voltage
    // otherwise the potentiometer value at the end of the range is also low
    fxSpeed = map(analogRead(POTENTIOMETER_PIN), 0, 1023, 500, 3000) / 1000;

    if (IrReceiver.decode())
    {       
        // Check if the received data is from the NEC protocol and not a repeat code -> Button not held down
        if (IrReceiver.decodedIRData.protocol == NEC && !(IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT))
        {
            uint8_t command = IrReceiver.decodedIRData.command;
            Serial.printf("Received command: 0x%02X -> ", command);

            if (!reactionGame)
            {

                switch (command)
                {
                // ---------- RED LED ----------
                case REMOTE_BUTTON_11:
                    led1_red_state = !led1_red_state;
                    Serial.printf("Button 1/1: LED1 %s\n", led1_red_state ? "ON" : "OFF");
                    break;

                // ---------- BLUE LED ----------
                case REMOTE_BUTTON_12:
                    led2_blue_state = !led2_blue_state;
                    Serial.printf("Button 1/2: LED2 %s\n", led2_blue_state ? "ON" : "OFF");
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

                // ---------- Start / Stop sequence ----------
                case REMOTE_BUTTON_31:
                    Serial.printf("Button 3/1: %s Sequence\n", playSequence ? "Stop" : "Start");
                    playSequence = !playSequence;
                    noTone(BUZZER_PIN);
                    red = 0;
                    green = 0;
                    blue = 0;
                    led1_red_state = false;
                    led2_blue_state = false;
                    if (playSequence)
                    {
                        currentStep = 0;
                        lastStepTime = millis();
                        Serial.printf("Playing sequence: %d\n", currentSequenceIndex + 1);
                    }
                    break;

                case REMOTE_BUTTON_32:
                    Serial.println("Button 3/2: Next Sequence");
                    noTone(BUZZER_PIN);
                    led1_red_state = false;
                    led2_blue_state = false;
                    currentSequenceIndex = (currentSequenceIndex + 1) % numSequences;
                    currentStep = 0;
                    break;

                case REMOTE_BUTTON_33:
                    Serial.println("Button 3/3: Previous Sequence");
                    noTone(BUZZER_PIN);
                    led1_red_state = false;
                    led2_blue_state = false;
                    currentSequenceIndex = (currentSequenceIndex - 1 + numSequences) % numSequences;
                    currentStep = 0;
                    break;

                case REMOTE_BUTTON_37:
                    reactionGame = true;
                    playSequence = false;
                    noTone(BUZZER_PIN);
                    if (reactionGame)
                    {
                        Serial.println("Reaction Game Started");
                    }
                    break;

                // Unknown command
                default:
                    Serial.printf("Unknown command: 0x%02X\n", command);
                    break;
                }

                digitalWrite(LED1_RED_PIN, led1_red_state);
                digitalWrite(LED2_BLUE_PIN, led2_blue_state);
            }
        }
        IrReceiver.resume();
    }

    while (reactionGame)
    {
        long totalTime = 0;

        for (int i = 0; i < 20; i++)
        {
            int ledChoice = random(0, 2); // 0 = Rot, 1 = Blau

            // Zufällige Wartezeit (Anti-Vorhersage)
            smartDelay(random(100, 3001));      // Also Checks wether the game has been aborted

            if (!reactionGame)
            {
                totalTime = 10000000; // Reset total time
                break;                // Exit if reaction game is stopped
            }

            if (ledChoice == 0)
            {
                digitalWrite(LED1_RED_PIN, HIGH);
            }
            else
            {
                digitalWrite(LED2_BLUE_PIN, HIGH);
            }

            unsigned long start = millis();
            bool correct = false;
            while (!correct && reactionGame)
            {
                checkIRstopGame();

                if (ledChoice == 0 && digitalRead(BUTTON1_RED_PIN) == LOW)
                {
                    correct = true;
                }
                else if (ledChoice == 1 && digitalRead(BUTTON2_BLUE_PIN) == LOW)
                {
                    correct = true;
                }
                else if (digitalRead(BUTTON1_RED_PIN) == LOW || digitalRead(BUTTON2_BLUE_PIN) == LOW)
                {
                    Serial.println("Falscher Knopf! + 600ms Strafe");
                    totalTime += 600; // Strafe
                    break;
                }
            }

            unsigned long reaction = millis() - start;
            totalTime += reaction;

            Serial.printf("Reaktion %d: %lu ms\n", i + 1, reaction);

            digitalWrite(LED1_RED_PIN, LOW);
            digitalWrite(LED2_BLUE_PIN, LOW);
        }

        Serial.printf("Gesamtzeit: %ld ms\n", totalTime);

        if (totalTime <= 4750)
        {
            Serial.println("Super Sound!");
            playMelody(coolMelody, 4);
        }
        else if (totalTime <= 7000)
        {
            Serial.println("Guter Sound");
            playMelody(goodMelody, 4);
        }
        else if (totalTime <= 8000)
        {
            Serial.println("Normaler Sound");
            playMelody(normalMelody, 4);
        }
        else
        {
            Serial.println("Loser Sound");
            playMelody(loserMelody, 4);
        }

        smartDelay(1000); // Pause
    }
}

// ---------------------------------------- Function definitions ---------------------------------------- \\ 

// -------------------- Reaction Game Functions -------------------- \\ 

// Function to check if the IR remote sends a stop command during the reaction game.
// If the stop command (REMOTE_BUTTON_37) is received, the reaction game is terminated,
// the sequence playback is stopped, and the buzzer is silenced.
void checkIRstopGame()
{
    if (IrReceiver.decode() && IrReceiver.decodedIRData.protocol == NEC && !(IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT))
    {
        uint8_t command = IrReceiver.decodedIRData.command;
        Serial.printf("Received command: 0x%02X\n", command);

        if (command == REMOTE_BUTTON_37)
        {
            // Stop Reaction Game
            reactionGame = false;
            playSequence = false;
            noTone(BUZZER_PIN);
            Serial.println("Reaction Game Stopped");
        }
    }
    IrReceiver.resume();
}

// Buzzer plays melody
void playMelody(int *melody, int len)
{
    for (int i = 0; i < len; i++)
    {
        tone(BUZZER_PIN, melody[i], 200);
        smartDelay(250);
        noTone(BUZZER_PIN);
    }
}

// -------------------- Main Code Functions -------------------- \\ 

// Software PWM Update Function
// This function updates the state of a pin based on the duty cycle provided.
void software_PWM_Update(uint8_t pin, uint8_t duty)
{
    if (micros() % 255 < duty)
    {
        digitalWrite(pin, HIGH);
    }
    else
    {
        digitalWrite(pin, LOW);
    }
}

// Smart Delay Function
// This function allows the software PWM to update while waiting for a specified time.
void smartDelay(uint32_t ms)
{
    uint32_t start = millis();
    while (millis() - start < ms)
    {
        update(red, green, blue);
        if (reactionGame)
        {
            checkIRstopGame();
            if (!reactionGame)
            {
                break; // Exit if reaction game is stopped
            }
        }
    }
}

// Function to update the color sequence
void updateSequence()
{
    if (playSequence && fxSpeed > 0 && (millis() - lastStepTime >= sequences[currentSequenceIndex].steps[currentStep].duration / fxSpeed))
    {
        lastStepTime = millis();

        SequenceStep step = sequences[currentSequenceIndex].steps[currentStep];

        // Update RGB colors
        red = step.red;
        green = step.green;
        blue = step.blue;

        // Update LEDs if specified
        if (step.led1 >= 0)
        {
            led1_red_state = step.led1;
            digitalWrite(LED1_RED_PIN, led1_red_state);
        }
        if (step.led2 >= 0)
        {
            led2_blue_state = step.led2;
            digitalWrite(LED2_BLUE_PIN, led2_blue_state);
        }

        // Update buzzer if specified
        if (step.buzzerTone > 0)
        {
            tone(BUZZER_PIN, step.buzzerTone);
        }
        else if (step.buzzerTone == 0)
        {
            noTone(BUZZER_PIN);
        }

        // Move to next step
        currentStep++;
        if (currentStep >= sequences[currentSequenceIndex].length)
        {
            currentStep = 0; // Loop back to beginning
        }

        Serial.printf("Sequence Step %d: RGB(%03d,%03d,%03d)\n", currentStep, red, green, blue);
    }
}

void update(uint8_t red, uint8_t green, uint8_t blue)
{
    software_PWM_Update(RGB_LED_BLUE_PIN, red);
    software_PWM_Update(RGB_LED_GREEN_PIN, green);
    software_PWM_Update(RGB_LED_RED_PIN, blue);
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

