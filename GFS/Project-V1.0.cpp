#include <Arduino.h>
#include <LCDI2C_Multilingual_MCD.h>
#include <Keypad.h>
#include <Stepper.h>

String correctPin = "1234";
String enteredPin = "";

// Anzahl Schritte pro Umdrehung
const int stepsPerRevolution = 2048;

// Initialisiere Stepper mit 4 Steuerpins
Stepper stepper(stepsPerRevolution, A5, A4, A3, A2);


LCDI2C_Latin lcd(0x27, 16, 2);

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

void setup() {
    Serial.begin(115200);

    stepper.setSpeed(15);  // Geschwindigkeit in U/min, bei mehr läuft der Motor aufgrund der Trägheit nicht mehr

    lcd.init();
    lcd.backlight();
    lcd.setCursor(0,0);
    lcd.print("Pin Eingeben: ");                          //Todo Umlaute implementiren.

}

void loop() {
    char keyPressed = keypad.getKey();

    if (keyPressed) {
        Serial.print("Es wurde: ");
        Serial.print(keyPressed);
        Serial.println(" gedrückt");

        if (keyPressed == '#') {
            //Eingabe löschen
            enteredPin = "";
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Pin Eingeben: ");
            lcd.setCursor(0,1);
        }
        else if (keyPressed == '*') {
            // Eingabe bestätigen
            
        }
        {
            /* code */
        }
        

        lcd.setCursor(0,1);
        lcd.print(keyPressed);

        if (keyPressed == '*') {
            stepper.step(stepsPerRevolution);
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