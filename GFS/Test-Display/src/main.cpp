#include <Arduino.h>
#include <SevSeg.h>

// put function declarations here:
void blinkPlus(int);
void resetSegments();
void count();

// Erstelle das SevSeg-Objekt
SevSeg sevseg;

void setup() {
  // put your setup code here, to run once:

  // tests the STM32 by blinking the LED bultin
  pinMode(LED_BUILTIN,OUTPUT);
  blinkPlus(LED_BUILTIN);
  
  //7-Segement-Display Pins
  pinMode(PC11, OUTPUT);  // Einerstelle aktivieren
  pinMode(PC12, OUTPUT);  // Zehnerstelle aktivieren
  
  // Zunächst beide Stellen deaktivieren
  digitalWrite(PC11, LOW);
  digitalWrite(PC12, LOW);

  // Initialisiere das SevSeg-Objekt
  byte numDigits = 2;  // 2 Ziffern (Zehner- und Einerstelle)
  byte digitPins[] = {PC11, PC12};  // Die Pins für die Stellen
  byte segmentPins[] = {PC0, PC1, PC2, PC3, PC4, PC5, PC6, PC7};  // Die Pins für die Segmente

  sevseg.begin(COMMON_ANODE, numDigits, digitPins, segmentPins);  // 'COMMON_ANODE' für gemeinsame Anode
  sevseg.setBrightness(90);  // Helligkeit auf 90 setzen
}

void loop() {
  count();
}


// put function definitions here:
void blinkPlus(int pin) {
  for (int i = 0; i < 4; i++) {
    digitalWrite(pin, HIGH);
    delay(50);
    digitalWrite(pin,LOW);
    delay(50);
  }
  for (int i = 0; i < 2; i++) {
    digitalWrite(pin, HIGH);
    delay(100);
    digitalWrite(pin,LOW);
    delay(100);
  }
}


void count() {
  for (int count = 0; count < 99; count++) {
    int ones = count % 10;
    int tens = count / 10;
    

    for (int i = 0; i < 50; i++) {

      sevseg.setNumber(ones, 0);  // Zeige die Einerstelle
      sevseg.refreshDisplay();    // Anzeige aktualisieren
      delay(5);

      sevseg.setNumber(tens, 1);  // Zeige die Zehnerstelle
      sevseg.refreshDisplay();    // Anzeige aktualisieren
      delay(5);
    }
  }
}

