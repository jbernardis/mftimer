#ifndef DISPLAY_H
#define DISPLAY_H
#include <LiquidTWI2.h>


class Display {
	public:
		Display(int);
		void DisplayTime(long);
		void DisplayAlert(boolean);
		void ShowLaps(long *, int);
		void ShowDirection(int);
		void ShowResolution(int);
		void ClearResolution(void);
		void ShowMenu(boolean, boolean);

	private:
		void lcdInit(boolean);
		void ShowTime(long, int, int);
		void ShowTime(long, int, int, int);
		int numberOfDigits;
		LiquidTWI2 *plcd;
		int direction;
		int timeDirChar;
		long clearResolutionTime;
};

#endif
