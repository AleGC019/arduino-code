/*
 * CÓDIGO EVITA-LÍNEAS (CON DEPURACIÓN / LOGS)
 * Versión para diagnosticar por qué no se mueve.
 */

// ... (El código de la SD comentado sigue aquí arriba) ...
/*
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#define PIN_SD_CS 5
*/

// --- Pines de los Sensores IR ---
const int pinSensorIzquierdo = 32;
const int pinSensorDerecho = 33;

// --- Pines del L298N (Motores) ---
const int pinENA = 25; // Motor Der
const int pinIN1 = 26;
const int pinIN2 = 27;
const int pinENB = 13; // Motor Izq
const int pinIN3 = 14;
const int pinIN4 = 12;

// --- Lógica de los Sensores ---
#define BLANCO 0 //
#define NEGRO 1  //

// --- Configuración de Velocidad (PWM en ESP32) ---
const int freqPWM = 25000;
const int canalPWM_A = 0;
const int canalPWM_B = 1;
const int resolucionPWM = 8;
const int velocidadBase = 175;
const int velocidadGiro = 170;

// ... (La función escribirLogSD() comentada va aquí) ...

void setup()
{
  Serial.begin(9600);
  Serial.println("--- INICIO DEL SISTEMA ---");
  Serial.println("Iniciando Carro Evita-Líneas (Modo Depuracion)");

  pinMode(pinSensorIzquierdo, INPUT);
  pinMode(pinSensorDerecho, INPUT);

  pinMode(pinIN1, OUTPUT);
  pinMode(pinIN2, OUTPUT);
  pinMode(pinIN3, OUTPUT);
  pinMode(pinIN4, OUTPUT);

  ledcSetup(canalPWM_A, freqPWM, resolucionPWM);
  ledcSetup(canalPWM_B, freqPWM, resolucionPWM);
  ledcAttachPin(pinENA, canalPWM_A);
  ledcAttachPin(pinENB, canalPWM_B);

  Serial.println("Setup de pines completado.");
  detenerse();
  delay(2000);
  Serial.println("Iniciando Bucle Principal...");

  // ... (El setup() de la SD comentado va aquí) ...
}

void loop()
{
  // 1. Leer el estado de los sensores
  int estadoIzquierdo = digitalRead(pinSensorIzquierdo);
  int estadoDerecho = digitalRead(pinSensorDerecho);

  // --- LOG DE SENSORES ---
  // Imprime el valor crudo que leen los sensores
  // Serial.print("Sensores [IZQ | DER]: [ "); // <-- LOG
  // Serial.print(estadoIzquierdo);            // <-- LOG
  // Serial.print(" | ");                      // <-- LOG
  // Serial.print(estadoDerecho);              // <-- LOG
  // Serial.print(" ]  ---  Decision: ");      // <-- LOG

  // 2. Tomar decisiones (Lógica "Evita-Líneas")

  if (estadoIzquierdo == BLANCO && estadoDerecho == BLANCO)
  {
    // Caso 1: Ambos en blanco.
    avanzar(velocidadBase, velocidadBase);
    // Serial.println("Avanzando"); // <-- LOG
  }
  else if (estadoIzquierdo == NEGRO && estadoDerecho == BLANCO)
  {
    // Caso 2: Sensor izquierdo ve negro - EVITAR girando a la DERECHA (apagar motor DERECHO)
    soloMotorIzquierdo(velocidadGiro);
    // Serial.println("Solo motor IZQUIERDO"); // <-- LOG
  }
  else if (estadoIzquierdo == BLANCO && estadoDerecho == NEGRO)
  {
    // Caso 3: Sensor derecho ve negro - EVITAR girando a la IZQUIERDA (apagar motor IZQUIERDO)
    soloMotorDerecho(velocidadGiro);
    // Serial.println("Solo motor DERECHO"); // <-- LOG
  }
  else
  {
    // Caso 4: (Ambos ven NEGRO)
    detenerse();
    // Serial.println("DETENIDO (Ambos ven negro)"); // <-- LOG
  }

  // Pequeño retardo para hacer el Monitor Serial legible// <-- RETARDO
}

// =============================================
// --- Funciones de Movimiento (L298N) ---
// (Estas funciones son idénticas a las de antes)
// =============================================

void detenerse()
{
  digitalWrite(pinIN1, LOW);
  digitalWrite(pinIN2, LOW);
  ledcWrite(canalPWM_A, 0);
  digitalWrite(pinIN3, LOW);
  digitalWrite(pinIN4, LOW);
  ledcWrite(canalPWM_B, 0);
}

void avanzar(int velIzquierda, int velDerecha)
{
  digitalWrite(pinIN1, HIGH);
  digitalWrite(pinIN2, LOW);
  ledcWrite(canalPWM_A, velDerecha);
  digitalWrite(pinIN3, HIGH);
  digitalWrite(pinIN4, LOW);
  ledcWrite(canalPWM_B, velIzquierda);
}

void soloMotorDerecho(int velocidadGiro)
{
  // Solo motor DERECHO encendido, izquierdo apagado
  digitalWrite(pinIN1, HIGH);
  digitalWrite(pinIN2, LOW);
  ledcWrite(canalPWM_A, velocidadGiro); // Motor derecho ON
  digitalWrite(pinIN3, LOW);
  digitalWrite(pinIN4, LOW);
  ledcWrite(canalPWM_B, 0); // Motor izquierdo OFF
}

void soloMotorIzquierdo(int velocidadGiro)
{
  // Solo motor IZQUIERDO encendido, derecho apagado
  digitalWrite(pinIN1, LOW);
  digitalWrite(pinIN2, LOW);
  ledcWrite(canalPWM_A, 0); // Motor derecho OFF
  digitalWrite(pinIN3, HIGH);
  digitalWrite(pinIN4, LOW);
  ledcWrite(canalPWM_B, velocidadGiro); // Motor izquierdo ON
}
