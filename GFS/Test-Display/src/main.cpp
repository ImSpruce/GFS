#include <Arduino.h>
#include <SevSeg.h>

// put function declarations here:
void blinkPlus(int);
void showDigit(int, int);
void resetSegments();
void count();



void setup() {
  // put your setup code here, to run once:

  // tests the STM32 by blinking the LED bultin
  pinMode(LED_BUILTIN,OUTPUT);
  blinkPlus(LED_BUILTIN);

  // 7-Segment-Display
  pinMode(PC0, OUTPUT);
  pinMode(PC1, OUTPUT);
  pinMode(PC2, OUTPUT);
  pinMode(PC3, OUTPUT);
  pinMode(PC4, OUTPUT);
  pinMode(PC5, OUTPUT);
  pinMode(PC6, OUTPUT);
  pinMode(PC7, OUTPUT);
  
  pinMode(PC11, OUTPUT);  // Einerstelle aktivieren
  pinMode(PC12, OUTPUT);  // Zehnerstelle aktivieren
  
  // Zunächst beide Stellen deaktivieren
  digitalWrite(PC11, LOW);
  digitalWrite(PC12, LOW);


  
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


void showDigit(int num, int positionPin) {
  // Ziffern-Tabelle (HIGH = Segment leuchtet, LOW = Segment aus)
  /*  
        dp g f e d c b a      a
      0b 0 0 0 0 0 0 0 0    f   b
                              g  
                            e   c
                              d
                                  dp
  */      

  const byte digitsAndChars[40] = {
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111, // 9

    /* Other Letters availabe with a 7 segment display
    0b01110111, // A
    0b01111100, // b
    0b00111001, // C
    0b01011110, // d
    0b01111001, // E
    0b01110001, // F
    0b00111101, // G
    0b01110110, // H
    0b00000110, // I (wie 1)
    0b00011110, // J
    0b11110100, // K (wie H)
    0b00111000, // L
    0b10010101, // M (nicht perfekt darstellbar)
    0b01010100, // N
    0b00111111, // O (wie 0)
    0b01110011, // P
    0b01100111, // q
    0b01010000, // r
    0b01101101, // S (wie 5)
    0b01111000, // T
    0b00111110, // U
    0b00111110, // V (wie U)
    0b10011101, // W (nicht perfekt darstellbar)
    0b01110110, // X (wie H)
    0b01101110, // Y
    0b01011011, // Z (wie 2)
    0b00000000, // Leerzeichen (alles aus)
    0b01000000, // - (Minus)
    0b10000000, // . (Punkt)
    0b00000001  // _ (Unterstrich)
    */
};

  byte segments = digitsAndChars[num];

  // checks for each segment wether it should be on
  digitalWrite(PC0, segments & 0b00000001);
  digitalWrite(PC1, segments & 0b00000010);
  digitalWrite(PC2, segments & 0b00000100);
  digitalWrite(PC3, segments & 0b00001000);
  digitalWrite(PC4, segments & 0b00010000);
  digitalWrite(PC5, segments & 0b00100000);
  digitalWrite(PC6, segments & 0b01000000);
  digitalWrite(PC7, segments & 0b10000000);

  // Stelle aktivieren
  digitalWrite(positionPin, HIGH);
}

void resetSegments() {
  digitalWrite(PC0, LOW);
  digitalWrite(PC1, LOW);
  digitalWrite(PC2, LOW);
  digitalWrite(PC3, LOW);
  digitalWrite(PC4, LOW);
  digitalWrite(PC5, LOW);
  digitalWrite(PC6, LOW);
  digitalWrite(PC7, LOW);
}

void count() {
  for (int count = 0; count < 99; count++) {
    int ones = count % 10;
    int tens = count / 10;

    for (int i = 0; i < 50; i++) {

      showDigit(ones, PC11);
      delay(5);
      digitalWrite(PC11, LOW);

      resetSegments();

      showDigit(tens, PC12);
      delay(5);
      digitalWrite(PC12, LOW);

      resetSegments();

    }
  }
}