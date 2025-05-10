#include <Arduino.h>
#include <Stepper.h>

// Anzahl Schritte pro Umdrehung (abhängig vom Motor – beim 28BYJ-48 sind es meistens 2048)
const int stepsPerRevolution = 2048;

// Initialisiere Stepper mit 4 Steuerpins
Stepper myStepper(stepsPerRevolution, A5, A4, A3, A2);  // Reihenfolge beachten!

void setup() {
  myStepper.setSpeed(15);  // Geschwindigkeit in U/min
}

void loop() {
  // Dreht eine Umdrehung im Uhrzeigersinn
  myStepper.step(stepsPerRevolution);
  delay(50);

  // Dreht eine Umdrehung gegen den Uhrzeigersinn
  myStepper.step(-stepsPerRevolution);
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
    arduino-libraries/Stepper

*/