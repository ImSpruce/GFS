#include <Arduino.h>
#include <LCDI2C_Multilingual_MCD.h>
#include <Keypad.h>
#include <AccelStepper.h>
#include <EEPROM.h>
#include <NonBlockingRtttl.h>


// EEPROM addresses
const int PIN_LENGTH_ADDR = 0;    // Address for PIN length 
const int PIN_ADDR = 1;           // Start address for PIN

// Default PIN as fallback
const String DEFAULT_PIN = "0000";
const int MAX_PIN_LENGTH = 4;

// Global PIN variables
String correctPin = "";          // PIN stored in EEPROM (the actual correct PIN)
String enteredPin = "";          // Currently being entered PIN by user
String newPin = "";              // Temporary storage for new PIN during change process

// PIN display control
bool showEnteredPin = false;     // Flag to show actual digits vs asterisks

// System states for finite state machine
enum SystemState {
  STATE_LOCKED,           // Initial locked state, waiting for PIN entry
  STATE_UNLOCKED,         // Access granted, showing options menu
  STATE_ENTER_OLD_PIN,    // PIN change: entering current PIN for verification
  STATE_ENTER_NEW_PIN,    // PIN change: entering new PIN
  STATE_CONFIRM_NEW_PIN   // PIN change: confirming new PIN by re-entering
};

SystemState currentState = STATE_LOCKED;

// Timing control variables
unsigned long showEnteredPinTime = 0;        // Timestamp when PIN visibility was toggled
unsigned int showEnteredPinDuration = 3000;  // How long to show actual PIN digits (ms)

unsigned long lastKeyPressTime = 0;          // Last keypress timestamp for debouncing
unsigned int debounceDelay = 200;            // Debounce delay in milliseconds



/* ------------------------------- Pin Definition ------------------------------ */
const int RESET_PIN = PB6;  // DIP Switch 2

const int SPEAKER_PIN = D10;

const int GREEN_LED = PC1;
const int GREEN_LED2 = PC7; // Always on as it is connected to the keypad power pin - acts as a status ON LED
const int RED_LED = PC0;
const int RED_LED2 = PC3;

/* ---------------------------------- Melodys ---------------------------------- */
// RTTTL (Ring Tone Text Transfer Language) melody definitions
const char* bootupMelody = "Bootup:d=4,o=5,b=100:8c6,8e6,8g6,16a6,8g6,8c6";                // Played on bootup 
const char* successMelody = "Success:d=4,o=5,b=60:16e6,16e6,16e6,16c6,8g6,16e6,16c6,8g6";  // Played on successful PIN entry
const char* errorMelody = "Error:d=12,o=4,b=120:g4,b4,g4,b4,g4,b4,g4,b4";                      // Played on error (currently unused)
// Additional melodies available for future use
const char * mario = "mario:d=4,o=5,b=100:16e6,16e6,32p,8e6,16c6,8e6,8g6,8p,8g,8p,8c6,16p,8g,16p,8e,16p,8a,8b,16a#,8a,16g.,16e6,16g6,8a6,16f6,8g6,8e6,16c6,16d6,8b,16p,8c6,16p,8g,16p,8e,16p,8a,8b,16a#,8a,16g.,16e6,16g6,8a6,16f6,8g6,8e6,16c6,16d6,8b,8p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16g#,16a,16c6,16p,16a,16c6,16d6,8p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16c7,16p,16c7,16c7,p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16g#,16a,16c6,16p,16a,16c6,16d6,8p,16d#6,8p,16d6,8p,16c6";
const char * tetris = "tetris:d=4,o=5,b=160:e6,8b,8c6,8d6,16e6,16d6,8c6,8b,a,8a,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,2a,8p,d6,8f6,a6,8g6,8f6,e6,8e6,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,a";
const char * arkanoid = "Arkanoid:d=4,o=5,b=140:8g6,16p,16g.6,2a#6,32p,8a6,8g6,8f6,8a6,2g6";


/* ----------------------------------- Motor ----------------------------------- */

// Number of steps per revolution
const int stepsPerRevolution = 4096;

// Initialize stepper object
// Stepper stepper(stepsPerRevolution, A4, A3, A2, A1);                                                 !!!!!! OLD
//AccelStepper stepper(AccelStepper::HALF4WIRE, A4, A2, A3, A1);
AccelStepper stepper(AccelStepper::HALF4WIRE, A4, A3, A2, A1);

