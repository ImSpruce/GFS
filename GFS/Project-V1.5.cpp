#include <Arduino.h>
#include <LCDI2C_Multilingual_MCD.h>
#include <Keypad.h>
#include <Stepper.h>

String correctPin = "1234";

String enteredPin = "";
String newPin = "";

bool locked = true;
bool showEnteredPin = false;
bool changingPin = false;
bool setNewPin = false;
bool confirmNewPin = false;

unsigned long showEnteredPinTime = 0;
unsigned int showEnteredPinDuration = 3500;

unsigned long lastKeyPressTime = 0;
unsigned int debounceDelay = 200;

const int GREEN_LED = PC1;
const int GREEN_LED2 = PC7; // immer an da an keypad versorgungs Pin - feature - fungirt als status ON LED
const int RED_LED = PC0;
const int RED_LED2 = PC3;

/* ------------------------ Motor ------------------------ */

// Anzahl Schritte pro Umdrehung
const int stepsPerRevolution = 2048;

// Initialisiere Stepper das Stepper Objekt
Stepper stepper(stepsPerRevolution, A4, A3, A2, A1);

/* ------------------------- LCD ------------------------- */
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

/* ein weitere platzt für Custrom Chars
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


/* ------------------------ Keypad ----------------------- */

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

/* ---------------- Funktionsdeklarationen --------------- */
void handlePinEntry(char key);
void displayPrompt(String topLine, String bottomLine = "");
void showSuccess(String message);
void showError(String message, String line2 = "");
void toogglePinVisibility(String promt, String bottomLine = "");
void lcdPrintSpecial(String text);

void setup()
{
    Serial.begin(115200);

    pinMode(RED_LED, OUTPUT);
    pinMode(RED_LED2, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);

    digitalWrite(RED_LED, LOW);
    digitalWrite(RED_LED2, LOW);
    digitalWrite(GREEN_LED, LOW);

    stepper.setSpeed(12); // Geschwindigkeit in U/min, bei mehr als 15 läuft der Motor aufgrund der Trägheit nicht mehr

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
    lcd.setCursor(0, 0);
    lcd.print("Pin Eingeben: ");
    lcd.setCursor(0, 1);
}

