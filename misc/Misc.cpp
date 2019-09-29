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

#include "Misc.h"
#include "Log.h"
#include "Net.h"

#ifdef WINDOWS
#include <direct.h>
#include <io.h>
#define ACCESS _access
#define MKDIR(a) _mkdir((a))
#else
#include <stdarg.h>
#include <sys/stat.h>
#define ACCESS access
#define MKDIR(a) mkdir((a), 0755)
#endif

Misc::Misc()
{

}

void Misc::sleep(int mec)
{
#ifdef WINDOWS
	Sleep(mec);
#else
	::usleep(1000 * mec);
#endif
}

void Misc::hold()
{
	promise<int> pro;
	pro.get_future().get();
}

int Misc::asprintf(char **strp, const char *fmt, ...)
{
#ifdef WINDOWS
	va_list va;
	va_start(va, fmt);
	const int required = vsnprintf(NULL, 0, fmt, va);
	char* const buffer = (char*) malloc(required + 1);
	const int ret = vsnprintf(buffer, required + 1, fmt, va);
	*strp = buffer;
	va_end(va);
	return ret;
#else
	va_list va;
	::va_start(va, fmt);
	int ret = ::vasprintf(strp, fmt, va);
	va_end(va);
	return ret;
#endif
}

string Misc::getStack()
{
#if defined(LINUX) && !defined(ANDROID)
	string ss;
	void* fs[0x100];
	int fc = ::backtrace(fs, 0x100);
	char** strs = ::backtrace_symbols(fs, fc);
	if (strs == NULL)
	{
		ss.assign("no stack.");
		return ss;
	}
	for (int i = 0; i < fc; ++i)
		ss.append(strs[i]).append("\n");
	::free(strs);
	return ss;
#else
	return string("unsupported OS.");
#endif
}

void Misc::lazyExit()
{
	Misc::sleep(5 * 1000);
	::exit(EXIT_FAILURE);
}

uint Misc::getTid()
{
#if defined(LINUX) || defined(ANDROID)
#define __get_tid__() 													syscall(__NR_gettid)			
#endif
#ifdef WINDOWS
#define __get_tid__() 													GetCurrentThreadId()			
#endif
#ifdef MAC
#define __get_tid__() 													getpid()						
#endif
	return (uint) __get_tid__();
}

size_t Misc::hash(const string& str)
{
	return std::hash<string> { }(str);
}

bool Misc::checkNameFormat(const char* name, int min, int max)
{
	if (name == NULL)
		return false;
	int len = ::strlen(name);
	if (len < min || len > max)
		return false;
	for (int i = 0; i < len; ++i)
	{
		if ((::isdigit(name[i]) == 0) && (::isalpha(name[i]) == 0) && (name[i] != '-') && (name[i] != '@') && (name[i] != '.') && (name[i] != '_'))
			return false;
	}
	return true;
}

bool Misc::checkPwdFormat(const char* name, int min, int max)
{
	return Misc::checkNameFormat(name, min, max);
}

bool Misc::checkEmailFormat(const char* email, int max)
{
	if (email == NULL)
		return false;
	int len = ::strlen(email);
	if (len < 3 || len > max) 
		return false;
	for (int i = 0; i < len; ++i)
	{
		if (email[i] != 0x2E && email[i] != 0x40 && (::isdigit(email[i]) == 0) && (::isalpha(email[i]) == 0))
			return false;
	}
	return true;
}

bool Misc::checkRegex(const char* str, const char* pattern)
{
#ifdef LINUX
	int ret;
	regex_t regex;
	ret = ::regcomp(&regex, pattern, REG_EXTENDED | REG_ICASE);
	if (ret != 0)
		return false;
	ret = ::regexec(&regex, str, 0, NULL, 0);
	::regfree(&regex);
	return ret != REG_NOMATCH;
#else
	LOG_FAULT("unsupported OS.")
	return false;
#endif
}

