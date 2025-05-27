#include <Arduino.h>
#include <LCDI2C_Multilingual_MCD.h>
#include <Keypad.h>
#include <AccelStepper.h>
#include <EEPROM.h>
#include <NonBlockingRtttl.h>


// EEPROM-Adressen
const int PIN_LENGTH_ADDR = 0;    // Adresse für die PIN-Länge 
const int PIN_ADDR = 1;           // Startadresse für den PIN

// Default-PIN als Fallback
const String DEFAULT_PIN = "0000";
const int MAX_PIN_LENGTH = 4;

String correctPin = "";
String enteredPin = "";
String newPin = "";


bool showEnteredPin = false;

enum SystemState {
  STATE_LOCKED,
  STATE_UNLOCKED,
  STATE_ENTER_OLD_PIN,
  STATE_ENTER_NEW_PIN,
  STATE_CONFIRM_NEW_PIN
};

SystemState currentState = STATE_LOCKED;



unsigned long showEnteredPinTime = 0;
unsigned int showEnteredPinDuration = 3000;

unsigned long lastKeyPressTime = 0;
unsigned int debounceDelay = 200;



/* ------------------------------- Pin Definition ------------------------------ */
const int RESET_PIN = PB6;  // DIP Switch 2

const int SPEAKER_PIN = D10;

const int GREEN_LED = PC1;
const int GREEN_LED2 = PC7; // Always on as it is connected to the keypad power pin - acts as a status ON LED
const int RED_LED = PC0;
const int RED_LED2 = PC3;

/* ---------------------------------- Melodys ---------------------------------- */
const char* melody = 
"Imperial:d=4,o=4,b=100:"
"g4,g4,g4,eb4,bb3,g4,eb4,bb3,g4,"
"d#4,d#4,d4,eb4,bb3,f#4,eb4,bb3,g4,"
"g4,g4,g4,eb4,bb3,g4,eb4,bb3,g4,"
"d#4,d#4,d4,eb4,bb3,f#4,eb4,bb3,g4,"
"g3,8g3,8g3,8g3,8g3,8g3,8g3,8g3,"
"g3,f#3,f3,d#3,eb3,bb2,f#3,eb3,bb2,g3";

const char* successMelody = "Success:d=4,o=5,b=60:16e6,16e6,16e6,16c6,8g6,16e6,16c6,8g6";
const char* errorMelody = "Error:d=4,o=5,b=60:16e6,16e6,16e6,16c6,8g6,16e6,16c6,8g6";
const char * mario = "mario:d=4,o=5,b=100:16e6,16e6,32p,8e6,16c6,8e6,8g6,8p,8g,8p,8c6,16p,8g,16p,8e,16p,8a,8b,16a#,8a,16g.,16e6,16g6,8a6,16f6,8g6,8e6,16c6,16d6,8b,16p,8c6,16p,8g,16p,8e,16p,8a,8b,16a#,8a,16g.,16e6,16g6,8a6,16f6,8g6,8e6,16c6,16d6,8b,8p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16g#,16a,16c6,16p,16a,16c6,16d6,8p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16c7,16p,16c7,16c7,p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16g#,16a,16c6,16p,16a,16c6,16d6,8p,16d#6,8p,16d6,8p,16c6";
const char * tetris = "tetris:d=4,o=5,b=160:e6,8b,8c6,8d6,16e6,16d6,8c6,8b,a,8a,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,2a,8p,d6,8f6,a6,8g6,8f6,e6,8e6,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,a";
const char * arkanoid = "Arkanoid:d=4,o=5,b=140:8g6,16p,16g.6,2a#6,32p,8a6,8g6,8f6,8a6,2g6";


/* ----------------------------------- Motor ----------------------------------- */

// Anzahl Schritte pro Umdrehung
const int stepsPerRevolution = 4096;

// Initialisiere Stepper das Stepper Objekt
// Stepper stepper(stepsPerRevolution, A4, A3, A2, A1);                                                 !!!!!! OLD
//AccelStepper stepper(AccelStepper::HALF4WIRE, A4, A2, A3, A1);
AccelStepper stepper(AccelStepper::HALF4WIRE, A4, A3, A2, A1);

/* ------------------------------------ LCD ------------------------------------ */
LCDI2C_Latin lcd(0x27, 16, 2);

// Bitmaps für die Umlaute
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
/* ein weitere platzt für Custrom Char
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

char hexaKeys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);


/* --------------------------- Funktionsdeklarationen -------------------------- */
void handlePinEntry(char key);

void displayPrompt(String topLine, String bottomLine = "");

void showSuccess(String message);
void showError(String message, String line2 = "");

void togglePinVisibility(String promt, String bottomLine = "");

void lcdPrintSpecial(String text);

void loadPinFromEEPROM();
void savePinToEEPROM(String pin);

void playTone(int frequency, int duration);

