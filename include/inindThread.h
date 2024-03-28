// O timer0 é utilizado pelo Arduino para funções como delay(), millis() e micros(). Então não se deve utilizar esse timer para evitar comprometer essas funções.
// O timer1 no Arduino UNO esse é o timer utilizado pela biblioteca de controle de servos. Caso você não esteja utilizando essa biblioteca, esse timer está livre para ser utilizado para outros propósitos. No Arduino Mega esse timer só será utilizado para controlar os servos se você estiver usando mais de 12 servos. (timer utilizado no projeto)
// O timer2 é utilizado pela função tone(). Então se você não precisar da função tone() esse timer está livre para outras aplicações
#include <Arduino.h>
#include <TimerOne.h>
#include <stdarg.h>

#ifndef NUMTHREADS
#define NUMTHREADS 1
#endif
struct threadStruct {
  void (*func)(); //Função executada pela thread
  unsigned long interval;//Instante atual da thread
  unsigned long time; //Altera o tempo da thread
} thread[NUMTHREADS];

void threads() {
  for (int i = 0; i < NUMTHREADS; i++) {
    if (thread[i].time >= thread[i].interval) {
      thread[i].func();
      thread[i].time = 0;
    }
    else thread[i].time++;
  }
}

void threadSetup(void (*callback)(), int threadInterval, ...) {
  va_list args;

  thread[0].time = 0;
  thread[0].func = callback;
  thread[0].interval = threadInterval;

  va_start(args, threadInterval);
  for (int i = 1; i < NUMTHREADS; i++) {
    thread[i].time = 0;
    thread[i].func = va_arg(args, void (*)());
    thread[i].interval = va_arg(args, int);
  }
  va_end(args);
  
  Timer1.initialize(1000); // Tempo em microsegundos da base das threads
  Timer1.attachInterrupt(threads);  
}