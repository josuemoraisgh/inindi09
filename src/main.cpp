#include <Arduino.h>
#include <math.h>
#include "digitalFilter.h"
#define NUMTHREADS 2 // Deve-se sempre declarar o NUMTHREADS antes de "inindThread.h"
#include "inindThread.h"

// Pinos para o PWM
#define pinLED1 13 // Configura o pino de Saida do LED
#define PINPOS 6
#define PINNEG 5
// Valores de Funcionamento
#define VMAX 5.0
#define NPONTOS (20 - 1) // QUANTIDADE DE PONTOS - 1
const int PONTOS[] = {0, 2 * NPONTOS, 2 * NPONTOS, 4 * NPONTOS};
#define Freq 15.0
#define T (1.0 / Freq)
#define SIN_INTERVAL ((unsigned int)((1000000.0 / (Freq * PONTOS[3]))))

int count = 0;
float AMPLITUDE = 2.0;            // Amplitude é de 0 a Vmax
unsigned long previousMicros = 0; // will store last time LED was updated
// Vetor de 256 posições contendo às amplitudes do PWM variando de forma senoidal
const float seno[] = {0.0,
                      0.07845909572784494,
                      0.15643446504023087,
                      0.2334453638559054,
                      0.3090169943749474,
                      0.3826834323650898,
                      0.45399049973954675,
                      0.5224985647159488,
                      0.5877852522924731,
                      0.6494480483301837,
                      0.7071067811865476,
                      0.7604059656000309,
                      0.8090169943749475,
                      0.8526401643540922,
                      0.8910065241883678,
                      0.9238795325112867,
                      0.9510565162951535,
                      0.9723699203976766,
                      0.9876883405951378,
                      0.996917333733128,
                      1.0};
void ledFunc() // Faz a leitura do sinal Analogico
{
  digitalWrite(pinLED1, !digitalRead(pinLED1));
}
/*
void sinFunc()
{
  const int8_t aux = count % NPONTOS;
  switch (aux)
  {
  case 0:
    analogWrite(PINPOS, (int)(255 * (AMPLITUDE / VMAX) * seno[count]));
    break;
  case 1:
    analogWrite(PINPOS, (int)(255 * (AMPLITUDE / VMAX) * seno[PONTOS[aux] - count]));
    break;
  case 2:
    analogWrite(PINNEG, (int)(255 * (AMPLITUDE / VMAX) * seno[count - PONTOS[aux]]));
    break;
  case 3:
    analogWrite(PINNEG, (int)(255 * (AMPLITUDE / VMAX) * seno[PONTOS[aux] - count]));
    break;
  }
  if (count >= PONTOS[3])
    count = 1;
  else
    count++;
}
*/
void sinFunc()
{
  const int8_t aux = count % NPONTOS;
  analogWrite(aux <= 1 ? PINPOS : PINNEG, (int)(255 * (AMPLITUDE / VMAX) * seno[abs(PONTOS[aux] - count)]));
  count = (count >= PONTOS[3]) ? 1 :count+1;
}

void setup()
{
  Serial.begin(19200);
  pinMode(PINPOS, OUTPUT);
  pinMode(PINNEG, OUTPUT);
  pinMode(pinLED1, OUTPUT); // Habilita interrupções globais
  analogWrite(PINNEG, 0);
  analogWrite(PINPOS, 0);
  threadSetup(ledFunc, 2000); // parametros:funcão,intervalo,funcão,intervalo,...
}

void loop()
{
  unsigned long currentMicros = micros();
  if (((currentMicros - previousMicros >= 0) && (currentMicros - previousMicros >= SIN_INTERVAL)) ||
      ((currentMicros - previousMicros < 0) && (currentMicros - 0 + 16383 - previousMicros >= SIN_INTERVAL)))
  {
    previousMicros = currentMicros;
    sinFunc();
  }
}
