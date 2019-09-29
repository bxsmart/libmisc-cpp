/*
  Copyright 2019 www.dev5.cn, Inc. dev5@qq.com
 
  This file is part of X-MSG-IM.
 
  X-MSG-IM is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  X-MSG-IM is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU Affero General Public License
  along with X-MSG-IM.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "DateMisc.h"
#include "Log.h"

time_t DateMisc::msec = 1L; 
time_t DateMisc::sec = DateMisc::msec * 1000L; 
time_t DateMisc::minute = DateMisc::sec * 60; 
time_t DateMisc::hour = DateMisc::minute * 60; 
time_t DateMisc::day = DateMisc::hour * 24; 
time_t DateMisc::week = DateMisc::day * 7; 

#if defined(WINDOWS)
void localtime_r(time_t* t, struct tm* tm)
{
	struct tm* m = ::localtime(t);
	::memcpy(tm, m, sizeof(struct tm));
}
#endif

#if defined(WINDOWS) && !(defined(__MINGW32__) || defined(__MINGW64__))
#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS) || defined(__WATCOMC__)
#define DELTA_EPOCH_IN_USEC 11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_USEC 11644473600000000ULL
#endif
typedef unsigned __int64 u_int64_t;
static u_int64_t filetime_to_unix_epoch(const FILETIME *ft)
{
	u_int64_t res = (u_int64_t) ft->dwHighDateTime << 32;
	res |= ft->dwLowDateTime;
	res /= 10;
	res -= DELTA_EPOCH_IN_USEC;
	return (res);
}
#endif

DateMisc::DateMisc()
{

}

void DateMisc::gettimeofday(struct timeval* tv)
{
#if defined(WINDOWS) && !(defined(__MINGW32__) || defined(__MINGW64__))
	FILETIME ft;
	u_int64_t tim;
	GetSystemTimeAsFileTime(&ft);
	tim = filetime_to_unix_epoch(&ft);
	tv->tv_sec = (long) (tim / 1000000ULL);
	tv->tv_usec = (long) (tim % 1000000ULL);
#else
	::gettimeofday(tv, NULL);
#endif
}

ullong DateMisc::nowUsec()
{
	struct timeval tv;
	DateMisc::gettimeofday(&tv);
	ullong ts = tv.tv_sec;
	ts *= 1000000ULL;
	ts += tv.tv_usec;
	return ts;
}

ullong DateMisc::now()
{
	return DateMisc::nowUsec() / 1000ULL;
}

ullong DateMisc::nowGmt0()
{
	return DateMisc::now() - DateMisc::timeZone();
}

ullong DateMisc::gotoGmt0(ullong ts)
{
	return ts - DateMisc::timeZone();
}

int DateMisc::elap(ullong sts )
{
	return (int) (DateMisc::now() - sts);
}

int DateMisc::elap(ullong aft, ullong bef)
{
	return (int) (aft - bef);
}

int DateMisc::elap(struct timeval* after, struct timeval* before)
{
	long int sec = after->tv_sec - before->tv_sec;
	long int use = after->tv_usec - before->tv_usec;
	sec = use < 0 ? (sec > 0 ? sec - 1 : sec) : sec; 
	use = use < 0 ? (1000000 - before->tv_usec + after->tv_usec) : (after->tv_usec - before->tv_usec);
	return (int) (sec * 1000000 + use);
}

int DateMisc::elapDida(ullong sts )
{
	return (int) ((DateMisc::dida() - sts) / 1000L);
}

ullong DateMisc::getMsec(struct timeval* ts)
{
	if (ts != NULL)
	{
		ullong x = ts->tv_sec;
		x = x * (ullong) 1000;
		x += (ts->tv_usec / 1000);
		return x;
	}
	struct timeval now;
	DateMisc::gettimeofday(&now);
	return DateMisc::getMsec(&now);
}

ullong DateMisc::dida()
{
#ifdef WINDOWS
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	double f = (double) freq.QuadPart;
	QueryPerformanceCounter(&freq);
	double s = freq.QuadPart;
	return (s / f) * 1000 * 1000;
#else
#ifdef LINUX
	struct timespec dida;
	::clock_gettime(CLOCK_MONOTONIC, &dida);
	return (dida.tv_sec * 1000000) + (dida.tv_nsec / 1000);
#else
	return 0ULL; 
#endif
#endif
}

ullong DateMisc::didaMsec()
{
	return DateMisc::dida() / 1000ULL;
}

int DateMisc::getYear(time_t ts)
{
	struct tm ft;
	::localtime_r(&ts, &ft);
	return ft.tm_year + 1900;
}

int DateMisc::getMonth(time_t ts)
{
	struct tm ft;
	::localtime_r(&ts, &ft);
	return ft.tm_mon + 1;
}

int DateMisc::getDay(time_t ts)
{
	struct tm ft;
	::localtime_r(&ts, &ft);
	return ft.tm_mday;
}

int DateMisc::getHour(time_t ts)
{
	struct tm ft;
	::localtime_r(&ts, &ft);
	return ft.tm_hour;
}

int DateMisc::getMinu(time_t ts)
{
	struct tm ft;
	::localtime_r(&ts, &ft);
	return ft.tm_min;
}

int DateMisc::getSec(time_t ts)
{
	struct tm ft;
	::localtime_r(&ts, &ft);
	return ft.tm_sec;
}

ullong DateMisc::timeZone()
{
	static ullong offset = 0xFFFFFFFFFFFFFFFFULL;
	if (offset == 0xFFFFFFFFFFFFFFFFULL)
	{
#if defined(__MINGW32__) || defined(__MINGW64__)
			offset = timezone;
	#else
		struct tm ret;
		time_t t;
		::localtime_r(&t, &ret);
		offset = (uint) ret.tm_gmtoff * 1000L;
#endif
	}
	return offset;
}

string DateMisc::to_yyyymm()
{
	string str;
	time_t ts = ::time(NULL);
	struct tm ft;
	::localtime_r(&ts, &ft);
	SPRINTF_STRING(&str, "%04d%02d", ft.tm_year + 1900, ft.tm_mon + 1);
	return str;
}

string DateMisc::to_yyyymmddhhmiss()
{
	return DateMisc::to_yyyymmddhhmiss(::time(NULL));
}

string DateMisc::to_yyyymmddhhmiss(time_t ts)
{
	string str;
	struct tm ft;
	::localtime_r(&ts, &ft);
	SPRINTF_STRING(&str, "%04d%02d%02d%02d%02d%02d", ft.tm_year + 1900, ft.tm_mon + 1, ft.tm_mday, ft.tm_hour, ft.tm_min, ft.tm_sec)
	return str;
}

string DateMisc::to_yyyy_mm_dd()
{
	return DateMisc::to_yyyy_mm_dd(::time(NULL));
}

string DateMisc::to_yyyy_mm_dd(time_t ts)
{
	string str;
	struct tm ft;
	::localtime_r(&ts, &ft);
	SPRINTF_STRING(&str, "%04d-%02d-%02d", ft.tm_year + 1900, ft.tm_mon + 1, ft.tm_mday)
	return str;
}

string DateMisc::to_yyyy_mm_dd_hh()
{
	string str;
	time_t ts = ::time(NULL);
	struct tm ft;
	::localtime_r(&ts, &ft);
	SPRINTF_STRING(&str, "%04d-%02d-%02d-%02d", ft.tm_year + 1900, ft.tm_mon + 1, ft.tm_mday, ft.tm_hour);
	return str;
}

string DateMisc::to_yyyy_mm_dd_hh_mi_ss()
{
	return DateMisc::to_yyyy_mm_dd_hh_mi_ss(::time(NULL));
}

string DateMisc::to_yyyy_mm_dd_hh_mi_ss(time_t ts)
{
	string str;
	struct tm ft;
	::localtime_r(&ts, &ft);
	SPRINTF_STRING(&str, "%04d-%02d-%02d %02d:%02d:%02d", ft.tm_year + 1900, ft.tm_mon + 1, ft.tm_mday, ft.tm_hour, ft.tm_min, ft.tm_sec)
	return str;
}

string DateMisc::to_yyyy_mm_dd_hh_mi_ss_ms()
{
	return DateMisc::to_yyyy_mm_dd_hh_mi_ss_ms(DateMisc::now());
}

string DateMisc::to_yyyy_mm_dd_hh_mi_ss_ms(ullong ts )
{
	time_t tts = ts / 1000L;
	struct tm ft;
	::localtime_r((time_t*) &tts, &ft);
	string str;
	SPRINTF_STRING(&str, "%04d-%02d-%02d %02d:%02d:%02d.%03d", ft.tm_year + 1900, ft.tm_mon + 1, ft.tm_mday, ft.tm_hour, ft.tm_min, ft.tm_sec, (int )(ts % 1000L))
	return str;
}

string DateMisc::to_hh_mi_ss_ms()
{
	return DateMisc::to_hh_mi_ss_ms(DateMisc::now());
}

string DateMisc::to_hh_mi_ss_ms(ullong ts )
{
	struct tm ft;
	time_t tts = ts / 1000L;
	::localtime_r((time_t*) &tts, &ft);
	string str;
	SPRINTF_STRING(&str, "%02d:%02d:%02d.%03d", ft.tm_hour, ft.tm_min, ft.tm_sec, (int )(ts % 1000L))
	return str;
}

string DateMisc::to_hh_mi_ss()
{
	char buff[0x09] = { 0 };
	time_t now = ::time(NULL);
	struct tm ft;
	::localtime_r(&now, &ft);
	::strftime(buff, 0x09, "%X", &ft);
	return string(buff);
}

string DateMisc::lastMonth()
{
	time_t now = ::time(NULL);
	struct tm ft;
	::localtime_r(&now, &ft);
	int y = ft.tm_year + 1900;
	int m = ft.tm_mon + 1;
	if (m > 1)
		m -= 1;
	else
	{
		y -= 1;
		m = 12;
	}
	string str;
	SPRINTF_STRING(&str, "%04d%02d", y, m);
	return str;
}

time_t DateMisc::parse_yyyy_mm_dd_hh_mi_ss(char* str)
{
	struct tm tm;
#ifdef LINUX
	::strptime(str, "%Y-%m-%d %H:%M:%S", &tm);
#else
	LOG_FAULT("unsupported OS.")
#endif
	return ::mktime(&tm);
}

time_t DateMisc::makeTime(int year, int month, int day, int hour, int minu, int sec)
{
	struct tm tm;
	tm.tm_year = year;
	tm.tm_mon = month;
	tm.tm_mday = day;
	tm.tm_hour = hour;
	tm.tm_min = minu;
	tm.tm_sec = sec;
	return ::mktime(&tm);
}

bool DateMisc::isSameDay(time_t f, time_t s)
{
	struct tm ft;
	struct tm st;
	::localtime_r(&f, &ft);
	::localtime_r(&s, &st);
	return (ft.tm_mday == st.tm_mday) && (::abs(f - s) < DateMisc::day / 1000L);
}

DateMisc::~DateMisc()
{

}

