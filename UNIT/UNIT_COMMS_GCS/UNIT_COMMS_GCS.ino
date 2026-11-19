#include <SoftwareSerial.h>

SoftwareSerial mySerial(10, 11); // RX, TX

void setup() {
   Serial.begin(9600);
   mySerial.begin(9600);
   Serial.println("Test Estacion de Tierra iniciado. Esperando PING...");
}

void loop() {
   // Escuchar si llega el PING del satélite
   if (mySerial.available() > 0) {
      String comando = mySerial.readStringUntil('\n');
      comando.trim();
      
      if (comando == "PING") {
         Serial.println("Recibido: PING. Respondiendo con ACK...");
         
         // Enviar respuesta de vuelta
         mySerial.println("ACK");
      }
   }
}