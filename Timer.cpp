#include <Arduino.h>

#include <SimpleTimer.h>
#include "Timer.h"

static Timer *TimerHolder;

void sec10TimerGlobal() {
	TimerHolder -> sec10Timer();
}

Timer::Timer(SimpleTimer *st) {
	displayValue = 0;
	isRunning = false;
	wasStopped = false;
	direction = 1;
	stopMillis = 0;
	startMillis = 0;
	sTimer = st;
	TimerHolder = this;
	mainTimer = sTimer -> setInterval(100, sec10TimerGlobal);
	sTimer -> disable(mainTimer);
}

void Timer::Start() {
	// start the timer
	isRunning = true;
	isTriggered = false;
	if (wasStopped) {
		startMillis = startMillis + millis() - stopMillis;	
		wasStopped = false;
	}
	else {
		startMillis = millis();
		startValue = displayValue;		
	}
	sTimer -> enable(mainTimer);
}

void Timer::Stop() {
	// stop the timer}
	sTimer -> disable(mainTimer);
	stopMillis = millis();
	isRunning = false;
	isTriggered = false;
	wasStopped = true;
}

void Timer::Reset() {
	Stop();
	displayValue = 0;
	stopMillis = 0;
	startMillis = 0;
	wasStopped = false;
	isRunning = false;
	isTriggered = false;
}

void Timer::SetValue(long val) {
	if (!isRunning) 
		displayValue = val;
}

void Timer::SetDirection(int d) {
	if (!isRunning) {
		wasStopped = false;
		direction = d;
	}
}

boolean Timer::IsRunning() {
	return (isRunning);
}

boolean Timer::WasStopped() {
	return (wasStopped);
}

void Timer::IncrementValue(int incr) {
	if (! isRunning) {
		displayValue += incr;
		if (displayValue < 0)
			displayValue = 0;
	}
}

long Timer::GetValue() {
	return (displayValue);
}

boolean Timer::IsTriggered() {
	return isTriggered;
}

void Timer::sec10Timer() {
	long now = millis();
	long elapsed = (now - startMillis)/100;
	if (direction > 0) {
		displayValue = startValue + elapsed;
	}
	else {
		displayValue = startValue - elapsed;
		if (displayValue <= 0) {
			Stop();
			displayValue = 0;
			isTriggered = true;
		}
	}
}



