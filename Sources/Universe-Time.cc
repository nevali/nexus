#include <cstdio>

#include "Nexus/Universe.hh"

using namespace Nexus;

/* 1 usec = 1/1000000th of a second */
static const unsigned long sec_usec = 1000000;
/* 1 tick = 1/100th of a second */
static const unsigned long tick_usec = 10000;
/* minimum number of elapsed ticks to advance time */
static const unsigned long tick_min = 10;

/* wall time as a time_t or struct timeval */
time_t
Universe::wallTime(struct timeval *tv) const
{
	if(tv)
	{
		*tv = _wallClock;
	}
	return _wallClock.tv_sec;
}

/* wall time, in Ticks */
Ticks
Universe::now(void) const
{
	return _now;
}

/* the number of Ticks per second (100) */
unsigned
Universe::ticksPerSecond(void) const
{
	return (sec_usec / tick_usec);
}

/* the age of the Universe in Ticks */
Ticks
Universe::age(void) const
{
	return _age;
}

Ticks
Universe::tick(void)
{
	struct timeval tv;
	Ticks latest;

	gettimeofday(&tv, NULL);
	latest = (tv.tv_sec * (sec_usec / tick_usec)) + (tv.tv_usec / tick_usec);
	if(latest - _now < (long long) tick_min)
	{
		/* not enough time has elapsed between ticks for us to do anything */
//		fprintf(stderr, "latest - now = %lld, which is less than %lu\n", latest - _now, tick_min);
		return _now;
	}
	/* there has been enough of an increment, update the clocks */
	_wallClock = tv;
	updateClocks();
	return _now;
}

void
Universe::updateClocks(void)
{
	Ticks latest, step;

	latest = (_wallClock.tv_sec * (sec_usec / tick_usec)) + (_wallClock.tv_usec / tick_usec);
	step = latest - _now;
	if(_running)
	{
		/* the current age of the Universe (in Ticks) is defined as:
		*
		* the age at startup
		*  + (
		*     ( the current time - the time at startup )
		*     * the time adjustment rate
		*  )
		*
		* the time adjustent rate defaults to 1.0
		*/
		_age = _ageAtLaunch + (latest - _launchTime);
		json_object_set_new(_meta, "age", json_integer(_age));
//		fprintf(stderr, "Universe::%s: RUNNING: now=%lld, step=%lld, age=%lld\n", __FUNCTION__, _now, step, _age);
	}
	else
	{
//		fprintf(stderr, "Universe::%s: SUSPENDED: tv_sec=%lu, tv_usec=%lu, prev=%lld, now=%lld, step=%lld\n", __FUNCTION__, _wallClock.tv_sec, _wallClock.tv_usec, _now, latest, step);
	}
	_now = latest;
}

bool
Universe::running(void) const
{
	return _running;
}

void
Universe::suspend(void)
{
	if(!_running)
	{
		return;
	}
	updateClocks();
	fprintf(stderr, "Universe::%s: suspending Universe at %lld, aged %lld\n", __FUNCTION__, _now, _age);
	_running = false;
	_ageAtLaunch = _age;
	_launchTime = _now;
}

void
Universe::resume(void)
{
	if(_running)
	{
		return;
	}
	fprintf(stderr, "Universe::%s: resuming Universe at %lld, aged %lld (at launch %lld)\n", __FUNCTION__, _now, _age, _ageAtLaunch);
	_launchTime = _now;
	_running = true;
	updateClocks();
}
