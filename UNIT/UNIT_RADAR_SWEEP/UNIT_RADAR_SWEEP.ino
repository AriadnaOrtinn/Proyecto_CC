#include <Servo.h>

//----------- SERVOMOTOR -----------
Servo miServo;
const int pinServo = 9;
int anguloActual = 0;
int direccionSweep = 1;

//----------- RADAR (ULTRASONIDOS) -----------
const int trigPin = 6;
const int echoPin = 7;

//----------- CONTROL DE TIEMPO -----------
unsigned long T_UltimoRadar = 0;
const unsigned long IntervaloRadar = 500;

unsigned long T_UltimoServo = 0;
const unsigned long IntervaloServo = 30;

void setup() {
  Serial.begin(9600);

  miServo.attach(pinServo);
  anguloActual = 0;
  miServo.write(anguloActual);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.println("--- INICIANDO TEST UNITARIO: RADAR Y SWEEP ---");
  delay(1000); // Pausa inicial para dar tiempo al servo a ir a 0 grados
}

void loop() {
  unsigned long T_Actual = millis();

  // 1. Movimiento fluido del servo (cada 30 ms)
  if (T_Actual - T_UltimoServo >= IntervaloServo) {
    T_UltimoServo = T_Actual;

    anguloActual += direccionSweep;

    if (anguloActual >= 180) {
      anguloActual = 180;
      direccionSweep = -1;
    } 
    else if (anguloActual <= 0) {
      anguloActual = 0;
      direccionSweep = 1;
    }

    miServo.write(anguloActual);
  }

  // 2. Medición de distancia y envío por Monitor Serie (cada 500 ms)
  if (T_Actual - T_UltimoRadar >= IntervaloRadar) {
    T_UltimoRadar = T_Actual;
    medirYMostrar(anguloActual);
  }
}

void medirYMostrar(int angulo) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Añadimos un timeout de 30000 us (~5 metros) para evitar bloqueos si no hay eco
  long duracion = pulseIn(echoPin, HIGH, 30000);
  float distancia = duracion * 0.034 / 2.0;

  Serial.print("4:");
  Serial.print(angulo);
  Serial.print(":");
  Serial.println(distancia, 2);
}