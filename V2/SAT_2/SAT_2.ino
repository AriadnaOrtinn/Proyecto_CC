//---------- SENSOR DE TEMPERATURA Y HUMEDAD ----------
#include <DHT.h>
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

//---------- SERVOMOTOR ----------
#include <Servo.h>
Servo miServo;
const int pinServo = 9;
int anguloActual = 0;
int direccionSweep = 1;
int anguloFijo = 0;
bool sweep = true;

//---------- RADAR ----------
const int trigPin = 6;
const int echoPin = 7;

//---------- LED ----------
int led = 13;

//---------- ENVÍO DE DATOS ----------
#include <SoftwareSerial.h>
SoftwareSerial mySerial(10, 11);
#include <string.h>
#include <stdbool.h>
bool enviando = true;
bool medias = true;
float listaTemp[10];
int contadorTemp = 0;
float listaHum[10];
int contadorHum = 0;


//---------- CONTROL DE TIEMPO ----------
unsigned long T_UltimoEnvio = 0;
unsigned long IntervaloEnvio = 3000;

unsigned long T_UltimoHeartbeat = 0;
const long IntervaloHeartbeat = 3000;

unsigned long T_UltimoEncendidoLED = 0;
bool EncendidoLED = false;

unsigned long T_UltimoRadar = 0;
unsigned long IntervaloRadar = 500;

unsigned long T_UltimoServo = 0;
unsigned long IntervaloServo = 30;


///////////////////////////////////////////////////
///////////////////////////////////////////////////
///////////////////////////////////////////////////

//---------- SETUP DEL SAT ----------
void setup() {
   Serial.begin(9600);
   mySerial.begin(9600);
   Serial.setTimeout(50);
   mySerial.setTimeout(50);   

   dht.begin();

   pinMode(led, OUTPUT);

   miServo.attach(pinServo);
   anguloActual = 0;
   miServo.write(anguloActual);

   pinMode(trigPin, OUTPUT);
   pinMode(echoPin, INPUT);
}

///////////////////////////////////////////////////
///////////////////////////////////////////////////
///////////////////////////////////////////////////

//---------- PROGRAMA DEL SAT EN BUCLE ----------
void loop(){
   unsigned long T_Actual = millis();

   //----- CONTROL DEL LED -----
   if((T_Actual - T_UltimoEncendidoLED >= 100) && (EncendidoLED == true)){
      digitalWrite(led, LOW);
      EncendidoLED = false;
   }
   
   //----- RECIBO DE DATOS -----
   if(mySerial.available()){
      String mensaje = mySerial.readStringUntil('\n');
      mensaje.trim();

      int posicion = mensaje.indexOf(":");
      int codigo = mensaje.substring(0, posicion).toInt();

      if(codigo == 1){ // MODIFICAR PERIODO --> CODE 1
         IntervaloEnvio = modificarIntervalo(mensaje);
      }
      else if(codigo == 2){ // PARAR/REANUDAR --> CODE 2
         enviando = pararReanudar(mensaje);
      }
      else if(codigo == 3){ //ENVIAR MEDIA T&H --> CODE 3
         medias = mediaTempHum(mensaje);
      }
      else if(codigo == 4){ // FIJAR ANGULO --> CODE 4
         anguloFijo = fijarAngulo(mensaje);
      }
   }

   //----- ENVÍO DE DATOS -----
   if(T_Actual - T_UltimoHeartbeat >= IntervaloHeartbeat){ // HEARTBEAT --> CODE 0
      T_UltimoHeartbeat = T_Actual;
      heartbeat();
   }

   if(T_Actual - T_UltimoEnvio >= IntervaloEnvio){ // T&H --> CODE 1 // ERROR DHT11 --> CODE 2 // MEDIA T&H --> CODE 3
      T_UltimoEnvio = T_Actual;
      float t = dht.readTemperature();
      float h = dht.readHumidity();

      if(!isnan(t)){
         if(contadorTemp < 10){
            listaTemp[contadorTemp] = t;
            contadorTemp++;
         }
         else{
            for(int i = 0; i < 9; i++){
               listaTemp[i] = listaTemp[i+1];
            }
            listaTemp[9] = t;
         }
      }
      if(!isnan(h)){
         if(contadorHum < 10){
            listaHum[contadorHum] = h;
            contadorHum++;
         }
         else{
            for(int i = 0; i < 9; i++){
               listaHum[i] = listaHum[i+1];
            }
            listaHum[9] = h;
         }
      }
      
      if(enviando == true){
         enviarTempHum(T_Actual, t, h);
      }
      if((medias == true) && (enviando == true)){
         enviarMediaTempHum(listaTemp, contadorTemp, listaHum, contadorHum);
      }
   }

   if(sweep == true){ // ANGULO DIST (BARRIDO) --> CODE 4
      if(T_Actual - T_UltimoServo >= 30){
         T_UltimoServo = T_Actual;

         anguloActual += direccionSweep;

         if(anguloActual >= 180){
            anguloActual = 180;
            direccionSweep = -1;
         }

         else if(anguloActual <= 0){
            anguloActual = 0;
            direccionSweep = 1;
         }

         miServo.write(anguloActual);
      }

      if(T_Actual - T_UltimoRadar >= IntervaloRadar){
         T_UltimoRadar = T_Actual;
         enviarAngDist(anguloActual);
      }
   }
   else if(sweep == false){ // ANGULO DIST (FIJO) --> CODE 4
      miServo.attach(pinServo);
      miServo.write(anguloFijo);

      if(T_Actual - T_UltimoRadar >= IntervaloRadar){
         T_UltimoRadar = T_Actual;
         enviarAngDist(anguloFijo);
      }
   }
}

