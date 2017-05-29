#ifndef DISPLAY_H
#define DISPLAY_H
#include <LiquidTWI2.h>

#define MODE_STOPWATCH  0
#define MODE_COUNTDOWN  1
#define MODE_INTERVAL  2
#define MODE_METRONOME  3

class Display {
	public:
		Display(int);
		void DisplayTime(long);
		void DisplayAlert(boolean);
		void ShowLaps(long *, int);
		void ShowMode(int);
		void ShowResolution(int);
		void ClearResolution(void);
		void ClearMode(void);
		void ShowMenu(boolean, boolean);

	private:
		void lcdInit(boolean);
    void ShowTime(long, int, int);
    void ShowTime(long, int, int, int);
		int numberOfDigits;
		LiquidTWI2 *plcd;
		int direction;
		int mode;
		long clearResolutionTime;
		long clearModeTime;
};

#endif