void smartDelay(unsigned long duration);

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

    stepper.setMaxSpeed(1000);    // Max. Geschwindigkeit (Steps/Sekunde)
    stepper.setAcceleration(500); // Beschleunigung (Steps/Sekunde²)

    lcd.init();

    // Erstellen der Sonderzeichen
    lcd.createChar(0, ae);
    lcd.createChar(1, AE);
    lcd.createChar(2, oe);
    lcd.createChar(3, OE);
    lcd.createChar(4, ue); 
    lcd.createChar(5, UE); 
    lcd.createChar(6, sz);

    lcd.backlight();
    
    // Überprüfe den Zustand des Reset-Pins
    if (digitalRead(RESET_PIN) == HIGH) {
        showError("    Reseting    ","     Board");
        delay(750);
        savePinToEEPROM("0000");
        showError("     Board", "     Reset");
    }

    // Lade den PIN aus dem EEPROM
    loadPinFromEEPROM();

    displayPrompt("Pin Eingeben: ");

    rtttl::begin(SPEAKER_PIN, successMelody);
}

void loop()
{   
    char keyPressed = keypad.getKey();
    
    rtttl::play();
    stepper.run();
    

    if (keyPressed)
    {
        if (millis() - lastKeyPressTime > debounceDelay)
        {
            lastKeyPressTime = millis();

            Serial.print("Es wurde: ");
            Serial.print(keyPressed);
            Serial.println(" gedrückt");
            
            switch (currentState)
            {
                case STATE_LOCKED:    
                    if (keyPressed == '#') // Eingabe löschen
                    {
                        enteredPin = "";
                        displayPrompt("Pin Eingeben: ");
                    }
                    else if (keyPressed == 'A') // Pin Sichtbarkeit umschalten
                    {
                        togglePinVisibility("Pin Eingeben: ");
                    }
                    else if (keyPressed == '*') // Eingabe bestätigen
                    {
                        if (enteredPin == correctPin) // korrekte Eingabe
                        {
                            currentState = STATE_UNLOCKED;
                            showEnteredPin = false;
                            Serial.println("Korrekter Pin");

                            showSuccess("Zugang gewährt");

                            stepper.move(stepsPerRevolution); // Motor auf eine volle Umdrehung bewegen
                            rtttl::begin(SPEAKER_PIN, successMelody);


                            displayPrompt("Pin ändern: (*)","Schließen:  (#)");
                        }
                        else
                        { 
                            // falsche Eingabe
                            Serial.print("Falscher Pin");
                            showError("Zugang", "verweigert");
                            displayPrompt("Pin Eingeben: ");

                        }
                        enteredPin = "";
                    }
                    else // Pin Eingabe
                    {
                        handlePinEntry(keyPressed);
                    }
                    break;
                    
                case STATE_UNLOCKED:
                    
                    if (keyPressed == '#')
                    {
                        // wieder schließen
                        currentState = STATE_LOCKED;

                        stepper.move(-stepsPerRevolution);

                        displayPrompt("Pin Eingeben: ");
                    }
                    if (keyPressed == '*') {
                        //pin ändern
                        currentState = STATE_ENTER_OLD_PIN;
                        displayPrompt("aktuellen Pin ","Eingeben: ");
                        enteredPin = "";
                    }
                    break;
                
                case STATE_ENTER_OLD_PIN:
                    
                    if (keyPressed == '#') {
                        //Pin Ändern Abbrechen
                        currentState = STATE_UNLOCKED;
                        enteredPin = "";
                        displayPrompt("Pin ändern", "abgebrochen");
                        smartDelay(300);
                        displayPrompt("Pin ändern: (*)","Schließen:  (#)");
                    }
                    else if (keyPressed == '*') {
                        // Eingabe bestätigen
                        if (enteredPin == correctPin) {
                            showEnteredPin = false;
                            Serial.println("Korrekter Pin");
                            currentState = STATE_ENTER_NEW_PIN;
                            displayPrompt("Neuen Pin", "Eingeben: ");
                        }
                        else {
                            showError("falscher Pin");
                            displayPrompt("aktuellen Pin ","Eingeben: ");
                        }
                        enteredPin = "";
                    }
                    else if (keyPressed == 'A')
                    {
                        togglePinVisibility("aktuellen Pin ","Eingeben: ");
                    }
                    else {
                        // pin Eingabe
                        handlePinEntry(keyPressed);
                    }
                    break;
                
                case STATE_ENTER_NEW_PIN:

                    if (keyPressed == '#') {
                        //Pin Ändern Abbrechen
                        currentState = STATE_UNLOCKED;
                        enteredPin = "";
                        displayPrompt("Pin ändern", "abgebrochen");
                        smartDelay(300);
                        displayPrompt("Pin ändern: (*)","Schließen:  (#)");
                    }
                    else if (keyPressed == '*') {
                        newPin = enteredPin;
                        currentState = STATE_CONFIRM_NEW_PIN;
                        enteredPin = "";
                        displayPrompt("Neuen Pin", "bestätigen: ");
                    }
                    else if (keyPressed == 'A')
                    {
                        togglePinVisibility("Neuen Pin", "Eingeben: ");
                    }
                    else {
                        handlePinEntry(keyPressed);
                    }
                    break;
                
                case STATE_CONFIRM_NEW_PIN:

                    if (keyPressed == '#') {
                        //Pin Ändern Abbrechen
                        currentState = STATE_UNLOCKED;
                        enteredPin = "";
                        newPin = "";
                        showError("Pin ändern", "abgebrochen");
                        smartDelay(300);
                        displayPrompt("Pin ändern: (*)","Schließen:  (#)");
                    }
                    else if (keyPressed == '*') {
                        // Neuen Pin bestätigen
                        if (enteredPin == newPin) {
                            correctPin = newPin;
                            savePinToEEPROM(correctPin);
                            enteredPin = "";
                            newPin = "";
                            currentState = STATE_UNLOCKED;

                            displayPrompt("Pin erfolgreich", "geändert");
                            smartDelay(1000);
                            displayPrompt("Pin ändern: (*)","Schließen:  (#)");    
                        }
                        else {
                            // Wenn die Bestätigung nicht mit dem neuen PIN übereinstimmt
                            showError("PINs stimmen", "nicht überein");
                            currentState = STATE_ENTER_NEW_PIN;
                            enteredPin = "";
                            displayPrompt("Neuen Pin", "Eingeben:");
                        }
                    }
                    else if (keyPressed == 'A')
                    {
                        togglePinVisibility("Neuen Pin", "bestätigen: ");
                    }
                    else {
                        handlePinEntry(keyPressed);
                    }
                    break;

                default:
                    // Fallback für unerwartete Zustände
                    currentState = STATE_LOCKED; // Zurück zum gesperrten Zustand
                    enteredPin = ""; // Eingabe zurücksetzen
                    break;

            }
        }
    }
    if (millis() - showEnteredPinTime > showEnteredPinDuration && showEnteredPin) // Hide Pin after duration
    {
        String topLine;
        String bottomLine;

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

/* --------------------------- Funktionsdefinitionen -------------------------- */

// Behandelt die Eingabe des PINs
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

// Zeigt eine Nachricht auf dem LCD an
void displayPrompt(String topLine, String bottomLine) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcdPrintSpecial(topLine);

    lcd.setCursor(0,1);
    if (bottomLine.length() > 0) {
        lcdPrintSpecial(bottomLine);
    }

}

// Zeigt eine Erfolgsmeldung mit grüner LED
void showSuccess(String message) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcdPrintSpecial(message);
    
    digitalWrite(GREEN_LED2, HIGH);
    smartDelay(500);
    digitalWrite(GREEN_LED2, LOW);
}

