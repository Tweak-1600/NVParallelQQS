#pragma once
#include <chrono>
using namespace std;
using namespace chrono;
class Stopwatch
{
private:
	using time_point = std::chrono::high_resolution_clock::time_point;
	time_point _start;
	time_point _end;
	bool _running;
	int64_t elapsed;
public:
	Stopwatch()
	{
		_start = _end = high_resolution_clock::now();
		_running = false;
		elapsed = 0;
	}
	inline void Start()
	{
		if (_running)
		{
			return;
		}
		_start = high_resolution_clock::now();
		_running = true;
	}
	inline void Stop()
	{
		if (_running)
		{
			_end = high_resolution_clock::now();
			elapsed += duration_cast<nanoseconds>(_end - _start).count();
		}
		_running = false;
	}
	inline void Zero()
	{
		if (_running) return;
		_running = false;
		_start = _end = high_resolution_clock::now();
		elapsed = 0;
	}
	inline void Restart()
	{
		Stop();
		Zero();
		Start();
	}
	inline void Reset()
	{
		Stop();
		Zero();
	}
	inline int64_t GetElapsedNanoseconds()
	{
		if (_running)
		{
			_end = high_resolution_clock::now();
			return (elapsed + duration_cast<nanoseconds>(_end - _start).count());
		}
		return elapsed;
	}
	inline int64_t GetElapsedTicks()
	{
		return GetElapsedNanoseconds() / 100;
	}
	inline int64_t GetElapsedMicroseconds()
	{
		return GetElapsedNanoseconds() / 1000;
	}
	inline int64_t GetElapsedMilliseconds()
	{
		return GetElapsedNanoseconds() / 1000000;
	}
	inline double GetElapsedSeconds()
	{
		return GetElapsedNanoseconds() / 1000000000.0;
	}
};

