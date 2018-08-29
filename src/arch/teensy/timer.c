#include <kinetis.h>
#include <string.h>
#include <stdlib.h>
#include "../include/timer.h"

// We will use FlexTimer Module (FTM) Timer One
#define NUM_TIMERS 1

typedef struct Timer {
  int timer_num;
  uint16_t duration_ms;
  TimerFn fn;
  void* args;
} Timer;

static Timer timers[NUM_TIMERS];

// initializes the structures for the timer
void Timers_init(void) {
  memset(timers, 0, sizeof(timers));
  for(int i=0;i<NUM_TIMERS;++i){
    timers[i].timer_num=i;
  }
}

// creates a timer that has a duration of ms milliseconds
// bound to the device
// each duration_ms the function timer_fn will be called
// with arguments timer_args

struct Timer* Timer_create(char* device,
        uint16_t duration_ms,
        TimerFn timer_fn,
        void* timer_args) {
  
  Timer* timer=(Timer*)calloc(sizeof(Timer), 1);
  if(!strcmp(device, "timer_0"))
    timer=timers;
  else
    return 0;
  timer->duration_ms=duration_ms;
  timer->timer_num=0;
  timer->fn=timer_fn;
  timer->args=timer_args;
  return timer;
}

// stops and destroyes a timer
void Timer_destroy(struct Timer* timer) {
  Timer_stop(timer);
  __disable_irq();
  int timer_num=timer->timer_num;
  memset(timer, 0, sizeof(Timer));
  timer->timer_num=timer_num;
  __enable_irq();
}

void _timer0_start(struct Timer* timer) {
    FTM1_SC=0;
    // Timer Overflow Interrupt Enable
    //FTM1_SC |= (1<<TOIE);
    
}

// starts a timer
void Timer_start(struct Timer* timer) {
  __disable_irq();
  if(timer->timer_num==0)
    _timer0_start(timer);
  __enable_irq();
}

// stops a timer
void Timer_stop(struct Timer* timer);
/*
ISR(TIMER1_COMPA_vect) {
  // resets the counter
  FTM1_CNT=0;
  uint32_t sc = FTM1_SC;
  // Check and resets FTM1_SC if TOF is True
  // 0x80 -> TOF (Time Overflow Flag) 
  if(sc & 0x80)
    FTM1_SC = sc & 0x7F;
  // execute ISR function with args
  if(timers[0].fn)
    (*timers[0].fn)(timers[0].args);
}
*/