/* ------------------------------------ LCD ------------------------------------ */
LCDI2C_Latin lcd(0x27, 16, 2);

// Bitmaps for umlauts
byte ae[8] = {
    0b01010,
    0b00000,
    0b01110, 
    0b00001,
    0b01111,
    0b10001,
    0b01111,
    0b00000};
byte AE[8] = {
    0b01010, 
    0b00000,
    0b01110, 
    0b10001,
    0b11111,
    0b10001,
    0b10001,
    0b00000};
byte oe[8] = {
    0b01010, 
    0b00000,
    0b01110, 
    0b10001, 
    0b10001,
    0b10001,
    0b01110,
    0b00000};
byte OE[8] = {
    0b01010,
    0b00000,
    0b01110,
    0b10001,
    0b10001,
    0b10001,
    0b01110,
    0b00000
};
byte ue[8] = {
    0b01010, 
    0b00000,
    0b10001, 
    0b10001,
    0b10001,
    0b10001,
    0b01110,
    0b00000};
byte UE[8] = {
    0b01010, 
    0b00000,
    0b10001, 
    0b10001,
    0b10001,
    0b10001,
    0b01110,
    0b00000
};
byte sz[8] = {
    0b00000,
    0b01110,
    0b10001,
    0b11110,
    0b10001,
    0b11110,
    0b10000,
    0b10000,
};
/* Another space for custom character
byte xy[8] = {
    0b11111,
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    0b11111,
}
*/


/* ----------------------------------- Keypad ---------------------------------- */

const byte ROWS = 4;
const byte COLS = 4;

// 4x4 keypad layout - keys and their functions:
// A = Toggle PIN visibility (show digits vs asterisks)
// * = Confirm/Enter
// # = Clear/Cancel
// C = Play Mario melody (for fun) only when Unlocked
// D, B = Currently unused
char hexaKeys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

// Pin assignments for keypad rows and columns
byte rowPins[ROWS] = {9, 8, 7, 6};    // Connect to digital pins 9,8,7,6
byte colPins[COLS] = {5, 4, 3, 2};    // Connect to digital pins 5,4,3,2

Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);


/* --------------------------- Function Declarations -------------------------- */
void handlePinEntry(char key);

void displayPrompt(String topLine, String bottomLine = "");

void showSuccess(String message);
void showError(String message, String line2 = "");

void togglePinVisibility(String promt, String bottomLine = "");

void lcdPrintSpecial(String text);

void loadPinFromEEPROM();
void savePinToEEPROM(String pin);

void smartDelay(unsigned long duration);


/* ------------------------------ Setup Function ------------------------------ */
void setup()
{
    Serial.begin(115200);

    pinMode(RESET_PIN, INPUT_PULLDOWN);

    pinMode(RED_LED, OUTPUT);
    pinMode(RED_LED2, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);

    digitalWrite(RED_LED, LOW);
    digitalWrite(RED_LED2, LOW);
    digitalWrite(GREEN_LED, LOW);

    stepper.setMaxSpeed(1000);    // Max speed (steps/second)
    stepper.setAcceleration(500); // Acceleration (steps/second²)

    lcd.init();

    // Create special characters
    lcd.createChar(0, ae);
    lcd.createChar(1, AE);
    lcd.createChar(2, oe);
    lcd.createChar(3, OE);
    lcd.createChar(4, ue); 
    lcd.createChar(5, UE); 
    lcd.createChar(6, sz);

    lcd.backlight();
    
    // Check the state of the reset pin
    if (digitalRead(RESET_PIN) == HIGH) {
        showError("    Reseting    ","     Board");
        delay(750);
        savePinToEEPROM("0000");
        showError("     Board", "     Reset");
    }

    // Load PIN from EEPROM
    loadPinFromEEPROM();

    displayPrompt("Pin Eingeben: ");

    rtttl::begin(SPEAKER_PIN, bootupMelody); // Play bootup melody
}

