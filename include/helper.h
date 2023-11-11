#ifndef __HELPER__
#define __HELPER__

#include <math.h>

// #ifdef _WIN32
#include <windows.h>

typedef struct timer_s
{
  LARGE_INTEGER frequency;
  LARGE_INTEGER start;
} timer;

void timer_start(timer *timer);
double timer_get_time(timer *timer);
double timer_stop(timer *timer);

// #endif

#endif // __HELPER

#ifdef __HELPER_IMPLEMENTATION__

// #ifdef _WIN32

void timer_start(timer *timer)
{
  QueryPerformanceFrequency(&(timer->frequency));
  QueryPerformanceCounter(&(timer->start));
  return;
}

double timer_get_time(timer *timer)
{
  double interval;
  LARGE_INTEGER end;
  QueryPerformanceCounter(&end);
  interval = (double)(end.QuadPart - timer->start.QuadPart) / timer->frequency.QuadPart;

  return interval;
}

double timer_stop(timer *timer)
{
  double interval = timer_get_time(timer);

  timer->frequency.QuadPart = 0;
  timer->start.QuadPart = 0;

  return interval;
}

// #endif

#endif // __HELPER_IMPLEMENTATION__