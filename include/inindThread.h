// O timer0 é utilizado pelo Arduino para funções como delay(), millis() e micros(). Então não se deve utilizar esse timer para evitar comprometer essas funções.
// O timer1 no Arduino UNO esse é o timer utilizado pela biblioteca de controle de servos. Caso você não esteja utilizando essa biblioteca, esse timer está livre para ser utilizado para outros propósitos. No Arduino Mega esse timer só será utilizado para controlar os servos se você estiver usando mais de 12 servos. (timer utilizado no projeto)
// O timer2 é utilizado pela função tone(). Então se você não precisar da função tone() esse timer está livre para outras aplicações.
#include <Arduino.h>
#include <stdarg.h>
#include <TimerOne.h>

#define TIMER_INTERVAL_US 1000
#define TAMTHREAD 5
using CallbackFunc = void (*)(); // definição de tipo para nossa callback
struct threadStruct
{
  volatile CallbackFunc func[TAMTHREAD];
  volatile int interval[TAMTHREAD];
  volatile int time[TAMTHREAD];
};
threadStruct thread;
volatile int numThread;

void TimerHandler()
{
  for (int i = 0; i < numThread && i < TAMTHREAD; i++)
  {
    if (thread.time[i] >= thread.interval[i])
    {
      thread.func[i]();
      thread.time[i] = 0;
    }
    else
      thread.time[i]++;
  }
}

void threadSetup(CallbackFunc callback, int threadInterval, ...)
{
  numThread = 1;
  va_list args;
  va_start(args, threadInterval);

  while (va_arg(args, CallbackFunc))
  {
    va_arg(args, int);
    numThread++;
  }
  thread.time[0] = 0;
  thread.func[0] = callback;
  thread.interval[0] = threadInterval;

  va_start(args, threadInterval);
  for (int i = 1; i < numThread && i < TAMTHREAD; i++)
  {
    thread.time[i] = 0;
    thread.func[i] = va_arg(args, CallbackFunc);
    thread.interval[i] = va_arg(args, int);
  }
  va_end(args);
  Timer1.initialize(TIMER_INTERVAL_US); //The led will blink in a half second time interval
  Timer1.attachInterrupt(TimerHandler);
}