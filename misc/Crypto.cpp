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

#include "Crypto.h"
#include "Net.h"
#include "DateMisc.h"
#include "Log.h"
#ifdef USE_OPENSSL
#include <openssl/rc4.h>
#include <openssl/sha.h>
#include <openssl/md5.h>
#include <openssl/ssl.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#else
#include "../sec/rc4.h"
#include "../sec/md5.h"
#include "../sec/sha256.h"
#include "../sec/sha512.h"
#include "../sec/sha1.h"
#endif
#include "../sec/Base64.h"
#include "../sec/aes.h"

static char __0aA__[] = { '_', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z' };

Crypto::Crypto()
{

}

string Crypto::base64enc(const uchar* dat, int len)
{
	char* buf = Base64::encode(dat, len);
	string str = string(buf);
	::free(buf);
	return str;
}

string Crypto::base64enc(const string& dat)
{
	return Crypto::base64enc((uchar*) dat.data(), dat.length());
}

string Crypto::base64dec(const char* str)
{
	int len = 0;
	uchar* buf = Base64::decode(str, &len);
	string dat;
	dat.assign((const char*) buf, len);
	::free(buf);
	return dat;
}

string Crypto::base64dec(const string& str)
{
	return Crypto::base64dec(str.c_str());
}

void Crypto::bcd2str(uchar* dat, int len, char* str)
{
	int index = 0;
	for (int i = 0; i < len; ++i)
	{
		uchar bit = (dat[i] & 0xF);
		if (bit > 0x09)
			break;
		str[index++] = (bit + 0x30);
		bit = ((dat[i] >> 0x04) & 0xF);
		if (bit > 0x09)
			break;
		str[index++] = (bit + 0x30);
	}
}

void Crypto::mi2str(uchar* mi, int len, char* str)
{
	uchar toi = mi[0] & 0x07;
	if (toi == 0x01 || toi == 0x02 || toi == 0x03)
	{
		str[0] = ((mi[0] >> 4) & 0x0F) + 0x30;
		for (int i = 1; i < len; ++i)
		{
			uchar low = (mi[i] & 0x0F);
			uchar hig = ((mi[i] >> 0x04) & 0x0F);
			if (hig > 0x09)
			{
				str[i * 2 - 1] = low + 0x30;
				return;
			}
			str[i * 2 - 1] = low + 0x30;
			str[i * 2] = hig + 0x30;
		}
		return;
	}
}

void Crypto::apn2str(uchar* apn, int len, char* str)
{
	if (apn[0] >= len)
		return;
	::memcpy(str + ::strlen(str), apn + 1, apn[0]);
	if (len - 1 > apn[0])
	{
		::sprintf(str + ::strlen(str), "%c", '.');
		Crypto::apn2str(apn + apn[0] + 1, len - apn[0] - 1, str);
	}
}

void Crypto::cid2str(uchar* cell, int len, char* str)
{
	short idx = 0, j = 0;
	ushort tmp = 0;
	for (; idx < len; idx++)
	{
		if (idx <= 2)
		{
			str[j++] = (cell[idx] & 0x0F) + '0';
			str[j++] = ((cell[idx] & 0xF0) >> 4) + '0';
		} else
		{
			str[j++] = '-';
			tmp = ((cell[idx] & 0xFF) << 8) + (cell[idx + 1] & 0xFF);
			idx++;
			sprintf(str + j, "%d", (tmp));
			j += strlen(str + j);
		}
	}
	str[3] = '-';
	str[j] = 0;
}

string Crypto::aes128enc(const string& key , const string& org)
{
	if (key.length() < 0x10)
		return "";
	int len = org.length();
	int mo = (16 - len % 16);
	int blen = len + mo;
	AES_ctx ctx;
	AES_init_ctx(&ctx, (uchar*) key.data());
	uchar* buff = (uchar*) malloc(blen);
	memcpy(buff, org.data(), len);
	for (int n = 0; n < mo; ++n)
		buff[len + n] = mo;
	for (int i = 0; i < blen; i += 16)
		AES_ECB_encrypt(&ctx, (uchar*) buff + i);
	string des((const char*) buff, blen);
	::free(buff);
	return des;
}

string Crypto::aes128enc2hexStrLowerCase(const string& key , const string& org)
{
	if (key.length() < 0x10)
		return "";
	string str = Crypto::aes128enc(key, org);
	return Net::hex2strLowerCase((uchar*) str.data(), str.length());
}

string Crypto::aes128dec(const string& key , const string& des)
{
	if (key.length() < 0x10)
		return "";
	int len = des.length();
	AES_ctx ctx;
	AES_init_ctx(&ctx, (uchar*) key.c_str());
	uchar* buff = (uchar*) ::calloc(1, len + (16 - (len % 16)));
	::memcpy(buff, des.data(), len);
	for (int i = 0; i < len; i += 16)
		AES_ECB_decrypt(&ctx, (uchar*) buff + i);
	int rlen = len - (buff[len - 1] == 0x01 ? 0 : buff[len - 1]);
	if (rlen < 1)
	{
		::free(buff);
		return "";
	}
	string org((const char*) buff, rlen);
	::free(buff);
	return org;
}

void Crypto::rc4enc(uchar* key, int klen, uchar* in, int ilen, uchar* out)
{
#ifdef USE_OPENSSL
	RC4_KEY k;
	::RC4_set_key(&k, klen, key);
	::RC4(&k, ilen, in, out);
#else
	struct rc4_state ks;
	::rc4_init(&ks, key, klen);
	::rc4_crypt(&ks, in, out, ilen);
#endif
}

void Crypto::rc4dec(uchar* key, int klen, uchar* in, int ilen, uchar* out)
{
	Crypto::rc4enc(key, klen, in, ilen, out);
}

void Crypto::md5(uchar* in, int ilen, uchar* out)
{
	MD5_CTX ctx;
	MD5_Init(&ctx);
	MD5_Update(&ctx, in, ilen);
	MD5_Final(out, &ctx);
}

string Crypto::md5ToHexStrLowerCase(uchar* in, int ilen)
{
	uchar dat[0x10];
	Crypto::md5(in, ilen, dat);
	char str[0x21];
	Net::hex2strLowerCase(dat, 0x10, str);
	str[0x20] = 0;
	return string(str);
}

void Crypto::sha1(uchar* in, int ilen, uchar* out)
{
#ifdef USE_OPENSSL
	::SHA1(in, ilen, out);
#else
	SHA1(in, ilen, out);
#endif
}

void Crypto::sha256(uchar* in, int ilen, uchar* out)
{
#ifdef USE_OPENSSL
	::SHA256(in, ilen, out);
#else
	SHA256_CTX sc;
	sha256_init(&sc);
	sha256_update(&sc, in, ilen);
	sha256_final(&sc, out);
#endif
}

string Crypto::sha256ToHexStrUperCase(uchar* in, int ilen)
{
	uchar dat[0x20];
	Crypto::sha256(in, ilen, dat);
	char str[0x41];
	Net::hex2strUperCase(dat, 0x20, str);
	str[0x40] = 0;
	return string(str);
}

string Crypto::sha256ToHexStrLowerCase(uchar* in, int ilen)
{
	uchar dat[0x20];
	Crypto::sha256(in, ilen, dat);
	char str[0x41];
	Net::hex2strLowerCase(dat, 0x20, str);
	str[0x40] = 0;
	return string(str);
}

string Crypto::sha256ToHexStrLowerCase(const string& str)
{
	return Crypto::sha256ToHexStrLowerCase((uchar*) str.data(), str.length());
}

void* Crypto::sha256init()
{
	SHA256_CTX* sc = (SHA256_CTX*) ::malloc(sizeof(SHA256_CTX));
	sha256_init(sc);
	return sc;
}

void Crypto::sha256update(void* sc, uchar* in, int ilen)
{
	sha256_update((SHA256_CTX*) sc, in, ilen);
}

void Crypto::sha256final(void* sc, uchar* out)
{
	sha256_final((SHA256_CTX*) sc, out);
	::free(sc);
}

string Crypto::sha256final2hexStrLowerCase(void* sc)
{
	uchar dat[0x20];
	Crypto::sha256final((SHA256_CTX*) sc, dat);
	char str[0x41];
	Net::hex2strLowerCase(dat, 0x20, str);
	str[0x40] = 0;
	return string(str);
}

void Crypto::sha512(uchar* in, int ilen, uchar* out)
{
#ifdef USE_OPENSSL
	::SHA512(in, ilen, out);
#else
	::sha_512(in, ilen, out, 0);
#endif
}

string Crypto::sha512ToHexStrUperCase(uchar* in, int ilen)
{
	uchar dat[0x40];
	Crypto::sha512(in, ilen, dat);
	char str[0x81];
	Net::hex2strUperCase(dat, 0x40, str);
	str[0x80] = 0;
	return string(str);
}

void Crypto::srand()
{
	static atomic_int seed((int) (DateMisc::now()));
	::srand(seed.fetch_add(1));
}

uint Crypto::randomInt()
{
#ifdef LINUX
	static int fd = ::open("/dev/urandom", O_RDONLY | O_NONBLOCK);
	uint x;
	::read(fd, &x, sizeof(uint));
	return x;
#else
	Crypto::srand();
	return (((::rand() & 0xFFFF) << 16) | (::rand() & 0xFFFF)) * (((::rand() & 0xFFFF) << 16) | (::rand() & 0xFFFF));
#endif
}

ullong Crypto::randomUllong()
{
	ullong x;
	Crypto::random64((uchar*) &x);
	return x;
}

void Crypto::random32(uchar* r32)
{
	uint x = Crypto::randomInt();
	::memcpy(r32, &x, 4);
}

void Crypto::random64(uchar* r64)
{
	Crypto::random32(r64);
	Crypto::random32(r64 + 0x04);
}

void Crypto::random128(uchar* r128)
{
	Crypto::random64(r128);
	Crypto::random64(r128 + 0x08);
}

void Crypto::random256(uchar* r256)
{
	Crypto::random128(r256);
	Crypto::random128(r256 + 0x10);
}

void Crypto::random512(uchar* r512)
{
	Crypto::random256(r512);
	Crypto::random256(r512 + 0x20);
}

void Crypto::gen0aAkey32(char* out)
{
	uint x = Crypto::randomInt();
	out[0] = __0aA__[(x) % sizeof(__0aA__)];
	out[1] = __0aA__[(x >> 0x08) % sizeof(__0aA__)];
	out[2] = __0aA__[(x >> 0x10) % sizeof(__0aA__)];
	out[3] = __0aA__[(x >> 0x18) % sizeof(__0aA__)];
}

string Crypto::gen0aAkey32()
{
	char s[0x05];
	Crypto::gen0aAkey32(s);
	s[0x04] = 0;
	return string(s);
}

void Crypto::gen0aAkey64(char* out)
{
	Crypto::gen0aAkey32(out);
	Crypto::gen0aAkey32(out + 0x04);
}

string Crypto::gen0aAkey64()
{
	char s[0x09];
	Crypto::gen0aAkey64(s);
	s[0x08] = 0;
	return string(s);
}

void Crypto::gen0aAkey128(char* out)
{
	Crypto::gen0aAkey64(out);
	Crypto::gen0aAkey64(out + 0x08);
}

string Crypto::gen0aAkey128()
{
	char s[0x11];
	Crypto::gen0aAkey128(s);
	s[0x10] = 0;
	return string(s);
}

void Crypto::gen0aAkey256(char* out)
{
	Crypto::gen0aAkey128(out);
	Crypto::gen0aAkey128(out + 0x10);
}

string Crypto::gen0aAkey256()
{
	char s[0x21];
	Crypto::gen0aAkey256(s);
	s[0x20] = 0;
	return string(s);
}

void Crypto::gen0aAkey512(char* out)
{
	Crypto::gen0aAkey256(out);
	Crypto::gen0aAkey256(out + 0x20);
}

string Crypto::gen0aAkey512()
{
	char s[0x41];
	Crypto::gen0aAkey512(s);
	s[0x40] = 0;
	return string(s);
}

Crypto::~Crypto()
{

}

