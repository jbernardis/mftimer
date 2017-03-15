#ifndef TIMER_H
#define TIMER_H

class Timer {
	public:
		Timer(SimpleTimer *);
		void Start();
		void Stop();
		void Reset();
		void SetValue(long);
		void SetDirection(int);
		void IncrementValue(int);
		long GetValue();
		boolean IsTriggered();
		boolean WasStopped();
		boolean IsRunning();
		void sec10Timer();

	private:

		long displayValue;
		long startValue;
		long startMillis;
		long stopMillis;
		int direction;
		boolean isRunning;
		boolean isTriggered;
		boolean wasStopped;
		SimpleTimer *sTimer;
		int mainTimer;
};

#endif
