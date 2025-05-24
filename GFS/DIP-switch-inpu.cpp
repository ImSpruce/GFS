#include <Arduino.h>

// Definiere den DIP-Switch-Pin gemäß Datenblatt
// Die DIP-Switches sind an PB0-PB7 angeschlossen
const int RESET_SWITCH_PIN = PB6;  // Du kannst auch einen anderen DIP-Switch verwenden (PB0-PB7)

// Indikator-LED
const int INDICATOR_LED = PC0;  // Rote LED an PC0

void setup() {
  // Starte die serielle Kommunikation
  Serial.begin(115200);
  Serial.println("System startet...");

  // Pin-Konfigurationen
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(INDICATOR_LED, OUTPUT);  // PC0 ist bereits als Output konfiguriert
  
  // DIP-Switch-Pin als Input mit Pulldown konfigurieren
  // Laut Datenblatt sind die DIP-Switches bereits als INPUT_PULLDOWN konfiguriert
  pinMode(RESET_SWITCH_PIN, INPUT_PULLDOWN);
  
  // Kurze Verzögerung für stabile Pin-Zustände
  delay(10);

  // Überprüfe den Zustand des Reset-DIP-Switches
  // DIP-Switches sind HIGH-aktiv (wenn sie ON sind, liefern sie HIGH)
  if (digitalRead(RESET_SWITCH_PIN) == HIGH) {
    // DIP-Switch ist ON (aktiviert)
    Serial.println("RESET-Modus erkannt! DIP-Switch PB6 ist aktiviert.");
    Serial.println("Default-Werte werden wiederhergestellt...");
    
    // Indikator-LED einschalten
    digitalWrite(INDICATOR_LED, HIGH);
    
    // Hier würde später der Code zum Zurücksetzen der Werte kommen
    // resetToDefaultValues();
    
    // Visuelles Feedback mit der eingebauten LED
    for (int i = 0; i < 5; i++) {
      digitalWrite(LED_BUILTIN, LOW);  // LED an (meist invertiert)
      delay(200);
      digitalWrite(LED_BUILTIN, HIGH); // LED aus
      delay(200);
    }
    
    delay(1000);
  } else {
    // DIP-Switch ist OFF (nicht aktiviert)
    Serial.println("Normaler Startvorgang... DIP-Switch PB6 ist nicht aktiviert.");
    digitalWrite(INDICATOR_LED, LOW);
  }
  
  Serial.println("System bereit!");
}

void loop() {
  // Normaler Betrieb: LED blinkt langsam als Heartbeat
  digitalWrite(LED_BUILTIN, LOW);   // LED an (Logik oft invertiert)
  delay(100);
  digitalWrite(LED_BUILTIN, HIGH);  // LED aus
  delay(100);
}