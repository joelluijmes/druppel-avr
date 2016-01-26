#pragma once

#include "../../util/ds1307_usi/ds1307.h"

static uint32_t _swstart;

static inline uint32_t time_current()
{
	return read_unix_time();
}

static inline void time_swreset()
{
	_swstart = time_current();
}

static inline uint32_t time_swstart()
{
	return _swstart;
}

static inline uint32_t time_swelapsed()
{
	return time_current() - _swstart;
}
