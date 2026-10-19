#include <SoftwareSerial.h>

// PINES DE INTERCAMBIO DE DATOS
SoftwareSerial mySerial(10, 11); // RX, TX (azul, naranja)

// LED
int led = 13;

// CONTROL DE TIEMPO
unsigned long tiempoUltimoMensaje = 0;
const long tiempoLimiteFallo = 4000; // Tolerancia antes de anunciar el fallo de 1 segundo
unsigned long tiempoEncendido = 0;
bool ledEncendido = false;

// SETUP DE LA ESTACIÓN DE TIERRA
void setup() {
   Serial.begin(9600);
   Serial.println("Inicio de la recepción:");
   mySerial.begin(9600);
   mySerial.setTimeout(50); // Reduce el bloqueo de readString() de 1000ms a 50ms
   pinMode(led, OUTPUT);
}

// PROGRAMA DE LA ESTACIÓN DE TIERRA
void loop() {
   unsigned long tiempoActual = millis(); // Actualizamos de forma constante el tiempo actual.

   // Apagar el LED si está encendido y han pasado 100ms.
   if (ledEncendido == true && (tiempoActual - tiempoEncendido >= 100)) {
      digitalWrite(led, LOW);
      ledEncendido = false;
   }

   // Mirar si hay información disponible
   if (mySerial.available()) {
      String data = mySerial.readString(); // Leemos lo que llega
      Serial.print(data); // Ponemos lo que hemos leído

      tiempoActual = millis(); // Refrescamos el tiempo tras la lectura

      digitalWrite(led, HIGH); // Encendemos LED
      tiempoEncendido = tiempoActual; // Mismo instante de referencia para el LED
      ledEncendido = true;

      tiempoUltimoMensaje = tiempoActual; // Mismo instante de referencia para el último mensaje
   }

   // Control de fallos de comunicación
   if (tiempoUltimoMensaje == 0) {
      // Caso 1: Si no se ha recibido el primer mensaje al arrancar
      if (tiempoActual > tiempoLimiteFallo) {
         Serial.println("ALERTA! Fallo en la comunicación.");
         tiempoUltimoMensaje = tiempoActual; // Reinicia la espera antes de volver a alertar
      }
   }
   else if (tiempoActual - tiempoUltimoMensaje > tiempoLimiteFallo) {
      // Caso 2: Si dejan de recibirse mensajes transcurridos los 4 segundos
      Serial.println("ALERTA! Fallo en la comunicación.");
      tiempoUltimoMensaje = tiempoActual; // Reinicia la espera antes de volver a alertar
   }
}