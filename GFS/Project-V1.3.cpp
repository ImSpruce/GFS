#include <Arduino.h>
#include <LCDI2C_Multilingual_MCD.h>
#include <Keypad.h>
#include <Stepper.h>

String correctPin = "1234";

String enteredPin = "";

bool locked = true;
bool showEnteredPin = false;
unsigned long showEnteredPinTime = 0;
unsigned int showEnteredPinDuration = 2000;

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

// Einfachere und besser erkennbare Zeichendefinitionen für die Umlaute
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

/* ------------------------ Keypad ----------------------- */
const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

/* ---------------- Funktionsdeklarationen --------------- */
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

    stepper.setSpeed(15); // Geschwindigkeit in U/min, bei mehr läuft der Motor aufgrund der Trägheit nicht mehr

    lcd.init();

    // Erstellen der Sonderzeichen
    lcd.createChar(0, ae);
    lcd.createChar(1, AE);
    lcd.createChar(2, oe);
    lcd.createChar(3, OE);
    lcd.createChar(4, ue); 
    lcd.createChar(5, UE); 
    lcd.createChar(6, sz);
    delay(100);

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
                    lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.print("Pin Eingeben: ");
                    lcd.setCursor(0, 1);
                }
                else if (keyPressed == 'A')
                {
                    showEnteredPin = !showEnteredPin;

                    lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.print("Pin Eingeben: ");
                    lcd.setCursor(0, 1);

                    if (showEnteredPin)
                    {
                        showEnteredPinTime = millis();
                        lcd.print(enteredPin);
                    }
                    else
                    {
                        for (int i = 0; i < enteredPin.length(); i++)
                        {
                            lcd.print('*');
                        }
                    }
                }
                else if (keyPressed == '*')
                {
                    // Eingabe bestätigen
                    if (enteredPin == correctPin)
                    {
                        // korrekte Eingabe
                        locked = false;
                        showEnteredPin = false;
                        Serial.print("Korrekter Pin");
                        lcd.clear();
                        lcd.setCursor(0, 0);
                        lcdPrintSpecial("Zugang gewährt");
                        stepper.step(stepsPerRevolution);
                        digitalWrite(GREEN_LED, LOW); // Notwendig da der Motor die LED anschaltet, Pin doppelt belegt

                        lcd.clear();
                        lcd.setCursor(0, 0);
                        lcdPrintSpecial("Pin ändern: (1)");
                        lcd.setCursor(0, 1);
                        lcdPrintSpecial("Schließen: (#)");
                    }
                    else
                    {
                        // falsche Eingabe
                        Serial.print("Falscher Pin");
                        lcd.clear();
                        lcd.setCursor(0, 0);
                        lcd.print("Zugang");
                        lcd.setCursor(0, 1);
                        lcd.print("verweigert");

                        for (int i = 0; i < 5; i++)
                        {
                            digitalWrite(RED_LED, HIGH);
                            digitalWrite(RED_LED2, LOW);
                            delay(100);
                            digitalWrite(RED_LED, LOW);
                            digitalWrite(RED_LED2, HIGH);
                            delay(100);
                        }
                        digitalWrite(RED_LED, LOW);
                        digitalWrite(RED_LED2, LOW);

                        lcd.clear();
                        lcd.setCursor(0, 0);
                        lcd.print("Pin Eingeben: ");
                        lcd.setCursor(0, 1);
                    }
                    enteredPin = "";
                }
                else
                {
                    enteredPin += keyPressed;
                    if (showEnteredPin)
                    {
                        lcd.print(keyPressed);
                    }
                    else
                    {
                        lcd.print('*');
                    }
                }
            }
            else
            {
                if (keyPressed == '#')
                {
                    locked = true;
                    stepper.step(-stepsPerRevolution);
                    lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.print("Pin Eingeben: ");
                    lcd.setCursor(0, 1);
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