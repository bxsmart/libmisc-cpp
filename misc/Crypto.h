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

#ifndef MISC_CRYPTO_H_
#define MISC_CRYPTO_H_

#if !defined (__LIBMISC_H__) && !defined (LIBMISC)
#error only libmisc.h can be included directly.
#endif

#include "../macro.h"

class Crypto
{
public:
	static string base64enc(const uchar* dat, int len); 
	static string base64enc(const string& dat); 
	static string base64dec(const char* str); 
	static string base64dec(const string& str); 
	static void bcd2str(uchar* dat, int len, char* str); 
	static void mi2str(uchar* mi, int len, char* str); 
	static void apn2str(uchar* apn, int len, char* str); 
	static void cid2str(uchar* cell, int len, char* str); 
	static string aes128enc(const string& key , const string& org); 
	static string aes128enc2hexStrLowerCase(const string& key , const string& org); 
	static string aes128dec(const string& key , const string& des); 
	static void rc4enc(uchar* key, int klen, uchar* in, int ilen, uchar* out); 
	static void rc4dec(uchar* key, int klen, uchar* in, int ilen, uchar* out); 
	static void md5(uchar* in, int ilen, uchar* out); 
	static string md5ToHexStrLowerCase(uchar* in, int ilen); 
	static void sha1(uchar* in, int ilen, uchar* out); 
	static void sha256(uchar* in, int ilen, uchar* out); 
	static string sha256ToHexStrUperCase(uchar* in, int ilen); 
	static string sha256ToHexStrLowerCase(uchar* in, int ilen); 
	static string sha256ToHexStrLowerCase(const string& str); 
	static void* sha256init(); 
	static void sha256update(void* sc, uchar* in, int ilen); 
	static void sha256final(void* sc, uchar* out); 
	static string sha256final2hexStrLowerCase(void* sc); 
	static void sha512(uchar* in, int ilen, uchar* out); 
	static string sha512ToHexStrUperCase(uchar* in, int ilen); 
	static void srand(); 
	static uint randomInt(); 
	static ullong randomUllong(); 
	static void random32(uchar* r32); 
	static void random64(uchar* r64); 
	static void random128(uchar* r128); 
	static void random256(uchar* r256); 
	static void random512(uchar* r512); 
	static void gen0aAkey32(char* out); 
	static string gen0aAkey32(); 
	static void gen0aAkey64(char* out); 
	static string gen0aAkey64(); 
	static void gen0aAkey128(char* out); 
	static string gen0aAkey128(); 
	static void gen0aAkey256(char* out); 
	static string gen0aAkey256(); 
	static void gen0aAkey512(char* out); 
	static string gen0aAkey512(); 
private:
	Crypto();
	virtual ~Crypto();
};

#endif 
