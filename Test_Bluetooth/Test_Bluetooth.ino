void setup() {
  Serial1.begin(9600);
  Serial1.println("Bluetooth encendido");
  Serial1.println("1: on, 0: off");
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 0);
}

void loop() {
  if (Serial1.available()) {
    int data = Serial1.read();
    if (data == '1') {
      digitalWrite(LED_BUILTIN, 1);
    }
    if (data == '0') {
      digitalWrite(LED_BUILTIN, 0);
    }
  }
  delay(100);
}
