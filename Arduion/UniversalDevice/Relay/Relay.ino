#define LED_PIN 2
#define POWER_PIN 26
#define DELAY 1000

void setup()
{
    pinMode(LED_PIN, OUTPUT);
    pinMode(POWER_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    digitalWrite(POWER_PIN, LOW);
}

void loop()
{
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(POWER_PIN, HIGH);
    delay(DELAY);
    digitalWrite(LED_PIN, LOW);
    digitalWrite(POWER_PIN, LOW);
    delay(DELAY);
}