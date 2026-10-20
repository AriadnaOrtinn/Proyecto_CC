#include <SoftwareSerial.h>

SoftwareSerial mySerial(10, 11); // RX, TX
unsigned long tiempoAnterior = 0;
const unsigned long intervalo = 2000;

void setup() {
   Serial.begin(9600);
   mySerial.begin(9600);
   Serial.println("Test Satelite iniciado. Enviando PING...");
}

void loop() {
   unsigned long tiempoActual = millis();

   // Enviar PING cada 2 segundos
   if (tiempoActual - tiempoAnterior >= intervalo) {
      tiempoAnterior = tiempoActual;
      mySerial.println("PING");
      Serial.println("Enviado: PING -> Esperando respuesta...");
   }

   // Escuchar la respuesta (ACK) de la Estación de Tierra
   if (mySerial.available() > 0) {
      String respuesta = mySerial.readStringUntil('\n');
      respuesta.trim();
      
      if (respuesta == "ACK") {
         Serial.println("¡Recibido ACK! Comunicación bidireccional OK.");
      }
   }
}