bool Misc::checkUtf8str(const string& role, int min, int max)
{
	int len = 0;
	for (uint i = 0; i < role.length();)
	{
		char word[4] = { 0 };
		::strncpy(word, role.c_str() + i, 3);
		if (Misc::checkRegex(word, "^[\u4E00-\u9FFF]"))
			i += 3; 
		else
			i += 1;
		len += 1;
	}
	if (len < min || len > max) 
		return false;
	return Misc::checkRegex(role.c_str(), "^[\u4E00-\u9FFF|0-9a-zA-Z][\u4E00-\u9FFF|0-9a-zA-Z]*$");
}

bool Misc::isAlphaString(const char* str)
{
	int len = ::strlen(str);
	for (int i = 0; i < len; ++i)
		if (::isalpha(str[i]) == 0)
			return false;
	return len > 0;
}

bool Misc::isDigitString(const char* str)
{
	int len = ::strlen(str);
	for (int i = 0; i < len; ++i)
	{
		if (::isdigit(str[i]) == 0)
			return false;
	}
	return len > 0;
}

bool Misc::isAlpDiString(const char* str)
{
	int len = ::strlen(str);
	for (int i = 0; i < len; ++i)
	{
		if (::isalpha(str[i]) == 0 && ::isdigit(str[i]) == 0)
			return false;
	}
	return len > 0;
}

bool Misc::isAlpDiStr(string* str)
{
	int len = str->length();
	for (int i = 0; i < len; ++i)
	{
		if (::isalpha(str->at(i)) == 0 && ::isdigit(str->at(i)) == 0)
			return false;
	}
	return len > 0;
}

bool Misc::isVisAdString(const char* str)
{
	int len = ::strlen(str);
	for (int i = 0; i < len; ++i)
	{
		if (str[i] < 0x20 || str[i] > 0x7E)
			return false;
	}
	return len > 0;
}

bool Misc::isVisAdStr(string* str)
{
	int len = str->length();
	for (int i = 0; i < len; ++i)
	{
		if (str->at(i) < 0x20 || str->at(i) > 0x7E)
			return false;
	}
	return len > 0;
}

bool Misc::startWith(const string& str, const string& sub)
{
	return str.find(sub) == 0;
}

bool Misc::endWith(const string& str, const string& sub)
{
	size_t strLen = str.length();
	size_t subLen = sub.length();
	if (strLen == 0 || subLen == 0 || subLen > strLen)
		return false;
	for (size_t i = 1; i <= subLen; ++i)
	{
		if (str.data()[strLen - i] != sub.data()[subLen - i])
			return false;
	}
	return true;
}

void Misc::split(const string str, const string& key, vector<string>& arr, int num)
{
	int pos = 0;
	int keylen = (int) key.length();
	int len = (int) str.length();
	int tlen = len;
	const char* tmp = ::strstr(str.c_str(), key.c_str());
	while (tmp != NULL)
	{
		int xlen = ::strlen(tmp);
		if (tlen > xlen)
		{
			if (num == 0 || (int) arr.size() < num)
				arr.push_back(string(str.data() + pos, tlen - xlen));
			else
				return;
		}
		pos += (tlen - xlen) + keylen;
		tlen = xlen - keylen;
		tmp = ::strstr(str.data() + pos, key.c_str());
	}
	if (len > pos)
	{
		if (num == 0 || (int) arr.size() < num)
			arr.push_back(string(str.data() + pos, len - pos));
	}
}

string Misc::replaceAll(string& str, const string& org, const string& des)
{
	while (true)
	{
		string::size_type pos(0);
		if ((pos = str.find(org)) == string::npos)
			break;
		str.replace(pos, org.length(), des);
	}
	return str;
}

string Misc::intArr2sqlStr(const vector<int>& arr)
{
	if (arr.empty())
		return "";
	string str = "(";
	size_t i = 0;
	for (; i < arr.size() - 1; ++i)
	{
		SPRINTF_STRING(&str, "%d, ", arr.at(i))
	}
	SPRINTF_STRING(&str, "%d)", arr.at(i))
	return str;
}

