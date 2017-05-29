#include <Arduino.h>

#include "Display.h"
#include <LiquidTWI2.h>

const int numRows = 4;
const int numCols = 20;

const int LAP1 = 0;
const int LAP2 = 1;
const int LAP3 = 2;
const int LAP4 = 3;


byte lap1Char[8] =
{
    B00010,
    B00110,
    B10010,
    B10010,
    B10111,
    B10000,
    B10000,
    B11111
};

byte lap2Char[8] =
{
    B00110,
    B00001,
    B10010,
    B10100,
    B10111,
    B10000,
    B10000,
    B11111
};

byte lap3Char[8] =
{
    B00110,
    B00001,
    B10010,
    B10001,
    B10110,
    B10000,
    B10000,
    B11111
};

byte lap4Char[8] =
{
    B00101,
    B00101,
    B10111,
    B10001,
    B10001,
    B10000,
    B10000,
    B11111
};


Display::Display(int addr) {
	plcd = new LiquidTWI2(addr);
	lcdInit(true);
	direction = 1;
	mode = 0;
}

void Display::DisplayAlert(boolean onoff) {
}

void Display::DisplayTime(long tm) {
	ShowTime(tm, 0, 0);
}

void Display::ShowTime(long tm, int row, int col) {
  ShowTime(tm, row, col, -1);
}

void Display::ShowTime(long tm, int row, int col, int sChar) {
	long tenths, secs, mins;
	tenths = tm % 10;
	tm = tm / 10;

	secs = tm % 60;
	mins = tm / 60;

	plcd -> setCursor(row, col);
  if (sChar < 0)
    plcd -> print(" ");
  else
    plcd -> write((uint8_t) sChar);
    	
	if (mins >= 1000) 
		mins = mins % 1000;
        
	if (mins < 100) 
		plcd -> print(" ");
	if (mins < 10) 
		plcd -> print(" ");

	plcd -> print(mins);

	plcd -> print(":");
	if (secs < 10) 
		plcd -> print("0");

	plcd -> print(secs);
	plcd -> print(".");
	plcd -> print(tenths);

	if (clearResolutionTime > 0 && millis() > clearResolutionTime) {
		clearResolutionTime = 0;
		ClearResolution();
	}

	if (clearModeTime > 0 && millis() > clearModeTime) {
		clearModeTime = 0;
		ClearMode();
	}
}

void Display::ShowLaps(long *lt, int n) {
	int tenths, secs, mins, l;

	for (l=0; l<4; l++) {
		if (l >= n) {
			plcd->setCursor(10, l);
			plcd->print("         ");
		}
		else {
			ShowTime(*(lt+l), 10, l, LAP1 + l);
		}
	}
}

void Display::ShowMode(int md) {
	mode = md;
	plcd->setCursor(10, 2);
	if (mode == MODE_STOPWATCH) {
		direction = 1;
		plcd->print(" stopwatch");
	}
	else {
		direction = -1;
		if (mode == MODE_COUNTDOWN)
			plcd->print(" countdown");
		else if (mode == MODE_INTERVAL)
			plcd->print(" interval ");
		else
			plcd->print(" metronome");
	}
	clearModeTime = millis() + 2000;
}

void Display::ClearMode() {
	plcd->setCursor(10, 2);
	plcd->print("          ");
}

void Display::ShowResolution(int res) {
	plcd->setCursor(1, 1);
	if (res == 1)
		plcd->print("       *");
	else if (res == 10)
		plcd->print("    **  ");
	else if (res == 600)
		plcd->print("***     ");
	else {
		ClearResolution();
		return;
	}
	clearResolutionTime = millis() + 5000;
}

void Display::ClearResolution() {
	plcd->setCursor(1, 1);
	plcd->print("        ");
}

void Display::ShowMenu(boolean running, boolean longMenu) {
	plcd->setCursor(0, 3);
	if (running) {
    if (longMenu) {
      plcd->print("stop  lap           ");
    }
    else
      plcd->print("stop  lap ");
	}
	else {
		if (longMenu) {
			plcd->print("strt  rst     mode  ");
		}
		else
			plcd->print("strt  rst ");
	}
}

void Display::lcdInit(boolean init) {
	if (init) {
		plcd -> setMCPType(LTI_TYPE_MCP23008); 
		plcd -> begin(numCols, numRows);
		plcd -> setBacklight(HIGH);
		plcd -> createChar(LAP1, lap1Char);
		plcd -> createChar(LAP2, lap2Char);
		plcd -> createChar(LAP3, lap3Char);
		plcd -> createChar(LAP4, lap4Char);
	}
	plcd -> clear();
}

