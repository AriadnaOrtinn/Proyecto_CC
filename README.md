# Proyecto CC // Sistema de Comunicación Satelital // Grupo 11📡🛰️
Este es el proyecto de Ciencias de la Computación del Grupo 11 formado por Simón Ferrándiz, Alba Segarra y Ariadna Ortin.
Consiste en un sistema de comunicación entre un satélite (SAT) y una estación de tierra (GCS, siglas de Ground Control Station) usando dos placas de Arduino (una por cada componente) y una interfaz para la estación de tierra desarrollada en Python.

<img width="1600" height="1183" alt="unnamed" src="https://github.com/user-attachments/assets/9a6e4b8e-1814-4459-8d81-a956321286d0" />

## Índice


## Caracterísitcas de la versión: 2.0


## Arquitectura del sistema


## Hardware usado
### SAT
- Placa Arduino UNO
- Sensor de temperatura y humedad DHT11
- Sensor de ultrasonidos HC-SR04
- Servomotor SG90
- LED
- Cables de protoboard o de tipo jumper
- Cable de 

### GCS
- Placa Arduino UNO
- LED
- Cables de protoboard o de tipo jumper


## Conexiones del hardware
### SAT
  Sensor de temperatura y humedad DHT11:
  - VCC --> 5V
  - GND --> GND
  - DATA --> Pin 2

  Sensor de ultrasonidos HC-SR04
  - VCC --> 5V
  - GND --> GND
  - TRIG --> Pin 6
  - ECHO --> Pin 7

  Servomotor SG90
  - VCC --> 5V
  - GND --> GND
  - SIGNAL --> Pin 3

  LED Indicador
  - Ánodo (Extremo largo, positivo +) --> Pin 13
  - Cátodo (Extremo corto, negativo -) --> GND

  Módulo de comunicación SoftwareSerial
  - RX --> Pin 10
  - TX --> Pin 11

### GCS
 LED Indicador
  - Ánodo (Extremo largo, positivo +) --> Pin 13
  - Cátodo (Extremo corto, negativo -) --> GND

  Módulo de comunicación SoftwareSerial
  - RX --> Pin 10
  - TX --> Pin 11


## Scripts y tests unitarios
### SAT
- SAT_2.ino

### GCS
- GCS_2.ino
- Interfaz_2.py

### Tests Unitarios
- UNIT_COMMS_SAT.ino
- UNIT_COMMS_GCS.ino
- UNIT_DHT11
- UNIT_LED
- UNIT_RADAR
- UNIT_RADAR_SWEEP
- UNIT_SERVO


## Protocolo de codificación de los mensajes
<img width="489" height="395" alt="ProtocoloCodificacionMensajesV2" src="https://github.com/user-attachments/assets/f9c00148-850c-447a-9f9f-44153f48d438" />


## Recorrido por las versiones documentado en vídeos:
- [Vídeo de la versión 1](https://youtu.be/EAymErKZG9A?si=buH1tQA3QK20ozV5)


## Uso de la inteligencia artifical (IA)
En este proyecto, se ha hecho un uso de la inteligencia artificial como herramienta para diversas tareas:
- Organizar ideas
- Establecer protocolos
- Estructurar y ordenar los códigos más largos
- Aprender extras que no aparecían de forma directa en la guía del proyecto


