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

#ifndef LOG_H_
#define LOG_H_

#if !defined (__LIBMISC_H__) && !defined (LIBMISC)
#error only libmisc.h can be included directly.
#endif

#include "DateMisc.h"
#include "Misc.h"
#include "../macro.h"

enum
{
	LOG_DISABLE = 0x00, 
	LOG_STDOUT = 0x01, 
	LOG_FILE = 0x02, 
	LOG_UPSTREAM = 0x04 
};

enum
{
	LOG_LEVEL_RECORD = 0x00, LOG_LEVEL_TRACE, LOG_LEVEL_DEBUG, LOG_LEVEL_INFO, LOG_LEVEL_WARN, LOG_LEVEL_ERROR, LOG_LEVEL_FAULT, LOG_LEVEL_OPER
};

class Log
{
public:
	static void init(const char* path = NULL, int o = LOG_STDOUT); 
	static void output(int lev, char* log); 
	static uchar getOutput(); 
	static void setOutput(int o); 
	static void setOutput(const string& str); 
	static void disableOutPut(); 
	static bool isOutPutStdout(); 
	static bool isOutPutFile(); 
	static bool isOutPutUpstream(); 
	static void setRecord(); 
	static void setTrace(); 
	static void setDebug(); 
	static void setInfo(); 
	static void setWarn(); 
	static void setError(); 
	static void setFault(); 
	static void setOper(); 
	static bool isRecord(); 
	static bool isTrace(); 
	static bool isDebug(); 
	static bool isInfo(); 
	static bool isWarn(); 
	static bool isError(); 
	static bool isFault(); 
	static bool isOper(); 
	static void setLevel(const char* lev); 
	static const char* getLevStr(); 
	static const char* getLevStr(uchar lev); 
	static void stream(function<void(uchar lev, const char* log)> log); 
private:
	static uchar o;
	static uchar lev;
	static string path;
	static function<void(uchar lev, const char* log)>* logStream;
private:
	Log();
	virtual ~Log();
	static void write(char* log);
	static void setLev(uchar lev);
};

#define __RECO__(str, format, ...) SPRINTF_CSTR((str), "%s[RECO]%u(%s %s %d)" format, DateMisc::to_hh_mi_ss_ms().c_str(), Misc::getTid(), __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define __TRAC__(str, format, ...) SPRINTF_CSTR((str), "%s[TRAC]%u(%s %s %d)" format, DateMisc::to_hh_mi_ss_ms().c_str(), Misc::getTid(), __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define __DEBU__(str, format, ...) SPRINTF_CSTR((str), "%s[DEBU]%u(%s %s %d)" format, DateMisc::to_hh_mi_ss_ms().c_str(), Misc::getTid(), __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define __INFO__(str, format, ...) SPRINTF_CSTR((str), "%s[INFO]%u(%s %s %d)" format, DateMisc::to_hh_mi_ss_ms().c_str(), Misc::getTid(), __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define __WARN__(str, format, ...) SPRINTF_CSTR((str), "%s[WARN]%u(%s %s %d)" format, DateMisc::to_hh_mi_ss_ms().c_str(), Misc::getTid(), __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define __ERRO__(str, format, ...) SPRINTF_CSTR((str), "%s[ERRO]%u(%s %s %d)" format, DateMisc::to_hh_mi_ss_ms().c_str(), Misc::getTid(), __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define __FAUL__(str, format, ...) SPRINTF_CSTR((str), "%s[FAUL]%u(%s %s %d)" format, DateMisc::to_hh_mi_ss_ms().c_str(), Misc::getTid(), __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define __OPER__(str, format, ...) SPRINTF_CSTR((str), "%s[OPER]%u(%s %s %d)" format, DateMisc::to_hh_mi_ss_ms().c_str(), Misc::getTid(), __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define LOG_RECORD(format, ...)	\
if(Log::isRecord()) \
{	\
	char* ___cstr___ = NULL;	\
	__RECO__(___cstr___, format, ##__VA_ARGS__)	\
	Log::output(LOG_LEVEL_RECORD, ___cstr___);	\
}

#define LOG_TRACE(format, ...)	\
if(Log::isTrace()) \
{	\
	char* ___cstr___ = NULL;	\
	__TRAC__(___cstr___, format, ##__VA_ARGS__)	\
	Log::output(LOG_LEVEL_TRACE, ___cstr___);	\
}

#define LOG_DEBUG(format, ...)	\
if(Log::isDebug()) \
{	\
	char* ___cstr___ = NULL;	\
	__DEBU__(___cstr___, format, ##__VA_ARGS__)	\
	Log::output(LOG_LEVEL_DEBUG, ___cstr___);	\
}

#define LOG_INFO(format, ...)	\
if(Log::isInfo()) \
{	\
	char* ___cstr___ = NULL;	\
	__INFO__(___cstr___, format, ##__VA_ARGS__)	\
	Log::output(LOG_LEVEL_INFO, ___cstr___);	\
}

#define LOG_WARN(format, ...)	\
if(Log::isWarn()) \
{	\
	char* ___cstr___ = NULL;	\
	__WARN__(___cstr___, format, ##__VA_ARGS__)	\
	Log::output(LOG_LEVEL_WARN, ___cstr___);	\
}

#define LOG_ERROR(format, ...)	\
if(Log::isError()) \
{	\
	char* ___cstr___ = NULL;	\
	__ERRO__(___cstr___, format, ##__VA_ARGS__)	\
	Log::output(LOG_LEVEL_ERROR, ___cstr___);	\
}

#define LOG_FAULT(format, ...)	\
if(Log::isFault()) \
{	\
	char* ___cstr___ = NULL;	\
	__FAUL__(___cstr___, format, ##__VA_ARGS__)	\
	Log::output(LOG_LEVEL_FAULT, ___cstr___);	\
}

#define LOG_OPER(format, ...)	\
if(Log::isOper()) \
{	\
	char* ___cstr___ = NULL;	\
	__OPER__(___cstr___, format, ##__VA_ARGS__)	\
	Log::output(LOG_LEVEL_OPER, ___cstr___);	\
}

#ifdef WINDOWS
#define SPRINTF_STRING(___string___, format, ...) \
{	\
	char* ___cstr___ = NULL;	\
	Misc::asprintf(&___cstr___, format, ##__VA_ARGS__);	\
	(___string___)->append(___cstr___);	\
	::free(___cstr___);	\
}
#else
#define SPRINTF_STRING(___string___, format, ...) \
{	\
	char* ___cstr___ = NULL;	\
	::asprintf(&___cstr___, format, ##__VA_ARGS__);	\
	(___string___)->append(___cstr___);	\
	::free(___cstr___);	\
}
#endif

#ifdef WINDOWS
#define SPRINTF_CSTR(___cstr___, format, ...) \
{	\
	Misc::asprintf(&___cstr___, format, ##__VA_ARGS__);	\
}
#else
#define SPRINTF_CSTR(___cstr___, format, ...) \
{	\
	::asprintf(&___cstr___, format, ##__VA_ARGS__);	\
}
#endif

#endif 
