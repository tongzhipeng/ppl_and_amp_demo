#pragma once


#include <Windows.h>
class CStopWatch {
public:
	CStopWatch() {
		m_start.QuadPart = 0;
		m_stop.QuadPart = 0;
		QueryPerformanceFrequency(&m_frequency);
		m_totalRoundTime = 0.0;
		m_nRoundCount = 0;
	}
	void start() { QueryPerformanceCounter(&m_start); }
	void stop() { QueryPerformanceCounter(&m_stop); }
	double getElapsedTime() // in seconds
	{
		LARGE_INTEGER time;
		time.QuadPart = m_stop.QuadPart - m_start.QuadPart;
		return (double)time.QuadPart / (double)m_frequency.QuadPart;
	}

	void startRace() {
		m_nRoundCount = 0;
		m_totalRoundTime = 0.0;
	}
	void roundBegin() { start(); }
	void roundEnd() {
		stop();
		m_nRoundCount++;
		m_totalRoundTime += getElapsedTime();
	}

	double endRace() { return m_totalRoundTime; }

private:
	LARGE_INTEGER m_start;
	LARGE_INTEGER m_stop;
	LARGE_INTEGER m_frequency;

	double m_totalRoundTime;
	int m_nRoundCount;
};

