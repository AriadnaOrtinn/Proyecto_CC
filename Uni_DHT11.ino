// Definiciones necesarias
#include <DHT.h>
#define DHTPIN 2
#define DHTTYPE DHT11
int ROJO = 13;
int AMARILLO = 12;
int VERDE = 8;

DHT dht(DHTPIN, DHTTYPE);
void setup() {
   Serial.begin(9600);
   // Inicialización del sensor
   dht.begin();
   pinMode(ROJO, OUTPUT);
   pinMode(AMARILLO, OUTPUT);
   pinMode(VERDE, OUTPUT);
}
void loop() {
   delay(2000);
   float h = dht.readHumidity();
   float t = dht.readTemperature();
   if (isnan(h) || isnan(t))
      Serial.println("Error al leer el sensor DHT11");
   else {
      if ((h > 80) && (t > 27)){
        Serial.print("HUMEDAD ALARMANTE: ");
        Serial.print(h);
        Serial.print("%\t");
        Serial.print("TEMPERATURA ALARMANTE: ");
        Serial.print(t);
        Serial.println("°C");
        digitalWrite(ROJO, HIGH);
        digitalWrite(AMARILLO, LOW);
        digitalWrite(VERDE, LOW);


      }
      
      else if (t > 27){
        Serial.print("Humedad: ");
        Serial.print(h);
        Serial.print("%\t");
        Serial.print("TEMPERATURA ALARMANTE: ");
        Serial.print(t);
        Serial.println("°C");
        digitalWrite(ROJO, LOW);
        digitalWrite(AMARILLO, HIGH);
        digitalWrite(VERDE, LOW);
      }

      else if (h > 80){
        Serial.print("HUMEDAD ALARMANTE: ");
        Serial.print(h);
        Serial.print("%\t");
        Serial.print("Temperatura: ");
        Serial.print(t);
        Serial.println("°C");
        digitalWrite(ROJO, LOW);
        digitalWrite(AMARILLO, HIGH);
        digitalWrite(VERDE, LOW);
      }
      
      else{
      Serial.print("Humedad: ");
      Serial.print(h);
      Serial.print("%\t");
      Serial.print("Temperatura: ");
      Serial.print(t);
      Serial.println("°C");
      digitalWrite(ROJO, LOW);
      digitalWrite(AMARILLO, LOW);
      digitalWrite(VERDE, HIGH);
      }
   }
}