///////////////////////////////////////////////////
///////////////////////////////////////////////////
///////////////////////////////////////////////////

//---------- FUNCIONES DE DATOS DE ENVÍO ----------
// HEARTBEAT --> CODE 0
void heartbeat(){ 
   miServo.detach();
   mySerial.println("0");
   miServo.attach(pinServo);
}

// ENVIAR T&H --> CODE 1
// ENVIAR ERROR DHT11 --> CODE 2
void enviarTempHum(unsigned long T_Actual, float t, float h){ 
   miServo.detach();
   
   if(isnan(h) && isnan(t)){
      mySerial.println("2:0");
   }
   else if(isnan(t)){
      mySerial.println("2:T");
   }
   else if(isnan(h)){
      mySerial.println("2:H");
   }

   else{
      digitalWrite(led, HIGH);
      T_UltimoEncendidoLED = T_Actual;
      EncendidoLED = true;

      if((h > 80) && (t > 30)){
         mySerial.print("1:");
         mySerial.print("T:");
         mySerial.print(t, 2);
         mySerial.print(":H:");
         mySerial.println(h, 2);     
      }
      else if(t > 30){
         mySerial.print("1:");
         mySerial.print("T:");
         mySerial.print(t, 2);
         mySerial.print(":h:");
         mySerial.println(h, 2);
         }
      else if(h > 80){
         mySerial.print("1:");
         mySerial.print("t:");
         mySerial.print(t, 2);
         mySerial.print(":H:");
         mySerial.println(h, 2);
         }
      else{
         mySerial.print("1:");
         mySerial.print("t:");
         mySerial.print(t, 2);
         mySerial.print(":h:");
         mySerial.println(h, 2);
      }
   }  
   miServo.attach(pinServo);
}

// ENVIAR MEDIA T&H --> CODE 3
void enviarMediaTempHum(float listaTemp[], int contadorTemp, float listaHum[], int contadorHum) {
  float mediaTemp = -1;
  float mediaHum = -1;

  if (contadorTemp > 0) {
    float sumaTemp = 0;
    for (int i = 0; i < contadorTemp; i++) {
      sumaTemp += listaTemp[i];
    }
    mediaTemp = sumaTemp / contadorTemp;
  }

  if (contadorHum > 0) {
    float sumaHum = 0;
    for (int i = 0; i < contadorHum; i++) {
      sumaHum += listaHum[i];
    }
    mediaHum = sumaHum / contadorHum;
  }

  miServo.detach();
  mySerial.print("3:");
  mySerial.print(mediaTemp, 2);
  mySerial.print(":");
  mySerial.println(mediaHum, 2);
  miServo.attach(pinServo);
}

// ENVIAR ANGULO DIST --> CODE 4
void enviarAngDist(int angulo){
   digitalWrite(trigPin, LOW);
   delayMicroseconds(2);
   digitalWrite(trigPin, HIGH);
   delayMicroseconds(10);
   digitalWrite(trigPin, LOW);

   long duracion = pulseIn(echoPin, HIGH, 30000);

   float distancia = duracion * 0.034 / 2.0;

   miServo.detach();
   mySerial.print("4:");
   mySerial.print(angulo);
   mySerial.print(":");
   mySerial.println(distancia, 2);
   miServo.attach(pinServo);
}

///////////////////////////////////////////////////
///////////////////////////////////////////////////
///////////////////////////////////////////////////

//---------- FUNCIONES DE RECIBO DE DATOS ----------

// MODIFICAR PERIODO --> CODE 1
float modificarIntervalo(String mensaje){
   int posicion = mensaje.indexOf(":");
   float nuevoIntervalo = mensaje.substring(posicion + 1).toFloat() * 1000;
   return nuevoIntervalo;
}

// PARAR/REANUDAR --> CODE 2
bool pararReanudar(String mensaje){
   int posicion = mensaje.indexOf(":");
   int nuevoEstado = mensaje.substring(posicion + 1).toInt();

   if(nuevoEstado == 0){
      return false;
   }
   else if(nuevoEstado == 1){
      return true;
   }
}

// PERMISO CALCULAR MEDIA --> CODE 3
bool mediaTempHum(String mensaje){
   int posicion = mensaje.indexOf(":");
   int nuevoEstado = mensaje.substring(posicion + 1).toInt();

   if(nuevoEstado == 0){
      return false;
   }
   if(nuevoEstado == 1){
      return true;
   }
}

// FIJAR ANGULO --> CODE 4
int fijarAngulo(String mensaje){
   int posicion = mensaje.indexOf(":");
   int anguloRecibido = mensaje.substring(posicion + 1).toInt();

   if(anguloRecibido == -1){
      sweep = true;
   }
   else{
      sweep = false;
      miServo.attach(pinServo);
      miServo.write(anguloRecibido);
   }

   return anguloRecibido;
}
