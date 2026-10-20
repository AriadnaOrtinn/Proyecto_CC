#include <SoftwareSerial.h>

//PINES DE INTERCAMBIO DE DATOS
SoftwareSerial mySerial(10, 11); // RX, TX (azul, naranja)

//LED
int led = 13;

//CONTROL DE TIEMPO
unsigned long tiempoUltimoMensaje = 0;
const long tiempoLimiteFallo = 4000; //Tolerancia antes de anunciar el fallo de 1 segundo (recibe cada 3, deja uno de margen antes de alertar)
unsigned long tiempoEncendido = 0;
bool ledEncendido = false;

//SETUP DEL SATÉLITE
void setup() {
   Serial.begin(9600);
   mySerial.begin(9600);
   mySerial.setTimeout(50);   // Reduce el bloqueo de readString() de 1000ms a 50ms
   pinMode (led,OUTPUT);
}

//PROGRAMA DEL SATÉLITE
void loop() {
   unsigned long tiempoActual = millis(); //Actualizamos de forma constante el tiempo actual.

   //Apagar el LED si está encendido y han pasado 100ms. Debe ir primero en el código para apagarlo si estaba encendido.
   if (ledEncendido == true && (tiempoActual - tiempoEncendido >= 100)){
      digitalWrite(led, LOW);
      ledEncendido = false;
   }

   //Mirar si hay información disponible cada 3 segundos
   if (mySerial.available()) {
      String data = mySerial.readString(); //Leemos lo que llega
      Serial.print(data); //Ponemos lo que he leído

      tiempoActual = millis(); //Refrescamos el tiempo tras la lectura (readString() bloquea hasta 50ms)

      digitalWrite(led, HIGH); //Encendemos LED
      tiempoEncendido = tiempoActual; //Mismo instante de referencia para el LED
      ledEncendido = true;

      tiempoUltimoMensaje = tiempoActual; //Mismo instante de referencia para el último mensaje
   }

   if (tiempoUltimoMensaje != 0 && (tiempoActual - tiempoUltimoMensaje > tiempoLimiteFallo)) { //Si no hay info en 4 segundos (debería haber cada 3):
      Serial.println ("ALERTA! Fallo en la comunicación.");
      tiempoUltimoMensaje = tiempoActual; //Evita el bucle infinito.
   }

   if (Serial.available()) { //Mirar si hay información del PYTHON para enviar al satélite: parar o retomar el envío de datos.
      String comando = Serial.readStringUntil('\n');
      comando.trim ();
      mySerial.println(comando);
   }
   
}