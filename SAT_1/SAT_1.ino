// LECTOR DE TEMPERATURA Y HUMEDAD
#include <DHT.h>
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// LED
int led = 13;

// ENVÍO DE DATOS
#include <SoftwareSerial.h>
SoftwareSerial mySerial(10, 11);

// CONTROL DE TIEMPO
unsigned long tiempoAnterior = 0;
const long intervalo = 3000;
unsigned long tiempoEncendido = 0;
bool ledEncendido = false;

// SETUP DEL SATÉLITE
void setup() {
   Serial.begin(9600); // Inicializado para mensajes de depuración local
   mySerial.begin(9600);
   mySerial.println("Empezamos");
   dht.begin();
   pinMode(led, OUTPUT);
}

// PROGRAMA DEL SATÉLITE
void loop() {
   unsigned long tiempoActual = millis(); // Actualizamos de forma constante el tiempo actual.

   // Apagar el LED si está encendido y han pasado 100ms.
   if (ledEncendido == true && (tiempoActual - tiempoEncendido >= 100)) {
      digitalWrite(led, LOW);
      ledEncendido = false;
   }

   // Enviar datos cada 3 segundos
   if (tiempoActual - tiempoAnterior >= intervalo) { 
      tiempoAnterior = tiempoActual;

      float h = dht.readHumidity(); // Leemos humedad, guardamos en -> h
      float t = dht.readTemperature(); // Leemos temperatura, guardamos en -> t

      if (isnan(h) && isnan(t)) { // Error al leer ambos datos (Evaluado primero)
         Serial.println("ALERTA! Error al leer temperatura y humedad del sensor DHT11.");
         Serial.println("Nuevo intento de lectura en 3 segundos...");
         mySerial.println("Fallo"); // Avisa a la Estación de Tierra del fallo de lectura
      }
      else if (isnan(h)) { // Error al leer la humedad
         Serial.println("ALERTA! Error al leer la humedad del sensor DHT11.");
         Serial.println("Nuevo intento de lectura en 3 segundos...");
         mySerial.println("Fallo"); // Avisa a la Estación de Tierra del fallo de lectura
      }
      else if (isnan(t)) { // Error al leer la temperatura
         Serial.println("ALERTA! Error al leer la temperatura del sensor DHT11.");
         Serial.println("Nuevo intento de lectura en 3 segundos...");
         mySerial.println("Fallo"); // Avisa a la Estación de Tierra del fallo de lectura
      }
      else {
         digitalWrite(led, HIGH); // Encendemos el LED
         tiempoEncendido = tiempoActual;
         ledEncendido = true;

         if ((h > 70) && (t > 30)) { // Temperatura y humedad altas (T, H)
            mySerial.print("T:");
            mySerial.print(t, 2);
            mySerial.print(":H:");
            mySerial.println(h, 2);
         }
         else if (t > 30) { // Temperatura alta (T), humedad nominal (h)
            mySerial.print("T:");
            mySerial.print(t, 2);
            mySerial.print(":h:");
            mySerial.println(h, 2);
         }
         else if (h > 70) { // Temperatura nominal (t), humedad alta (H)
            mySerial.print("t:");
            mySerial.print(t, 2);
            mySerial.print(":H:");
            mySerial.println(h, 2);
         }
         else { // Temperatura y humedad nominales (t, h)
            mySerial.print("t:");
            mySerial.print(t, 2);
            mySerial.print(":h:");
            mySerial.println(h, 2);
         }
      }
   }
}