#include <Arduino.h>

/*
 * Speaker Test für STM32 mit ET-MINI AUDIO OUT
 * Testet verschiedene Töne und Melodien mit einem an PA1 angeschlossenen Lautsprecher
 */

// Pin-Definitionen
#define SPEAKER_PIN D10 // A1 entspricht PA1 auf dem STM32

// Töne definieren (Frequenzen in Hz)
#define TONE_SUCCESS 1000  // 1kHz Ton für erfolgreiche Eingabe
#define TONE_ERROR 200     // 200Hz Ton für falsche Eingabe

// Funktion zum Abspielen eines Tons für eine bestimmte Dauer
void playTone(int frequency, int duration) {
  // Periode in Mikrosekunden
  int period = 1000000 / frequency;
  int halfPeriod = period / 2;
  
  // Ton für die angegebene Dauer spielen
  long elapsed = 0;
  while (elapsed < duration * 1000) {
    digitalWrite(SPEAKER_PIN, HIGH);
    delayMicroseconds(halfPeriod);
    digitalWrite(SPEAKER_PIN, LOW);
    delayMicroseconds(halfPeriod);
    elapsed += period;
  }
}

// Erfolgston abspielen
void playSuccessTone() {
  playTone(TONE_SUCCESS, 500);  // 500ms Ton bei 1kHz
}

// Fehlerton abspielen
void playErrorTone() {
  playTone(TONE_ERROR, 1000);   // 1s Ton bei 200Hz
}

// Beispiel für eine Melodie bei erfolgreicher Eingabe
void playSuccessMelody() {
  playTone(659, 150);  // E5
  delay(50);
  playTone(784, 150);  // G5
  delay(50);
  playTone(1047, 300); // C6
}

// "Super Mario" Melodie - vollständiges Main Theme
void playMarioMelody() {
  // Teil 1 - Intro
  playTone(660, 100); delay(150); // E5
  playTone(660, 100); delay(300); // E5
  playTone(660, 100); delay(300); // E5
  playTone(510, 100); delay(100); // C5
  playTone(660, 100); delay(300); // E5
  playTone(770, 100); delay(550); // G5
  playTone(380, 100); delay(575); // G4
  
  // Teil 2
  playTone(510, 100); delay(450); // C5
  playTone(380, 100); delay(400); // G4
  playTone(320, 100); delay(500); // E4
  playTone(440, 100); delay(300); // A4
  playTone(480, 80);  delay(330); // B4
  playTone(450, 100); delay(150); // A#4/Bb4
  playTone(430, 100); delay(300); // A4
  
  // Teil 3
  playTone(380, 100); delay(200); // G4
  playTone(660, 80);  delay(200); // E5
  playTone(760, 50);  delay(150); // G5
  playTone(860, 100); delay(300); // C6
  playTone(760, 80);  delay(150); // G5
  playTone(860, 50);  delay(350); // C6
  playTone(700, 80);  delay(300); // F5
  
  // Teil 4
  playTone(760, 50);  delay(150); // G5
  playTone(660, 80);  delay(300); // E5
  playTone(520, 80);  delay(150); // C5
  playTone(580, 80);  delay(150); // D5
  playTone(480, 80);  delay(500); // B4
  
  // Teil 5 - Wiederholung Teil 2 mit Variation
  playTone(510, 100); delay(450); // C5
  playTone(380, 100); delay(400); // G4
  playTone(320, 100); delay(500); // E4
  playTone(440, 100); delay(300); // A4
  playTone(480, 80);  delay(330); // B4
  playTone(450, 100); delay(150); // A#4/Bb4
  playTone(430, 100); delay(300); // A4
  
  // Teil 6
  playTone(380, 100); delay(200); // G4
  playTone(660, 80);  delay(200); // E5
  playTone(760, 50);  delay(150); // G5
  playTone(860, 100); delay(300); // C6
  playTone(760, 80);  delay(150); // G5
  playTone(860, 50);  delay(350); // C6
  playTone(700, 80);  delay(300); // F5
  
  // Teil 7 - Abschluss
  playTone(760, 50);  delay(150); // G5
  playTone(660, 80);  delay(300); // E5
  playTone(520, 80);  delay(150); // C5
  playTone(580, 80);  delay(150); // D5
  playTone(480, 80);  delay(500); // B4
  
  // Schnelles Finale
  playTone(500, 60); // C5
  delay(150);
  playTone(500, 60); // C5
  delay(300);
  playTone(500, 60); // C5
  delay(150);
  playTone(500, 60); // C5
  delay(300);
  playTone(580, 80); // D5
  delay(150);
  playTone(660, 80); // E5
  delay(150);
  playTone(520, 80); // C5
  delay(150);
  playTone(440, 80); // A4
  delay(150);
  playTone(480, 80); // B4
  delay(150);
  playTone(450, 80); // A#4/Bb4
  delay(300);
  
  // Triumphierender Schlussteil
  playTone(430, 100); // A4
  delay(150);
  playTone(380, 100); // G4
  delay(150);
  playTone(660, 100); // E5
  delay(150);
  playTone(760, 100); // G5
  delay(150);
  playTone(860, 100); // C6
  delay(150);
  playTone(700, 100); // F5
  delay(150);
  playTone(760, 100); // G5
  delay(150);
  playTone(660, 100); // E5
  delay(300);
  
  // Schlusston
  playTone(520, 100); // C5
  delay(100);
  playTone(580, 100); // D5
  delay(100);
  playTone(480, 100); // B4
  delay(500);
}

void setup() {
  // Seriellen Monitor für Debug-Ausgaben initialisieren
  Serial.begin(115200);
  Serial.println("Speaker Test gestartet");
  
  // Lautsprecher-Pin als Ausgang konfigurieren
  pinMode(SPEAKER_PIN, OUTPUT);
  
  // Anfangsbeep zum Testen
  delay(1000);
  Serial.println("Einfacher Ton wird gespielt...");
  playTone(440, 500);  // A4 für 500ms
  delay(1000);
}

void loop() {
  // Test-Menü über seriellen Monitor
  Serial.println("\n--- Speaker Test Menü ---");
  Serial.println("1: Einfachen Ton abspielen");
  Serial.println("2: Erfolgston abspielen");
  Serial.println("3: Fehlerton abspielen");
  Serial.println("4: Erfolgsmelodie abspielen");
  Serial.println("5: Mario Melodie abspielen");
  Serial.println("Wählen Sie eine Option (1-5):");
  
  // Warte auf Benutzereingabe
  while (Serial.available() == 0) {
    // Warte auf Eingabe
  }
  
  // Eingabe lesen
  int choice = Serial.parseInt();
  
  // Ausführen basierend auf der Wahl
  switch (choice) {
    case 1:
      Serial.println("Spiele einfachen Ton (440Hz, 1s)...");
      playTone(440, 1000);
      break;
    case 2:
      Serial.println("Spiele Erfolgston...");
      playSuccessTone();
      break;
    case 3:
      Serial.println("Spiele Fehlerton...");
      playErrorTone();
      break;
    case 4:
      Serial.println("Spiele Erfolgsmelodie...");
      playSuccessMelody();
      break;
    case 5:
      Serial.println("Spiele Mario Melodie...");
      playMarioMelody();
      break;
    default:
      Serial.println("Ungültige Auswahl!");
      break;
  }
  
  delay(500); // Kurze Pause vor der nächsten Auswahl
}