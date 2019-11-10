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

#ifndef MISC_H_
#define MISC_H_

#if !defined (__LIBMISC_H__) && !defined (LIBMISC)
#error only libmisc.h can be included directly.
#endif

#include "../macro.h"
#include "tinyxml2.h"
using namespace tinyxml2;

class Misc
{
public:
	static void sleep(int mec); 
	static void hold(); 
	static int asprintf(char **strp, const char *fmt, ...); 
	static string getStack(); 
	static void lazyExit(); 
	static uint getTid(); 
	static size_t hash(const string& str); 
	static bool isGreat(ullong a, ullong b); 
	static bool isGreatOrEq(ullong a, ullong b); 
	static bool checkNameFormat(const string& name, int min, int max); 
	static bool checkPwdFormat(const string& name, int min, int max); 
	static bool checkEmailFormat(const string& email, int max); 
	static bool checkRegex(const char* str, const char* pattern); 
	static bool checkUtf8str(const string& role, int min, int max); 
	static bool isAlphaString(const char* str); 
	static bool isDigitString(const char* str); 
	static bool isAlpDiString(const char* str); 
	static bool isAlpDiStr(string* str); 
	static bool isVisAdString(const char* str); 
	static bool isVisAdStr(string* str); 
	static bool startWith(const string& str, const string& sub); 
	static bool endWith(const string& str, const string& sub); 
	static void split(const string& str, const string& key, vector<string>& arr, int num = 0 ); 
	static string replaceAll(string& str, const string& org, const string& des); 
	static string intArr2sqlStr(const vector<int>& arr); 
	static string strArr2sqlStr(const vector<string>& arr); 
	static string toUpercase(const string& str); 
	static string toLowercase(const string& str); 
	static list<string> getEnvs(); 
	static string getEnvStr(const char* key, const char* def = NULL); 
	static int getEnvInt(const char* key, int def = 0);
	static string getSetEnvStr(const char* key, const char* def = NULL); 
	static int getSetEnvInt(const char* key, int def = 0); 
	static void setEnv(const char* key, const char* val); 
	static const char* pathSeparator(); 
	static int getFileAtt(char* path, char* name, ullong* size); 
	static void listFiles(const char* path, list<string>* files); 
	static string loadFile(const char* file); 
	static bool mkdir(const string& dir); 
	static bool isDirExisted(const string& dir); 
	static int readAll(int fd, uchar* buf, int len); 
	static bool strAtt(XMLElement* element, const char* att, string* val); 
	static string strAtt(XMLElement* element, const char* att); 
	static bool strChild(XMLElement* element, const char* node, string* val); 
	static string strChild(XMLElement* element, const char* node); 
	static bool intAtt(XMLElement* element, const char* att, uint* val); 
	static uint intAtt(XMLElement* element, const char* att); 
	static bool intChild(XMLElement* element, const char* node, uint* val); 
	static uint intChild(XMLElement* element, const char* node); 
	static bool hexOrInt(XMLElement* element, const char* att, uint* val); 
	static uint hexOrInt(XMLElement* element, const char* att); 
	static bool floatAtt(XMLElement* element, const char* att, float* val); 
	static float floatAtt(XMLElement* element, const char* att); 
private:
	Misc();
	virtual ~Misc();
};

#endif 
