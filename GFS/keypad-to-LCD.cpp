#include <LCDI2C_Multilingual_MCD.h>
#include <Keypad.h>

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

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 


void setup() {

  Serial.begin(115200);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Gedrückt: ");
}

void loop() {
  char customKey = customKeypad.getKey();

  if (customKey){
    Serial.println(customKey);
    lcd.setCursor(0,1);
    lcd.print(customKey);
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
upload_protocol = stlink#include <LCDI2C_Multilingual_MCD.h>
#include <Keypad.h>

int current_millis = 0;

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

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 


void setup() {

    Serial.begin(115200);

    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Gedruckt:                       ");

    lcd.setCursor(0,1);
}

void loop() {
    current_millis =  getCurrentMillis();

    char customKey = customKeypad.getKey();
    

    if (customKey){
        Serial.println(customKey + " wurde gedrückt");
        if (customKey == '#'){
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Gedruckt:                       ");
            lcd.setCursor(0,1);
        }
        else {
            Serial.println(customKey);
            lcd.print(customKey);
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
	mcendu/LCDI2C_Multilingual_MCD@^2.1.0
    chris--a/Keypad @ 3.1.1
*/