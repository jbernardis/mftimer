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
Button dirButton = Button(6, LOW);
Button encButton = Button(7, LOW);
Encoder setTime(4, 5);
const int speakerPin = 9;

void updateDisplay(void);
void getButtons(void);
void readEncoder(void);
void startBeeping(void);
void beep(void);
void stopBeeping(void);

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

int beepTicks;
boolean triggered = false;
int direction = 1;
boolean running = false;
                                                                                                              
int nLaps = 0;
long lapTime[4];

long oldPosition  = 0;


void setup(void) {
	display = new Display(0x20);
	direction = 1;
	timer.SetDirection(direction);
	display->ShowDirection(direction);
	display->ShowMenu(false, true);
	startButton.setDebounceDelay(10);
	lapButton.setDebounceDelay(10);
	dirButton.setDebounceDelay(10);
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
			startBeeping();
	}
}

void getButtons(void) {
	startButton.listen();
	lapButton.listen();
	dirButton.listen();
	encButton.listen();
	
	if (startButton.onPress()) {
		if (timer.IsRunning()) {
			timer.Stop();
			oldPosition = timer.GetValue() * ticksPerClick;
			setTime.write(oldPosition);
		}
		else {
			display->ShowDirection(direction);
			timer.Start();
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
			nLaps = 0;
			display->ShowLaps(lapTime, nLaps);
			oldPosition = 0;
			setTime.write(0);
		}
		display->ShowMenu(timer.IsRunning(), nLaps <= 3);
	}

	if (dirButton.onPress() && !timer.IsRunning()) {
		direction = 0 - direction;
		timer.SetDirection(direction);
		display->ShowDirection(direction);
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
			timer.SetValue(newEnc/ticksPerClick);
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
	}
}

void stopBeeping() {
	noTone(speakerPin);
	triggered = false;
	stimer.disable(beepTimer);
}

