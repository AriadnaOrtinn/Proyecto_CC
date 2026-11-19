const int pinTrig = 3;
const int pinEcho = 4;

unsigned long tiempoAnterior = 0;
const unsigned long intervaloLectura = 1000; // Leer cada medio segundo

void setup() {
  Serial.begin(9600);
  pinMode(pinTrig, OUTPUT);
  pinMode(pinEcho, INPUT);
  
  digitalWrite(pinTrig, LOW); // Aseguramos que empieza apagado
  Serial.println("Iniciando test de sensor ultrasonico...");
}

void loop() {
  unsigned long tiempoActual = millis();

  if (tiempoActual - tiempoAnterior >= intervaloLectura) {
    tiempoAnterior = tiempoActual;
    
    float distancia = medirDistancia();
    
    // Filtramos lecturas fuera de rango (el HC-SR04 mide bien entre 2 cm y 400 cm)
    if (distancia <= 0 || distancia > 400) {
      Serial.println("Fallo o fuera de rango");
    } else {
      Serial.print("Distancia: ");
      Serial.print(distancia);
      Serial.println(" cm");
    }
  }
}

float medirDistancia() {
  // 1. Limpiamos el pin Trig durante 2 microsegundos
  digitalWrite(pinTrig, LOW);
  delayMicroseconds(2);
  
  // 2. Enviamos un pulso ultrasónico de 10 microsegundos
  digitalWrite(pinTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinTrig, LOW);
  
  // 3. Medimos el tiempo que tarda en volver el rebote (timeout de 30ms para no bloquear)
  long duracion = pulseIn(pinEcho, HIGH, 30000); 
  
  // 4. Calculamos la distancia (ida y vuelta, por eso dividimos entre 2)
  float distanciaCm = (duracion * 0.0343) / 2.0;
  
  return distanciaCm;
}