void loop()
{
    char keyPressed = keypad.getKey();

    if (keyPressed)
    {
        if (millis() - lastKeyPressTime > debounceDelay)
        {
            lastKeyPressTime = millis();

            Serial.print("Es wurde: ");
            Serial.print(keyPressed);
            Serial.println(" gedrückt");

            if (locked)
            {
                if (keyPressed == '#')
                {
                    // Eingabe löschen
                    enteredPin = "";
                    displayPrompt("Pin Eingeben: ");
                }
                else if (keyPressed == 'A')
                {
                    toogglePinVisibility("Pin Eingeben: ");
                }
                else if (keyPressed == '*')
                {
                    // Eingabe bestätigen
                    if (enteredPin == correctPin)
                    {
                        // korrekte Eingabe
                        locked = false;
                        showEnteredPin = false;
                        Serial.println("Korrekter Pin");

                        showSuccess("Zugang gewährt");

                        stepper.step(stepsPerRevolution);
                        digitalWrite(GREEN_LED, LOW); // Notwendig da der Motor die LED anschaltet, Pin doppelt belegt

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
                else // Pin Eingabe <- weder löschen noch senden
                {
                    handlePinEntry(keyPressed);
                }
            }
            else // unlocked
            {
                if (!changingPin && !setNewPin && !confirmNewPin) {
                    if (keyPressed == '#')
                    {
                        // wieder schließen
                        locked = true;
                        stepper.step(-stepsPerRevolution);
                        digitalWrite(GREEN_LED, LOW);
                        displayPrompt("Pin Eingeben: ");
                    }
                    if (keyPressed == '*') {
                        //pin ändern
                        changingPin = true;
                        displayPrompt("aktuellen Pin ","Eingeben: ");
                        enteredPin = "";
                    }
                }
                else if (changingPin && !setNewPin && !confirmNewPin) {
                    // Alten Pin Eingeben
                    if (keyPressed == '#') {
                        //Pin Ändern Abbrechen
                        changingPin = false;
                        enteredPin = "";
                        displayPrompt("Pin ändern", "abgebrochen");
                        delay(300);
                        displayPrompt("Pin ändern: (*)","Schließen:  (#)");
                    }
                    else if (keyPressed == 'A') {
                        toogglePinVisibility("aktuellen Pin ", "Eingeben: ");
                    }
                    else if (keyPressed == '*') {
                        // Eingabe bestätigen
                        if (enteredPin == correctPin) {
                            showEnteredPin = false;
                            Serial.println("Korrekter Pin");
                            setNewPin = true;
                            changingPin = false;
                            displayPrompt("Neuen Pin", "eingeben:");
                        }
                        else {
                            showError("falscher Pin");
                            displayPrompt("aktuellen Pin ","Eingeben: ");
                        }
                        enteredPin = "";
                    }
                    else {
                        // pin Eingabe
                        handlePinEntry(keyPressed);
                    }
                }
                else if (setNewPin && !confirmNewPin) {
                    if (keyPressed == '#') {
                        //Pin Ändern Abbrechen
                        changingPin = false;
                        setNewPin = false;
                        enteredPin = "";
                        displayPrompt("Pin ändern", "abgebrochen");
                        delay(300);
                        displayPrompt("Pin ändern: (*)","Schließen:  (#)");
                    }
                    else if (keyPressed == '*') {
                        newPin = enteredPin;
                        confirmNewPin = true;
                        setNewPin = false;
                        enteredPin = "";
                        displayPrompt("Neuen Pin", "bestätigen: ");
                    }
                    else {
                        handlePinEntry(keyPressed);
                    }
                }
                else if (confirmNewPin) {
                    if (keyPressed == '#') {
                        //Pin Ändern Abbrechen
                        changingPin = false;
                        setNewPin = false;
                        enteredPin = "";
                        newPin = "";
                        showError("Pin ändern", "abgebrochen");
                        delay(300);
                        displayPrompt("Pin ändern: (*)","Schließen:  (#)");
                    }
                    else if (keyPressed == '*') {
                        // Neuen Pin bestätigen
                        if (enteredPin == newPin) {
                            correctPin = newPin;
                            enteredPin = "";
                            newPin = "";
                            confirmNewPin = false;
                            setNewPin = false;
                            changingPin = false;

                            displayPrompt("Pin erfolgreich", "geändert");
                            delay(1000);
                            displayPrompt("Pin ändern: (*)","Schließen:  (#)");    
                        }
                        else {
                            // Wenn die Bestätigung nicht mit dem neuen PIN übereinstimmt
                            showError("PINs stimmen", "nicht überein");
                            confirmNewPin = false;
                            setNewPin = true;
                            enteredPin = "";
                            displayPrompt("Neuen Pin", "eingeben:");
                        }
                    }
                    else {
                        handlePinEntry(keyPressed);
                    }
                }
                
            }
        }
    }
    if (millis() - showEnteredPinTime > showEnteredPinDuration && showEnteredPin)
    {
        showEnteredPin = false;

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Pin Eingeben: ");
        lcd.setCursor(0, 1);

        for (int i = 0; i < enteredPin.length(); i++)
        {
            lcd.print('*');
        }
    }
}

/* ---------------- Funktionsdefinitionen ---------------- */

void handlePinEntry(char key) {
    enteredPin += key;
    if (showEnteredPin) {
        lcd.print(key);
    } else {
        lcd.print('*');
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
    delay(500);
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
        delay(100);
        digitalWrite(RED_LED, LOW);
        digitalWrite(RED_LED2, HIGH);
        delay(100);
    }
    digitalWrite(RED_LED, LOW);
    digitalWrite(RED_LED2, LOW);
    delay(800);
}

void toogglePinVisibility(String promt, String bottomLine) {
    showEnteredPin = !showEnteredPin;

    displayPrompt(promt , bottomLine);

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
                case 164: // ä
                    lcd.write(0);
                    break;
                case 132: // Ä
                    lcd.write(1);
                    break;
                case 182: // ö
                    lcd.write(2);
                    break;
                case 150: // Ö
                    lcd.write(3);
                    break;
                case 188: // ü
                    lcd.write(4);
                    break;
                case 156: // Ü
                    lcd.write(5);
                    break;
                case 159: // ß
                    lcd.write(6);
                    break;
                default:
                    lcd.print('?'); // Unbekannter Umlaut
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
    arduino-libraries/Stepper
    mcendu/LCDI2C_Multilingual_MCD@^2.1.0
    chris--a/Keypad @ 3.1.1

*/
