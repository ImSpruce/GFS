#include <Arduino.h>

// Definiere den Pin, der mit der eingebauten LED verbunden ist
// Bei den meisten STM32 Nucleo-Boards ist es Pin PC13


int duration = 100;
//int duration = 250;

void setup() {
  // Setze den LED-Pin als Ausgang
  
  Serial.begin(115200);
  Serial.println("Start 2");

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PC0, OUTPUT);
  pinMode(PC1, OUTPUT);
  pinMode(PC2, OUTPUT);
  pinMode(PC3, OUTPUT);
  pinMode(PC4, OUTPUT);
  pinMode(PC5, OUTPUT);
  pinMode(PC6, OUTPUT);
  pinMode(PC7, OUTPUT);
  
  pinMode(PC11, OUTPUT);
  pinMode(PC12, OUTPUT);

  digitalWrite(PC11, HIGH);
  digitalWrite(PC12, HIGH);
  
}

void loop() {
  

  // Schalte die LED ein
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(PC0, HIGH);
  digitalWrite(PC1, HIGH);
  digitalWrite(PC2, HIGH);
  digitalWrite(PC3, HIGH);
  digitalWrite(PC4, HIGH);
  digitalWrite(PC5, HIGH);
  digitalWrite(PC6, HIGH);
  digitalWrite(PC7, HIGH);
  delay(duration);  // Warte 1 Sekunde
  
  // Schalte die LED aus
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(PC0, LOW);
  digitalWrite(PC1, LOW);
  digitalWrite(PC2, LOW);
  digitalWrite(PC3, LOW);
  digitalWrite(PC4, LOW);
  digitalWrite(PC5, LOW);
  digitalWrite(PC6, LOW);
  digitalWrite(PC7, LOW);
  delay(duration);  // Warte 1 Sekunde
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

*/