void loop()
{   
    char keyPressed = keypad.getKey();
    
    // Non-blocking operations that must run continuously
    rtttl::play();    // Continue playing melody if active
    stepper.run();    // Continue stepper motor movement if active
    

    if (keyPressed)
    {
        // Debounce keypad input to prevent multiple rapid triggers
        if (millis() - lastKeyPressTime > debounceDelay)
        {
            lastKeyPressTime = millis();

            Serial.print("Es wurde: ");
            Serial.print(keyPressed);
            Serial.println(" gedrückt");
            
            // Handle keypress based on current system state
            switch (currentState)
            {
                case STATE_LOCKED:    
                    if (keyPressed == '#') // Clear input
                    {
                        enteredPin = "";
                        displayPrompt("Pin Eingeben: ");
                    }
                    else if (keyPressed == 'A') // Toggle PIN visibility
                    {
                        togglePinVisibility("Pin Eingeben: ");
                    }
                    else if (keyPressed == '*') // Confirm input
                    {
                        if (enteredPin == correctPin) // Correct input
                        {
                            // Successful PIN entry - unlock system
                            currentState = STATE_UNLOCKED;
                            showEnteredPin = false;
                            Serial.println("Korrekter Pin");

                            showSuccess("Zugang gewährt");

                            stepper.move(stepsPerRevolution); // Move motor one full revolution (unlock)
                            rtttl::begin(SPEAKER_PIN, successMelody);

                            displayPrompt("Pin ändern: (*)","Schließen:  (#)");
                        }
                        else
                        { 
                            // Wrong PIN entered - show error and stay locked
                            rtttl::begin(SPEAKER_PIN, errorMelody);
                            Serial.print("Falscher Pin");
                            showError("Zugang", "verweigert");
                            displayPrompt("Pin Eingeben: ");

                        }
                        enteredPin = "";
                    }
                    else if (keyPressed >= '0' && keyPressed <= '9') // PIN input (numeric keys)
                    {
                        handlePinEntry(keyPressed);
                    }
                    break;
                    
                case STATE_UNLOCKED:
                    
                    if (keyPressed == '#')
                    {
                        // Lock the system again
                        currentState = STATE_LOCKED;

                        stepper.move(-stepsPerRevolution);  // Reverse motor movement (lock)

                        displayPrompt("Pin Eingeben: ");
                    }
                    else if (keyPressed == '*') {
                        // Start PIN change process
                        currentState = STATE_ENTER_OLD_PIN;
                        displayPrompt("aktuellen Pin ","Eingeben: ");
                        enteredPin = "";
                    }
                    else if (keyPressed == 'C') {
                        if (rtttl::isPlaying()) {
                            rtttl::stop(); // Stop any currently playing melody
                        }
                        else {
                            // Play Mario melody for fun
                            Serial.println("Mario Melody wird gespielt");
                            rtttl::begin(SPEAKER_PIN, mario); // Play Mario melody
                        }
                    }
                    break;
                
                case STATE_ENTER_OLD_PIN:
                    
                    if (keyPressed == '#') {
                        // Cancel PIN change and return to unlocked menu
                        currentState = STATE_UNLOCKED;
                        enteredPin = "";
                        displayPrompt("Pin ändern", "abgebrochen");
                        smartDelay(300);
                        displayPrompt("Pin ändern: (*)","Schließen:  (#)");
                    }
                    else if (keyPressed == '*') {
                        // Verify current PIN before allowing change
                        if (enteredPin == correctPin) {
                            showEnteredPin = false;
                            Serial.println("Korrekter Pin");
                            currentState = STATE_ENTER_NEW_PIN;
                            displayPrompt("Neuen Pin", "Eingeben: ");
                        }
                        else {
                            // Wrong current PIN - show error and try again
                            rtttl::begin(SPEAKER_PIN, errorMelody);
                            showError("falscher Pin");
                            displayPrompt("aktuellen Pin ","Eingeben: ");
                        }
                        enteredPin = "";
                    }
                    else if (keyPressed == 'A')
                    {
                        togglePinVisibility("aktuellen Pin ","Eingeben: ");
                    }
                    else if (keyPressed >= '0' && keyPressed <= '9') // PIN input (numeric keys)
                    {
                        handlePinEntry(keyPressed);
                    }
                    break;
                
                case STATE_ENTER_NEW_PIN:

                    if (keyPressed == '#') {
                        // Cancel PIN change and return to unlocked menu
                        currentState = STATE_UNLOCKED;
                        enteredPin = "";
                        displayPrompt("Pin ändern", "abgebrochen");
                        smartDelay(300);
                        displayPrompt("Pin ändern: (*)","Schließen:  (#)");
                    }
                    else if (keyPressed == '*') {
                        // Store new PIN temporarily and move to confirmation
                        newPin = enteredPin;
                        currentState = STATE_CONFIRM_NEW_PIN;
                        enteredPin = "";
                        displayPrompt("Neuen Pin", "bestätigen: ");
                    }
                    else if (keyPressed == 'A')
                    {
                        togglePinVisibility("Neuen Pin", "Eingeben: ");
                    }
                    else if (keyPressed >= '0' && keyPressed <= '9') // PIN input (numeric keys)
                    {
                        handlePinEntry(keyPressed);
                    }
                    break;
                
                case STATE_CONFIRM_NEW_PIN:

                    if (keyPressed == '#') {
                        // Cancel PIN change and return to unlocked menu
                        currentState = STATE_UNLOCKED;
                        enteredPin = "";
                        newPin = "";
                        showError("Pin ändern", "abgebrochen");
                        smartDelay(300);
                        displayPrompt("Pin ändern: (*)","Schließen:  (#)");
                    }
                    else if (keyPressed == '*') {
                        // Verify that confirmation matches new PIN
                        if (enteredPin == newPin) {
                            // PINs match - save new PIN and return to menu
                            correctPin = newPin;
                            savePinToEEPROM(correctPin);
                            enteredPin = "";
                            newPin = "";
                            currentState = STATE_UNLOCKED;

                            rtttl::begin(SPEAKER_PIN, successMelody);

                            displayPrompt("Pin erfolgreich", "geändert");
                            smartDelay(1000);
                            displayPrompt("Pin ändern: (*)","Schließen:  (#)");    
                        }
                        else {
                            // Confirmation doesn't match - restart new PIN entry
                            rtttl::begin(SPEAKER_PIN, errorMelody);
                            showError("PINs stimmen", "nicht überein");
                            currentState = STATE_ENTER_NEW_PIN;
                            enteredPin = "";
                            displayPrompt("Neuen Pin", "Eingeben: ");
                        }
                    }
                    else if (keyPressed == 'A')
                    {
                        togglePinVisibility("Neuen Pin", "bestätigen: ");
                    }
                    else if (keyPressed >= '0' && keyPressed <= '9') // PIN input (numeric keys)
                    {
                        handlePinEntry(keyPressed);
                    }
                    break;

                default:
                    // Fallback for unexpected states
                    currentState = STATE_LOCKED; // Return to locked state
                    enteredPin = ""; // Reset input
                    break;

            }
        }
    }
    
    // Auto-hide PIN digits after timeout period
    if (millis() - showEnteredPinTime > showEnteredPinDuration && showEnteredPin) // Hide PIN after duration
    {
        String topLine;
        String bottomLine;

        // Determine appropriate prompt text based on current state
        switch (currentState) {
            case STATE_LOCKED:
                topLine = "Pin Eingeben: ";
                bottomLine = "";
                break;

            case STATE_ENTER_OLD_PIN:
                topLine = "aktuellen Pin ";
                bottomLine = "Eingeben: ";
                break;
            
            case STATE_ENTER_NEW_PIN:
                topLine = "Neuen Pin";
                bottomLine = "Eingeben: ";
                break;
            
            case STATE_CONFIRM_NEW_PIN:
                topLine = "Neuen Pin";
                bottomLine = "bestätigen: ";
                break;
            
            case STATE_UNLOCKED:
                topLine = "Pin ändern: (*)";
                bottomLine = "Schließen:  (#)";
                break;

            default:
                currentState = STATE_LOCKED; // Fallback
        }
        togglePinVisibility(topLine, bottomLine);
    }
}

