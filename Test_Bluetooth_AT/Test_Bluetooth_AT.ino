/*
 * Envia comandos AT al modulo bluetooth.
 * Conectar el modulo HC-05 a los pines TX1, RX1 (arduino due)
 * Antes de conectar la placa Arduino, presionar el boton del módulo.
 * Si titilea con un intervalo de 2 segundos, esta en modo AT.
 * Cargar este programa.
 * Abrir el serial monitor, a 9600, y setear "Both NL & CR"
 * Enviar "AT" (sin las comillas), tiene que responder OK.
 */

void setup() {
  Serial.begin(9600);
  Serial.println("Enter AT commands:");
  Serial1.begin(38400);
}

void loop() {
  if(Serial1.available()) {
    Serial.write(Serial1.read());
  }
  if(Serial.available()) {
    Serial1.write(Serial.read());
  }
}
