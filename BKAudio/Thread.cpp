#include <ctime>
#include <cstdlib>

#include "Thread.h"
#include <float.h>

#ifdef BKE_SYS_WINDOWS
#include <qt_windows.h>
#else
#include <unistd.h>
#include <cerrno>
#endif


#undef MAX

BKE_Clock::t BKE_Clock::Max = DBL_MAX;

#ifdef BKE_SYS_WINDOWS
BKE_Clock::BKE_Clock(){
	LARGE_INTEGER li;
	this->data = (!QueryPerformanceFrequency(&li)) ? 0 : li.QuadPart / 1000;
}
#endif

BKE_Clock::t BKE_Clock::get() const{
#ifdef BKE_SYS_WINDOWS
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return BKE_Clock::t(li.QuadPart) / BKE_Clock::t(this->data);
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return BKE_Clock::t(tv.tv_sec)*1000.0 + BKE_Clock::t(tv.tv_usec) / 1000.0;
#endif
}