string Misc::strArr2sqlStr(const vector<string>& arr)
{
	if (arr.empty())
		return "";
	string str = "(";
	size_t i = 0;
	for (; i < arr.size() - 1; ++i)
	{
		SPRINTF_STRING(&str, "'%s', ", arr.at(i).c_str())
	}
	SPRINTF_STRING(&str, "'%s')", arr.at(i).c_str())
	return str;
}

string Misc::toUpercase(const string& str)
{
	string out = str;
	::transform(out.begin(), out.end(), out.begin(), ::toupper);
	return out;
}

string Misc::toLowercase(const string& str)
{
	string out = str;
	::transform(out.begin(), out.end(), out.begin(), ::tolower);
	return out;
}

list<string> Misc::getEnvs()
{
	list<string> envs;
#ifdef LINUX
	char **env = environ;
	while (*env)
		envs.push_back(string(*env++));
	envs.sort();
#else
	LOG_FAULT("unsupported OS.")
#endif
	return envs;
}

string Misc::getEnvStr(const char* key, const char* def)
{
	char* str = ::getenv(key);
	return (str != NULL) ? string(str) : (def == NULL ? "" : def);
}

int Misc::getEnvInt(const char* key, int def)
{
#ifndef WINDOWS
	char* str = ::getenv(key);
	return (str == NULL) ? def : Net::hexOrint(str);
#else
	LOG_FAULT("unsupported OS.")
	return 0;
#endif
}

string Misc::getSetEnvStr(const char* key, const char* def)
{
	string val = Misc::getEnvStr(key, def);
	Misc::setEnv(key, val.c_str());
	return val;
}

int Misc::getSetEnvInt(const char* key, int def)
{
	int val = Misc::getEnvInt(key, def);
	Misc::setEnv(key, Net::itoa(val).c_str());
	return val;
}

void Misc::setEnv(const char* key, const char* val)
{
#ifdef LINUX
	::setenv(key, val, 1);
#else
	LOG_FAULT("unsupported OS.")
#endif
}

const char* Misc::pathSeparator()
{
#ifdef WINDOWS
	static const char* s = "\\";
#else
	static const char* s = "/";
#endif
	return s;
}

int Misc::getFileAtt(char* path, char* name, ullong* size)
{
	int i = ::strlen(path) - 1;
	int indx = -1;
	for (; i >= 0; --i)
	{
#ifdef WINDOWS
		if (path[i] == '\\')
#else
		if (path[i] == '/')
#endif
		{
			indx = i;
			break;
		}
	}
	if (name != NULL)
	{
		if (indx != -1)
			::memcpy(name, path + indx + 1, ::strlen(path) - indx - 1);
		else
			::memcpy(name, path, ::strlen(path));
	}
#ifdef WINDOWS
	struct _stati64 s64;
	if (_stati64(path, &s64) != 0)
	return 1;
	*size = s64.st_size;
	return 0;
#else
	struct stat x;
	if (::stat(path, &x) != 0)
		return 1;
	*size = x.st_size;
	return 0;
#endif
}

void Misc::listFiles(const char* path, list<string>* files)
{
#ifndef WINDOWS
	DIR* dir = ::opendir(path);
	struct dirent *dr;
	while ((dr = ::readdir(dir)) != NULL)
	{
		if (::strcmp(".", dr->d_name) != 0 && ::strcmp("..", dr->d_name) != 0)
			files->push_back(string(dr->d_name));
	}
	::closedir(dir);
#else
	LOG_FAULT("unsupported OS.")
#endif
}

string Misc::loadFile(const char* file)
{
#ifndef WINDOWS
	ullong xlen = 0;
	if (Misc::getFileAtt((char*) file, NULL, &xlen) != 0)
		return NULL;
	int f = ::open(file, O_RDONLY);
	if (f < 1)
		return NULL;
	size_t len = xlen;
	uchar* dat = (uchar*) ::malloc(len);
	if (::read(f, dat, len) != (int) len)
	{
		::free(dat);
		::close(f);
		return NULL;
	}
	::close(f);
	string str;
	str.assign((const char*) dat, len);
	::free(dat);
	return str;
#else
	LOG_FAULT("unsupported OS.")
	return "";
#endif
}