/* --------------------------- Function Definitions -------------------------- */

// Handles PIN input
void handlePinEntry(char key) {
    if (enteredPin.length() < MAX_PIN_LENGTH) {
        enteredPin += key;
        if (showEnteredPin) {
            lcd.print(key);
        } else {
            lcd.print('*');
        }
    } else {
        showError("Max PIN Länge", "überschritten");
    }
}

// Displays a message on the LCD
void displayPrompt(String topLine, String bottomLine) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcdPrintSpecial(topLine);

    lcd.setCursor(0,1);
    if (bottomLine.length() > 0) {
        lcdPrintSpecial(bottomLine);
    }

}

// Shows success message with green LED
void showSuccess(String message) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcdPrintSpecial(message);
    
    digitalWrite(GREEN_LED2, HIGH);
    smartDelay(500);
    digitalWrite(GREEN_LED2, LOW);
}

// Shows error message with red LED
void showError(String message, String line2) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcdPrintSpecial(message);
    
    if (line2.length() > 0) {
        lcd.setCursor(0, 1);
        lcdPrintSpecial(line2);
    }
    
    for (int i = 0; i < 5; i++) {
        digitalWrite(RED_LED, HIGH);
        digitalWrite(RED_LED2, LOW);
        smartDelay(100);
        digitalWrite(RED_LED, LOW);
        digitalWrite(RED_LED2, HIGH);
        smartDelay(100);
    }
    digitalWrite(RED_LED, HIGH);
    digitalWrite(RED_LED2, HIGH);
    smartDelay(800);
    digitalWrite(RED_LED, LOW);
    digitalWrite(RED_LED2, LOW);
    
}

