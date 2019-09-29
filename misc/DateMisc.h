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

#ifndef DATEMISC_H_
#define DATEMISC_H_

#if !defined (__LIBMISC_H__) && !defined (LIBMISC)
#error only libmisc.h can be included directly.
#endif

#include "../macro.h"

class DateMisc
{
public:
	static time_t msec; 
	static time_t sec; 
	static time_t minute; 
	static time_t hour; 
	static time_t day; 
	static time_t week; 
public:
	static void gettimeofday(struct timeval* tv); 
	static ullong nowUsec(); 
	static ullong now(); 
	static ullong nowGmt0(); 
	static ullong gotoGmt0(ullong ts); 
	static ullong getMsec(struct timeval* ts); 
	static int elap(ullong sts ); 
	static int elap(ullong aft, ullong bef); 
	static int elap(struct timeval* aft, struct timeval* bef); 
	static int elapDida(ullong sts ); 
	static ullong dida(); 
	static ullong didaMsec(); 
	static int getYear(time_t ts); 
	static int getMonth(time_t ts); 
	static int getDay(time_t ts); 
	static int getHour(time_t ts); 
	static int getMinu(time_t ts); 
	static int getSec(time_t ts); 
	static ullong timeZone(); 
	static string to_yyyymm(); 
	static string to_yyyymmddhhmiss(); 
	static string to_yyyymmddhhmiss(time_t ts); 
	static string to_yyyy_mm_dd(); 
	static string to_yyyy_mm_dd(time_t ts); 
	static string to_yyyy_mm_dd_hh(); 
	static string to_yyyy_mm_dd_hh_mi_ss(); 
	static string to_yyyy_mm_dd_hh_mi_ss(time_t ts); 
	static string to_yyyy_mm_dd_hh_mi_ss_ms(); 
	static string to_yyyy_mm_dd_hh_mi_ss_ms(ullong ts ); 
	static string to_hh_mi_ss_ms(); 
	static string to_hh_mi_ss_ms(ullong ts ); 
	static string to_hh_mi_ss(); 
	static string lastMonth(); 
	static time_t parse_yyyy_mm_dd_hh_mi_ss(char* str); 
	static time_t makeTime(int year, int month, int day, int hour, int minu, int sec); 
	static bool isSameDay(time_t f, time_t s); 
private:
	DateMisc();
	virtual ~DateMisc();
};

#endif 

