#include <Arduino.h>

#define BUTTON1_RED_PIN D3
#define BUTTON2_BLUE_PIN D2

#define BUTTON1_RED_PIN D3
#define BUTTON2_BLUE_PIN D2

#define BUZZER_PIN D5
#define IR_RECEIVE_PIN D6

#define FLOATING_SEED_PIN A3

int loserMelody[] = {262, 196, 196, 196};
int normalMelody[] = {330, 330, 294, 262};
int goodMelody[] = {392, 440, 494, 523};
int coolMelody[] = {523, 659, 784, 988};
// ChatGPT sounds

void setup()
{
    pinMode(LED1_RED_PIN, OUTPUT);
    pinMode(LED2_BLUE_PIN, OUTPUT);
    pinMode(BUTTON1_RED_PIN, INPUT_PULLUP);
    pinMode(BUTTON2_BLUE_PIN, INPUT_PULLUP);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(IR_RECEIVE_PIN, INPUT);

    Serial.begin(9600);
    randomSeed(analogRead(FLOATING_SEED_PIN)); // Für Zufall
}

// ----------------------------------------------------

void loop()
{
    long totalTime = 0;

    for (int i = 0; i < 20; i++)
    {
        int ledChoice = random(0, 2); // 0 = Rot, 1 = Blau

        // Zufällige Wartezeit (Anti-Vorhersage)
        delay(random(100, 3001));

        if (ledChoice == 0)
        {
            digitalWrite(LED1_RED_PIN, HIGH);
        }
        else
        {
            digitalWrite(LED2_BLUE_PIN, HIGH);
        }

        unsigned long start = millis();
        bool correct = false;
        while (!correct)
        {
            checkIRSensor(); // Währenddessen IR prüfen
            if (ledChoice == 0 && digitalRead(BUTTON1_RED_PIN) == LOW)
            {
                correct = true;
            }
            else if (ledChoice == 1 && digitalRead(BUTTON2_BLUE_PIN) == LOW)
            {
                correct = true;
            }
            else if (digitalRead(BUTTON1_RED_PIN) == LOW || digitalRead(BUTTON2_BLUE_PIN) == LOW)
            {
                Serial.println("Falscher Knopf! +200ms Strafe");
                totalTime += 200; // Strafe
                correct = true;   // Trotzdem weitermachen      //Todo vlt lieber mit break?
            }
        }

        unsigned long reaction = millis() - start;
        totalTime += reaction;

        Serial.printf("Reaktion %d: %lu ms\n", i + 1, reaction);

        digitalWrite(LED1_RED_PIN, LOW);
        digitalWrite(LED2_BLUE_PIN, LOW);
    }

    Serial.printf("Gesamtzeit: %ld ms\n", totalTime);


    if (totalTime <= 4750)
    {
        Serial.println("Super Sound!");
        playMelody(coolMelody, 4);
    }
    else if (totalTime <= 7000)
    {
        Serial.println("Guter Sound");
        playMelody(goodMelody, 4);
    }
    else if (totalTime <= 8000)
    {
        Serial.println("Normaler Sound");
        playMelody(normalMelody, 4);
    }
    else
    {
        Serial.println("Loser Sound");
        playMelody(loserMelody, 4);
    }

    delay(3000); // Pause
}

// IR-Erkennung
void checkIRSensor()
{
    if (digitalRead(IR_RECEIVE_PIN) == LOW)
    {
        Serial.println("IR erkannt!");
        tone(BUZZER_PIN, 880, 150);
        delay(200);
        noTone(BUZZER_PIN);
    }
}

// Buzzer spielt Melodie
void playMelody(int *melody, int len)
{
    for (int i = 0; i < len; i++)
    {
        tone(BUZZER_PIN, melody[i], 200);
        delay(250);
        noTone(BUZZER_PIN);
    }
}