bool Misc::mkdir(const string& dir)
{
#ifdef WINDOWS
	return _mkdir(dir.c_str()) == 0;
#else
	return ::mkdir(dir.c_str(), 0755) == 0;
#endif
}

bool Misc::isDirExisted(const string& dir)
{
#ifdef WINDOWS
	return _access(dir.c_str(), 0) == 0;
#else
	return ::access(dir.c_str(), 0) == 0;
#endif
}

int Misc::readAll(int fd, uchar* buf, int len)
{
#ifdef LINUX
	int total = 0;
	int ofst = 0;
	while (true)
	{
		int size = ::read(fd, buf + ofst, len - ofst);
		if (size < 1)
			return -1;
		total += size;
		if (total == len)
			return len;
		ofst += size;
	}
#endif
	LOG_FAULT("unsupported OS.")
	return -1;
}

bool Misc::strAtt(XMLElement* element, const char* att, string* val)
{
	const char* x = element->Attribute(att);
	if (x == NULL)
		return false;
	val->clear();
	val->assign(x);
	return true;
}

string Misc::strAtt(XMLElement* element, const char* att)
{
	string str;
	Misc::strAtt(element, att, &str);
	return str;
}

bool Misc::strChild(XMLElement* element, const char* node, string* val)
{
	XMLElement* child = element->FirstChildElement(node);
	if (child == NULL)
		return false;
	val->assign(child->GetText() == NULL ? "" : child->GetText());
	return true;
}

string Misc::strChild(XMLElement* element, const char* node)
{
	string str;
	Misc::strChild(element, node, &str);
	return str;
}

bool Misc::intAtt(XMLElement* element, const char* att, uint* val)
{
	*val = element->IntAttribute(att);
	return true;
}

uint Misc::intAtt(XMLElement* element, const char* att)
{
	uint x = 0;
	Misc::intAtt(element, att, &x);
	return x;
}

bool Misc::intChild(XMLElement* element, const char* node, uint* val)
{
	XMLElement* child = element->FirstChildElement(node);
	if (child == NULL)
		return false;
	*val = ::atoi(child->GetText());
	return true;
}

uint Misc::intChild(XMLElement* element, const char* node)
{
	uint x = 0;
	Misc::intChild(element, node, &x);
	return x;
}

bool Misc::hexOrInt(XMLElement* element, const char* att, uint* val)
{
	const char* str = element->Attribute(att);
	if (str == NULL)
		return false;
	if (::strstr(str, "0x") != NULL) 
	{
		int len = ::strlen(str);
		if (len > 10 || len < 3) 
			return false;
		len -= 2;
		string x = (str + 2);
		if (len == 1)
			x = "0000000" + x;
		else if (len == 2)
			x = "000000" + x;
		else if (len == 3)
			x = "00000" + x;
		else if (len == 4)
			x = "0000" + x;
		else if (len == 5)
			x = "000" + x;
		else if (len == 6)
			x = "00" + x;
		else if (len == 7)
			x = "0" + x;
		*val = Net::hexStr2int((char*) x.c_str());
		return true;
	}
	*val = ::atoi(str);
	return true;
}

uint Misc::hexOrInt(XMLElement* element, const char* att)
{
	uint x = 0;
	Misc::hexOrInt(element, att, &x);
	return x;
}

bool Misc::floatAtt(XMLElement* element, const char* att, float* val)
{
	const char* x = element->Attribute(att);
	if (x == NULL)
		return false;
	*val = ::atof(x);
	return true;
}

float Misc::floatAtt(XMLElement* element, const char* att)
{
	float x = 0;
	Misc::floatAtt(element, att, &x);
	return x;
}

Misc::~Misc()
{

}
