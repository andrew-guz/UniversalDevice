int LED_PIN = 2;

void setup(void) {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  Serial.println("On");
  digitalWrite(LED_PIN, HIGH);
  delay(1000);
  Serial.println("Off");
  digitalWrite(LED_PIN, LOW);
  delay(1000);
}
