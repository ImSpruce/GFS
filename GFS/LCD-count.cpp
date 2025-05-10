#include <LCDI2C_Multilingual_MCD.h>


LCDI2C_Latin lcd(0x27, 16, 2);

void setup() {
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Sekunden: ");
}

void loop() {
  unsigned long currentMillis = millis();
  lcd.setCursor(0,1);
  lcd.print(currentMillis / 1000);
  lcd.print(" s  ");
  delay(50);
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

*/