// Zeigt eine Fehlermeldung mit roter LED
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

// wechselt zwischen stern und ziffern anzeige
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

// gibt umlaute und sz aus
void lcdPrintSpecial(String text)
{
    for (int i = 0; i < text.length(); i++)
    {
        // UTF-8 Prüfung für deutsche Umlaute
        if (text[i] == 195)
        { // UTF-8 Prefix für Umlaute
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
                    default: lcd.print('?'); // Unbekannter Umlaut
                }
                i++; // Überspringe das zweite Byte des UTF-8 Zeichens
            }
        }
        // Normale ASCII-Zeichen
        else
        {
            lcd.print(text[i]);
        }
    }
}

// Lädt den PIN aus dem EEPROM
void loadPinFromEEPROM() {
    // Lese die Länge des PINs
    int pinLength = EEPROM.read(PIN_LENGTH_ADDR);
    
    // Überprüfe, ob die Länge valide ist
    if (pinLength > 0 && pinLength <= MAX_PIN_LENGTH) {
        correctPin = "";
        // Lese den PIN aus dem EEPROM
        for (int i = 0; i < pinLength; i++) {
            char digit = EEPROM.read(PIN_ADDR + i);
            correctPin += digit;
        }
        Serial.print("PIN aus EEPROM geladen: ");
        Serial.println(correctPin);
    } else {
        // Wenn kein PIN gespeichert ist, verwende den Default-PIN
        correctPin = DEFAULT_PIN;
        savePinToEEPROM(correctPin); // Speichere den Default-PIN
        Serial.println("Default-PIN gesetzt");
    }
}

// Speichert den PIN im EEPROM
void savePinToEEPROM(String pin) {
    // Speichere die Länge des PINs
    EEPROM.write(PIN_LENGTH_ADDR, pin.length());
    
    // Speichere den PIN selbst
    for (int i = 0; i < pin.length(); i++) {
        EEPROM.write(PIN_ADDR + i, pin[i]);
    }
    
    
    
    
    Serial.print("Neuer PIN im EEPROM gespeichert: ");
    Serial.println(pin);
}

// smartDelay Funktion without blocking
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
