#include <Arduino.h>

#include <Encoder.h>
#include <Button.h>
#include <SimpleTimer.h>
#include <stdlib.h>

#include "Timer.h"
#include "Display.h"

Display *display;

SimpleTimer stimer;
Timer timer(&stimer);

Button startButton = Button(2, LOW);
Button lapButton = Button(3, LOW);
Button modeButton = Button(6, LOW);
Button encButton = Button(7, LOW);
Encoder setTime(4, 5);
const int speakerPin = 9;

void updateDisplay(void);
void clearLaps(void);
void getButtons(void);
void readEncoder(void);
void startBeeping(void);
void beep(void);
void stopBeeping(void);
void pip(void);

int encRes = 1;

const int ticksPerClick = 1;

int displayTimer;
int beepTimer;
int currentState;
int nextAction;
const int BEEPOFF = 0;
const int BEEP1 = 1;
const int BEEPPAUSE1 = 2;
const int BEEP2 = 3;
const int PIPON = 4;
const int PIPOFF = 5;

int beepTicks;
boolean triggered = false;
int direction = 1;
int mode = MODE_STOPWATCH;
boolean running = false;
                                                                                                              
int nLaps = 0;
long lapTime[4];

long oldPosition  = 0;
long timerSetValue = 0;


void setup(void) {
	display = new Display(0x20);
	direction = 1;
	mode = MODE_STOPWATCH;
	timer.SetDirection(direction);
	display->ShowMode(mode);
	display->ShowMenu(false, true);
	startButton.setDebounceDelay(10);
	lapButton.setDebounceDelay(10);
	modeButton.setDebounceDelay(10);
	encButton.setDebounceDelay(10);
	setTime.write(0);
	displayTimer = stimer.setInterval(100, updateDisplay);

	beepTimer = stimer.setInterval(150, beep);
	stimer.disable(beepTimer);
}

void loop(void) {
	readEncoder();
	getButtons();
	stimer.run();
	if (!triggered) {
		if (timer.IsTriggered()) 
			switch(mode) {
			case MODE_STOPWATCH:
				break;
			case MODE_COUNTDOWN:
				startBeeping();
				break;
			case MODE_INTERVAL:
        timer.Reset();
				pip();
				timer.SetValue(timerSetValue);
        display->ShowMenu(timer.IsRunning(), nLaps <= 3);
        break;
			case MODE_METRONOME:
        timer.Reset();
				pip();
				timer.SetValue(timerSetValue);
				timer.Start();
				break;
			}

	}
}

void getButtons(void) {
	startButton.listen();
	lapButton.listen();
	modeButton.listen();
	encButton.listen();
	
	if (startButton.onPress()) {
		if (timer.IsRunning()) {
			timer.Stop();
      switch (mode) {
      case MODE_STOPWATCH:
      case MODE_COUNTDOWN:
        timerSetValue = timer.GetValue();
        oldPosition = timerSetValue * ticksPerClick;
        setTime.write(oldPosition);
        break;
      case MODE_INTERVAL:
      case MODE_METRONOME:
        timer.SetValue(timerSetValue);
        oldPosition = timerSetValue * ticksPerClick;
        setTime.write(oldPosition);
        break;     
      }
		}
		else {
			//display->ShowMode(mode);
			switch (mode) {
			case MODE_STOPWATCH:
				timer.Start();
				break;
			case MODE_COUNTDOWN:
			case MODE_INTERVAL:
			case MODE_METRONOME:
				if (timerSetValue == 0)
					pip();
				else
					timer.Start();
			}

		}
		display->ShowMenu(timer.IsRunning(), nLaps <= 3);
	}
	if (lapButton.onPress()) {
		stopBeeping();
		if (timer.IsRunning()) {
			long lt = timer.GetValue();
			lapTime[3] = lapTime[2];
			lapTime[2] = lapTime[1];
			lapTime[1] = lapTime[0];
			lapTime[0] = lt;
			if (nLaps < 4)
				nLaps++;
			display->ShowLaps(lapTime, nLaps);
		}
		else {
			timer.Reset();
			clearLaps();
			oldPosition = 0;
      timerSetValue = 0;
			setTime.write(0);
		}
		display->ShowMenu(timer.IsRunning(), nLaps <= 3);
	}

	if (modeButton.onPress() && !timer.IsRunning()) {
    clearLaps();
		mode++;
		if (mode > MODE_METRONOME) {
			mode = 0;
			direction = 1;
		}
		else {
			direction = -1;
		}
		timer.SetDirection(direction);
		display->ShowMode(mode);
		display->ShowMenu(timer.IsRunning(), nLaps <= 3);
	}

	if (encButton.onPress() && !timer.IsRunning()) {
		if (encRes == 1)
			encRes = 10;
		else if (encRes == 10)
			encRes = 600;
		else
			encRes = 1;
		display->ShowResolution(encRes);
	}
    
}

void clearLaps(void) {
  nLaps = 0;
  display->ShowLaps(lapTime, nLaps);
}

void readEncoder(void) {
	long delta, newEnc;
	if (!(timer.IsRunning() || timer.WasStopped())) {
		long newPosition = setTime.read();

 		if (newPosition != oldPosition) {
			display->ShowResolution(encRes);
 			if (newPosition < 0) {
 				delta = -oldPosition;
 				newEnc = 0;
 			}
 			else {
				delta = newPosition - oldPosition;
				newEnc = oldPosition + delta*encRes;
				if (newEnc < 0) {
					newEnc = 0;
				}
			}
			setTime.write(newEnc);
			timerSetValue = newEnc / ticksPerClick;
			timer.SetValue(timerSetValue);
			oldPosition = newEnc;
		}
	}
}

void updateDisplay(void) {
	display->DisplayTime(timer.GetValue());
}

void startBeeping() {
	stimer.enable(beepTimer);
	tone(speakerPin, 4000);
	currentState = BEEPOFF;
	nextAction = 0;
	beepTicks = 0;
	triggered = true;
}

void beep() {
	beepTicks++;
	if (beepTicks < nextAction)
		return;

	switch (currentState) {
		case BEEPOFF:
			tone(speakerPin, 4000);
			currentState = BEEP1;
			nextAction = beepTicks + 3;
			break;
		case BEEP1:
			noTone(speakerPin);
			currentState = BEEPPAUSE1;
			nextAction = beepTicks + 3;
			break;
		case BEEPPAUSE1:
			tone(speakerPin, 4000);
			currentState = BEEP2;
			nextAction = beepTicks + 3;
			break;
		case BEEP2:
			noTone(speakerPin);
			currentState = BEEPOFF;
			nextAction = beepTicks + 6;
			break;
		case PIPOFF:
			tone(speakerPin, 5000);
			currentState = PIPON;
			nextAction = beepTicks + 1;
			break;
		case PIPON:
			noTone(speakerPin);
			triggered = false;
			stimer.disable(beepTimer);
			break;
	}
}

void stopBeeping() {
	noTone(speakerPin);
	triggered = false;
	stimer.disable(beepTimer);
}

void pip() {
	stimer.enable(beepTimer);
	tone(speakerPin, 5000);
	currentState = PIPOFF;
	nextAction = 0;
	beepTicks = 0;
	triggered = true;
}

