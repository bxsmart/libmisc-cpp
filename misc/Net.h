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

#ifndef NET_H_
#define NET_H_

#if !defined (__LIBMISC_H__) && !defined (LIBMISC)
#error only libmisc.h can be included directly.
#endif

#include "../macro.h"

class Net
{
public:
	static void ip2str(uint ip, char* str); 
	static string ip2str(uint ip); 
	static bool str2ipAndPort(const char* str, string* ip, int* port); 
	static bool isDomain(const char* host); 
	static void sockaddr2str(struct sockaddr_in* addr, char* str); 
	static string sockaddr2str(struct sockaddr_in* addr); 
	static void setAddr(const char* host, int port, struct sockaddr_in* addr); 
	static string getHostByName(const char* domain); 
	static bool takeDomainPortPath(const char* url, string* domain, int* port, string* path); 
	static void splitUrl(const char* url, unordered_map<string, string>* kvs); 
	static string getHttpSuffixName(const char* url); 
	static void close(int sock); 
	static int tcpConnect(const char* host, ushort port); 
	static int tcpConnectNoBlocking(const char* host, ushort port, uint sec , uint rcvbuf = 512 * 1024, uint sndbuf = 512 * 1024); 
	static int tcpListen(const char* host, ushort port); 
	static int tcpAccept(int sock, struct sockaddr* peer); 
	static int tcpRecv(int sock, uchar* buff, int size); 
	static int tcpRecvNoBlocking(int sock, uchar* buff, int size); 
	static bool tcpRecvN(int sock, uchar* buff, int size); 
	static int tcpSend(int sock, uchar* buff, int size); 
	static int udpBind(const char* host, ushort port, struct sockaddr_in* addr); 
	static int udpBindNoBlocking(const char* host, ushort port, struct sockaddr_in* addr); 
	static int udpBindBroadCast(const char* host, ushort port, struct sockaddr_in* addr); 
	static int udpRecv(int sock, uchar* buff, int size, struct sockaddr_in* from); 
	static int udpSend(int sock, uchar* buff, int size, struct sockaddr_in* to); 
	static bool setNoBlocking(int sock); 
	static bool setLinger(int sock); 
	static bool setSNDBUF(int sock, int size); 
	static bool setRCVBUF(int sock, int size); 
	static bool setSNDTIMEO(int sock, int sec); 
	static bool setRCVTIMEO(int sock, int sec); 
	static bool setNODELAY(int sock); 
	static void bigEndianInt(uint x, uchar* dat); 
	static void litEndianInt(uint x, uchar* dat); 
	static ushort getShort(uchar* dat); 
	static uint getInt(uchar* dat); 
	static ullong getUllong(uchar* dat); 
	static ullong bcd2llong(uchar* dat, int len); 
	static void reversal(uchar* dat, int len); 
#if !defined(MAC)
	static ullong htonll(ullong h); 
	static ullong ntohll(ullong n); 
#endif
	static bool isHexStr(const uchar* dat, int len); 
	static void hex2strUperCaseSpace(const uchar* dat, int len, char* str); 
	static string hex2strUperCaseSpace(const uchar* dat, int len); 
	static void hex2strUperCase(const uchar* dat, int len, char* str); 
	static string hex2strUperCase(const uchar* dat, int len); 
	static string hex2strUperCase(const string& str); 
	static void hex2strLowerCase(const uchar* dat, int len, char* str); 
	static string hex2strLowerCase(const uchar* dat, int len); 
	static uchar hexStr2byte(const char* hex); 
	static ushort hexStr2short(const char* hex); 
	static uint hexStr2int(const char* hex); 
	static void hexStr2bytes(const char* hex, int len, uchar* dat); 
	static string hexStr2bytes(const char* hex, int len); 
	static string hexStr2bytesSpace(const char* hex, int len); 
	static string hexStr2bytesSpace(const string& str); 
	static uint hexOrint(const char* hex); 
	static void int2hexStr(uint i, char* str); 
	static void printHex(const uchar* dat, int len); 
	static void printHex2str(const uchar* dat, int len, char* str); 
	static string printHex2str(const uchar* dat, int len); 
	static void printInt2binStr(uint v, char* str); 
	static void printLong2binStr(ullong v, char* str); 
	static string itoa(int v);
	static string ltoa(long long int v); 
	static char b2c(uchar chr); 
private:
	Net();
	virtual ~Net();
};

#endif 
