#define pinLED PD3

void setup() {
  Serial.begin(9600);
  pinMode(pinLED, OUTPUT);
  Serial.println("START");  
}

void loop() {
  digitalWrite(pinLED, HIGH);
  delay(100);
  digitalWrite(pinLED, LOW);
  delay(100);
  Serial.println("Hello World");  
}

