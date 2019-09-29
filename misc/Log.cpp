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

#include "Log.h"
#include "Misc.h"
#include "Net.h"

static const char* LOG_LEV_STR[] = { "RECORD", "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FAULT", "OPER" };

uchar Log::o = LOG_STDOUT;
uchar Log::lev = LOG_LEVEL_DEBUG;
string Log::path = "../log/";
function<void(uchar lev, const char* log)>* Log::logStream = NULL;

Log::Log()
{

}

void Log::init(const char* path, int o)
{
	Log::o = o & 0x07;
	Log::lev = LOG_LEVEL_INFO; 
	Log::path.clear();
	SPRINTF_STRING(&Log::path, "%s", path == NULL ? "../log/": path)
}

uchar Log::getOutput()
{
	return Log::o;
}

void Log::setOutput(int o)
{
	Log::o = o;
}

void Log::setOutput(const string& str)
{
	if ("DISABLE" == str)
	{
		Log::disableOutPut();
		return;
	}
	uchar o = LOG_DISABLE;
	if (str.find("STDOUT") >= 0)
		o |= LOG_STDOUT;
	if (str.find("FILE") >= 0)
		o |= LOG_FILE;
	if (str.find("UPSTREAM") >= 0)
		o |= LOG_UPSTREAM;
	if (o != LOG_DISABLE)
		Log::setOutput(o);
}

void Log::disableOutPut()
{
	Log::o = LOG_DISABLE;
}

bool Log::isOutPutStdout()
{
	return Log::o & LOG_STDOUT;
}

bool Log::isOutPutFile()
{
	return Log::o & LOG_FILE;
}

bool Log::isOutPutUpstream()
{
	return Log::o & LOG_UPSTREAM;
}

void Log::setRecord()
{
	Log::setLev(LOG_LEVEL_RECORD);
}

void Log::setTrace()
{
	Log::setLev(LOG_LEVEL_TRACE);
}

void Log::setDebug()
{
	Log::setLev(LOG_LEVEL_DEBUG);
}

void Log::setInfo()
{
	Log::setLev(LOG_LEVEL_INFO);
}

void Log::setWarn()
{
	Log::setLev(LOG_LEVEL_WARN);
}

void Log::setError()
{
	Log::setLev(LOG_LEVEL_ERROR);
}

void Log::setFault()
{
	Log::setLev(LOG_LEVEL_FAULT);
}

void Log::setOper()
{
	Log::setLev(LOG_LEVEL_OPER);
}

void Log::setLev(uchar lev)
{
	if (lev <= LOG_LEVEL_OPER && lev >= LOG_LEVEL_RECORD)
		Log::lev = lev;
}

bool Log::isRecord()
{
	return Log::lev <= LOG_LEVEL_RECORD;
}

bool Log::isTrace()
{
	return Log::lev <= LOG_LEVEL_TRACE;
}

bool Log::isDebug()
{
	return Log::lev <= LOG_LEVEL_DEBUG;
}

bool Log::isInfo()
{
	return Log::lev <= LOG_LEVEL_INFO;
}

bool Log::isWarn()
{
	return Log::lev <= LOG_LEVEL_WARN;
}

bool Log::isError()
{
	return Log::lev <= LOG_LEVEL_ERROR;
}

bool Log::isFault()
{
	return Log::lev <= LOG_LEVEL_FAULT;
}

bool Log::isOper()
{
	return Log::lev <= LOG_LEVEL_OPER;
}

const char* Log::getLevStr()
{
	return LOG_LEV_STR[Log::lev];
}

const char* Log::getLevStr(uchar lev)
{
	return LOG_LEV_STR[lev % (LOG_LEVEL_OPER + 1)];
}

void Log::setLevel(const char* lev)
{
	if (::strcmp(lev, LOG_LEV_STR[LOG_LEVEL_RECORD]) == 0)
		Log::setRecord();
	else if (::strcmp(lev, LOG_LEV_STR[LOG_LEVEL_TRACE]) == 0)
		Log::setTrace();
	else if (::strcmp(lev, LOG_LEV_STR[LOG_LEVEL_DEBUG]) == 0)
		Log::setDebug();
	else if (::strcmp(lev, LOG_LEV_STR[LOG_LEVEL_WARN]) == 0)
		Log::setWarn();
	else if (::strcmp(lev, LOG_LEV_STR[LOG_LEVEL_ERROR]) == 0)
		Log::setError();
	else if (::strcmp(lev, LOG_LEV_STR[LOG_LEVEL_FAULT]) == 0)
		Log::setFault();
	else if (::strcmp(lev, LOG_LEV_STR[LOG_LEVEL_OPER]) == 0)
		Log::setOper();
	else
		Log::setInfo();
}

void Log::output(int lev, char* log)
{
	if (Log::o)
	{
		if (Log::isOutPutStdout())
			::printf("%s\n", log);
		if (Log::isOutPutFile())
			Log::write(log);
		if (Log::isOutPutUpstream() && Log::logStream != NULL)
			(*(Log::logStream))(lev, log);
	}
	::free(log);
}

void Log::write(char* log)
{
	string file;
	SPRINTF_STRING(&file, "%s%s.log", Log::path.c_str(), DateMisc::to_yyyy_mm_dd().c_str())
	struct stat filestat;
	if (::stat(file.c_str(), &filestat) != -1 && (sizeof(size_t) == 4 && filestat.st_size > 1024 * 1024 * 2000) ) 
	{
		string nfile;
		SPRINTF_STRING(&nfile, "%s.bak", file.c_str())
		::remove(nfile.c_str());
		::rename(file.c_str(), nfile.c_str());
	}
	FILE *fp;
	if ((fp = ::fopen(file.c_str(), "a+")) == NULL)
		return;
	::fprintf(fp, "%s\n", log);
	::fclose(fp);
}

void Log::stream(function<void(uchar lev, const char* log)> log)
{
	Log::logStream = new function<void(uchar lev, const char* log)>();
	*(Log::logStream) = log;
}

Log::~Log()
{

}

