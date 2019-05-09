#include "Arduino.h"

// Motor
#include <DCMotor.h>

// FreeRTOS
#include <Arduino_FreeRTOS.h>
#include <SimpleFreeRTOS.h>

// Buzzer
#include <pitches.h>
#include <toneDelay.h>

// IR Receiver
#include <IRremote.h>
#include <IRremoteRemoteControlCodes.h>

// Ping Sensor
#include <Ping.h>

// Servo
#include <Servo.h>

// Cambia apariciones de bluetooth por Serial1
#define bluetooth Serial1

// Ping Sensor
PingSensor ping(A1); // S1 input

// Motores
DCMotor motor0(M0_EN, M0_D0, M0_D1);
DCMotor motor1(M1_EN, M1_D0, M1_D1);

// IR receiver
IRrecv irReceiver(A0);

// Servo
Servo servo;

//int op = 0;

// Semaforo binario para sincronizar patada
semaforo semBin;

// Semaforo binario para hacer sonar el Buzzer
semaforo semBuz;

// actualizado por la tarea ecoica
long eco_measure = 0;

// nota que utiliza el buzzer
int nota = NOTE_FS5;

// direccion de movimiento del servo
int dir_a = 180;
int dir_b = 0;

/**
 * Recibe comandos por bluetooth.
 * Duino Joy: https://play.google.com/store/apps/details?id=com.lekpkd.duinojoy
 */
int angle = 0;
int strength = 0;
int command = 0;
#define DEBUG_tarea_comandos_duinojoy 1
tarea(tarea_comandos_duinojoy)
{
  int i;
  int comma;
  char c;
  char buf[25];
  
  Serial.println("Comandos:");
  while (true) {    
    if (bluetooth.available() > 0) {
      i = 0;
      comma = 0;
      while(bluetooth.available() > 0) {
        c = bluetooth.read();
        // fin de comando
        if (c == ')') {
          if (comma == 1) {
            buf[i] = '\0';
            strength = atoi(buf);
          } else {
            if (i == 1) {
              #if DEBUG_tarea_comandos_duinojoy == 1
              Serial.println(buf[0]);
              #endif
              if (buf[0] == 'a') {
                command = buf[0];
                liberarSemaforo(semBin);
              }
              if (buf[0] == 'b') {
                command = buf[0];
                liberarSemaforo(semBin);
              }
              if (buf[0] == 'y') {
                nota = NOTE_A7;
                liberarSemaforo(semBuz);                
              }
              if (buf[0] == 'z') {
                // cambia dirección del pateo
                dir_a = (dir_a == 180) ? 0 : 180;
                dir_b = (dir_b == 0) ? 180 : 0;
              }              
            }            
          }
          break;
        }
        else if (c == ',') {
          buf[i] = '\0';
          angle = atoi(buf);          
          i = 0;
          comma = 1;
        }
        else {
          buf[i++] = c;      
        }
      }

      #if DEBUG_tarea_comandos_duinojoy == 1
      Serial.print(angle);
      Serial.print(" - ");
      Serial.print(strength);
      Serial.print(" - ");
      Serial.println(command);
      #endif
    } 

    esperarPeriodo();
  }
}

#define DBG_TASK_MTR 0
tarea(tarea_motor)
{
  motor0.setClockwise(false);
  motor1.setClockwise(false);

  int a = 0;
  int s = 0;

  while(true)
  {
    if (angle < 180) {
      if (angle < 80) {
        motor0.setSpeed(strength * (angle / 90.0));
        motor1.setSpeed(strength);        
      }
      else if (angle > 100) {        
        motor0.setSpeed(strength);
        motor1.setSpeed(strength * ((180 - angle) / 90.0));
      }
      else {
        motor0.setSpeed(strength);
        motor1.setSpeed(strength);
      }
    }
    else if (angle > 180) {
      strength = strength * (-1);
      if (angle > 280) {
        motor0.setSpeed(strength * ((360 - angle) / 90.0));
        motor1.setSpeed(strength);        
      } 
      else if (angle < 260) {
        motor0.setSpeed(strength);
        motor1.setSpeed(strength * ((90 - (270 - angle)) / 90.0));  
      }
      else {
        motor0.setSpeed(strength);
        motor1.setSpeed(strength);
      }
    }
    
    esperarPeriodo();    
  }
}

tarea(tarea_sensor_ecoico)
{
  pinMode(A1, INPUT);
  
  while(true) {
    eco_measure = 0;
    
    //value = ping.measureCM(A1);
    pinMode(A1, OUTPUT);
    digitalWrite(A1, LOW);
    delayMicroseconds(2);
    digitalWrite(A1, HIGH);
    delayMicroseconds(5);

    pinMode(A1, INPUT);
    eco_measure = pulseIn(A1, HIGH) / 29 / 2;

    if ((eco_measure > 0) && (eco_measure < 7)) {
      nota = NOTE_F5;
      liberarSemaforo(semBuz);
    }
    
    esperarPeriodo();
  }
}

tarea(tarea_bateria) 
{
  while(true) {
    Serial.print("Bateria: ");
    Serial.println(analogRead(BATTERY));
    esperarPeriodo();
  }
}

/*
 * Gira un servo de recorrido continuo para "patear".
 */
tarea(tarea_patada) 
{
  servo.attach(A5);
  servo.write(90); // detiene el servo
  int dir;

  while (true) {
    tomarSemaforo(semBin); // bloqueado 
    servo.write((command == 'a') ? dir_a : dir_b); // gira a maxima velocidad
    esperar(500); // espera a que realice un giro
    servo.write(90);    
  }
}

/*
 * Patada con un servo de 180 grados
 */
tarea(tarea_patada_180) 
{
  servo.attach(A5);
  servo.write(0);
  
  while(true)
  {
    // preparar para patear
    servo.write(180);
    esperar(1000);
    servo.write(60);
    esperar(1000);

    // espera la orden
    tomarSemaforo(semBin);

    // patear
    servo.write(0);
    esperar(1000);
  }
}

tarea(tarea_buzzer)
{  
  while (true) {
    tomarSemaforo(semBuz);
    toneWithDelay(SPEAKER, nota, 750);    
  }
}

tarea(tarea_baliza_bluetooth) 
{
  while (true) {
    bluetooth.println("baliza bluetooth");
    esperarPeriodo();    
  }
}


void setup() {
  Serial.begin(9600);
  bluetooth.begin(9600);
  
  pinMode(LED_BUILTIN, OUTPUT);  
  
  semBin = creaSemaforoBinario();
  semBuz = creaSemaforoBinario();

  crearTareaPeriodica(tarea_sensor_ecoico,     2, 500);  
  crearTareaPeriodica(tarea_motor,             3, 100);
  crearTareaPeriodica(tarea_comandos_duinojoy, 5, 50);  
  crearTarea(tarea_patada, 4);
  crearTarea(tarea_buzzer, 1);
  
  iniciarPlanificador();
}

void loop() {
  // No hace nada
}