// Toggles between asterisk and digit display
void togglePinVisibility(String topLine, String bottomLine) {
    showEnteredPin = !showEnteredPin;

    displayPrompt(topLine , bottomLine);

    if (showEnteredPin) {
        showEnteredPinTime = millis();
        lcd.print(enteredPin);
    }
    else {
        for (int i = 0; i < enteredPin.length(); i++) {    
            lcd.print('*');
        }
    }
}

// Outputs umlauts and sz
void lcdPrintSpecial(String text)
{
    for (int i = 0; i < text.length(); i++)
    {
        // UTF-8 check for German umlauts
        if (text[i] == 195)
        { // UTF-8 prefix for umlauts
            if (i + 1 < text.length())
            {
                switch (text[i + 1])
                {
                    case 164: lcd.write(0); break; // ä
                    case 132: lcd.write(1); break; // Ä
                    case 182: lcd.write(2); break; // ö
                    case 150: lcd.write(3); break; // Ö
                    case 188: lcd.write(4); break; // ü
                    case 156: lcd.write(5); break; // Ü
                    case 159: lcd.write(6); break; // ß
                    default: lcd.print('?'); // Unknown umlaut
                }
                i++; // Skip the second byte of the UTF-8 character
            }
        }
        // Normal ASCII characters
        else
        {
            lcd.print(text[i]);
        }
    }
}

// Loads PIN from EEPROM
void loadPinFromEEPROM() {
    // Read PIN length
    int pinLength = EEPROM.read(PIN_LENGTH_ADDR);
    
    // Check if length is valid
    if (pinLength > 0 && pinLength <= MAX_PIN_LENGTH) {
        correctPin = "";
        // Read PIN from EEPROM
        for (int i = 0; i < pinLength; i++) {
            char digit = EEPROM.read(PIN_ADDR + i);
            correctPin += digit;
        }
        Serial.print("PIN aus EEPROM geladen: ");
        Serial.println(correctPin);
    } else {
        // If no PIN is stored, use default PIN
        correctPin = DEFAULT_PIN;
        savePinToEEPROM(correctPin); // Save default PIN
        Serial.println("Default-PIN gesetzt");
    }
}

// Saves PIN to EEPROM
void savePinToEEPROM(String pin) {
    // Save PIN length
    EEPROM.write(PIN_LENGTH_ADDR, pin.length());
    
    // Save PIN itself
    for (int i = 0; i < pin.length(); i++) {
        EEPROM.write(PIN_ADDR + i, pin[i]);
    }
    
    
    
    
    Serial.print("Neuer PIN im EEPROM gespeichert: ");
    Serial.println(pin);
}

// smartDelay function without blocking
void smartDelay(unsigned long duration) {
    unsigned long startTime = millis();
    while (millis() - startTime < duration) {
        rtttl::play();
        stepper.run();
    }
}


/*  ; PlatformIO Project Configuration File (platformio.ini)
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
	mcendu/LCDI2C_Multilingual_MCD@^2.1.0
	chris--a/Keypad @ 3.1.1
	end2endzone/NonBlockingRTTTL@^1.3.0
	waspinator/AccelStepper@^1.64

*/

