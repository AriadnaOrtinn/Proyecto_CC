#include <Servo.h>

Servo miServo;
const int pinServo = 9;

void setup() {
  miServo.attach(pinServo);
}

void loop() {
  // Barrido lento de 20 a 160 grados (evitamos los topes extremos 0 y 180)
  for (int pos = 20; pos <= 160; pos += 1) {
    miServo.write(pos);
    delay(20);
  }
  for (int pos = 160; pos >= 20; pos -= 1) {
    miServo.write(pos);
    delay(20);
  }
}
