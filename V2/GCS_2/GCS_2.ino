//---------- ENVÍO DE DATOS ----------
#include <SoftwareSerial.h>
SoftwareSerial mySerial(10, 11);
#include <string.h>
#include <stdbool.h>

//---------- LED ----------
int led = 13;

//---------- CONTROL DE TIEMPO ----------
unsigned long T_UltimoHeartbeat = 0;
const long T_MaxHeartbeat = 4000;
bool comunicacion = true;
String mensaje_comunicacion = "0:1";

unsigned long T_UltimoEncendidoLED = 0;
bool EncendidoLED = false;

///////////////////////////////////////////////////
///////////////////////////////////////////////////
///////////////////////////////////////////////////

//---------- SETUP DE LA GCS ----------
void setup(){
  Serial.begin(9600);
  mySerial.begin(9600);
  // mySerial.setTimeout(50);
  pinMode(led, OUTPUT);
}

///////////////////////////////////////////////////
///////////////////////////////////////////////////
///////////////////////////////////////////////////

//---------- PROGRAMA DE LA GCS EN BUCLE ----------
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
    
    if(codigo == 0){ // ESTADO COMUNICACIÓN OK --> CODE 0:1
      T_UltimoHeartbeat = T_Actual;
      if(comunicacion == false){
        comunicacion = true;
        mensaje_comunicacion = "0:1";
        Serial.println(mensaje_comunicacion);
      }
    }
    if(codigo == 1){ // RECIBIR T&H --> CODE 1
      Serial.println(mensaje);
      
      digitalWrite(led, HIGH);
      T_UltimoEncendidoLED = T_Actual;
      EncendidoLED = true;
    }
    if(codigo == 2){ // RECIBIR ERROR DHT11 --> CODE 2
      Serial.println(mensaje);
    }
    if(codigo == 3){ // RECIBIR MEDIA T&H --> CODE 3
      Serial.println(mensaje);
    }
    if(codigo == 4){ // RECIBIR ANGULO DISTANCIA --> CODE 4
      Serial.println(mensaje);
    }
  }

  //----- ENVÍO DE COMANDOS DESDE PYTHON AL SAT -----
  if(Serial.available()){
    String comando = Serial.readStringUntil('\n');
    comando.trim();
    mySerial.println(comando);
  }

  //----- COMPROBACIÓN DE COMUNICACIÓN CON HEARTBEAT --> TIMEOUT MÁXIMO  -----
  if(T_Actual - T_UltimoHeartbeat >= T_MaxHeartbeat){ // ESTADO COMUNICACIÓN NOT OK--> CODE 0:0
    if(comunicacion == true){
      comunicacion = false;
      mensaje_comunicacion = "0:0";
      Serial.println(mensaje_comunicacion);
    }
  }
}