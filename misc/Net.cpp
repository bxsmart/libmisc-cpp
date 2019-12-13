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

#include "Net.h"
#include "Log.h"
#include "Misc.h"

static char __0F__[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
static char __0f__[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

Net::Net()
{

}

void Net::ip2str(uint ip, char* str)
{
	::sprintf(str, "%d.%d.%d.%d", (ip >> 24 & 0xFF), (ip >> 16 & 0xFF), (ip >> 8 & 0xFF), (ip & 0xFF));
}

string Net::ip2str(uint ip)
{
	string str;
	SPRINTF_STRING(&str, "%d.%d.%d.%d", (ip >> 24 & 0xFF), (ip >> 16 & 0xFF), (ip >> 8 & 0xFF), (ip & 0xFF))
	return str;
}

bool Net::str2ipAndPort(const char* str, string* ip, int* port)
{
	vector<string> vec;
	Misc::split(str, ":", vec);
	if (vec.size() != 2)
		return false;
	*ip = vec.at(0);
	*port = ::atoi(vec.at(1).c_str());
	return true;
}

bool Net::isDomain(const char* host)
{
	for (size_t i = 0; i < ::strlen(host); ++i)
	{
		if (host[i] == '.')
			continue;
		if (host[i] < '0' || host[i] > '9')
			return true;
	}
	return false;
}

void Net::sockaddr2str(struct sockaddr_in* addr, char* str)
{
	Net::ip2str(ntohl(addr->sin_addr.s_addr), str);
	::sprintf(str + ::strlen(str), ":%d", ntohs(addr->sin_port));
}

string Net::sockaddr2str(struct sockaddr_in* addr)
{
	string str;
	uint ip = ntohl(addr->sin_addr.s_addr);
	SPRINTF_STRING(&str, "%d.%d.%d.%d:%d", (ip >> 24 & 0xFF), (ip >> 16 & 0xFF), (ip >> 8 & 0xFF), (ip & 0xFF), ntohs(addr->sin_port))
	return str;
}

void Net::setAddr(const char* host, int port, struct sockaddr_in* addr)
{
	addr->sin_family = AF_INET;
	addr->sin_port = htons(port);
	addr->sin_addr.s_addr = host == NULL ? htonl(INADDR_ANY) : ::inet_addr(host);
}

string Net::getHostByName(const char* domain)
{
	struct hostent* host = ::gethostbyname(domain);
	string str = "";
	if (host != NULL && host->h_addr_list[0] != NULL)
	{
		struct in_addr addr;
		addr.s_addr = *((uint*) (host->h_addr_list[0]));
		char* x = ::inet_ntoa(addr);
		SPRINTF_STRING(&str, "%s", x)
	}
	return str;
}

bool Net::takeDomainPortPath(const char* url, string* domain, int* port, string* path)
{
	string httpUrl(url);
	int indx = httpUrl.find("http://");
	if (indx == -1)
		return false;
	httpUrl = httpUrl.substr(indx + 7, httpUrl.length());
	indx = httpUrl.find("/");
	if (indx == -1) 
		return false;
	*path = httpUrl.substr(indx);
	httpUrl = httpUrl.substr(0, indx);
	indx = httpUrl.find(":");
	*domain = ((int) indx == -1 ? httpUrl : httpUrl.substr(0, indx));
	*port = ::atoi(((int) indx == -1 ? "80" : httpUrl.substr(indx + 1, httpUrl.length()).c_str()));
	return true;
}

void Net::splitUrl(const char* url, unordered_map<string, string>* kvs)
{
	vector<string> vec;
	Misc::split(url, "&", vec);
	if (vec.size() < 1)
		return;
	for (vector<string>::iterator it = vec.begin(); it != vec.end(); ++it)
	{
		vector<string> kv;
		Misc::split(it->c_str(), "=", kv);
		if (kv.size() != 2)
			continue;
		(*kvs)[kv[0]] = kv[1];
	}
}

string Net::getHttpSuffixName(const char* url)
{
	vector<string> item;
	Misc::split(url, "/", item);
	return item.empty() ? "" : item.at(item.size() - 1);
}

void Net::close(int sock)
{
#ifdef WINDOWS
	::closesocket(sock);
#else
#ifdef ANDROID	
	::shutdown(sock, SHUT_RDWR);
#endif
	::close(sock);
#endif
}

int Net::tcpConnect(const char* host, ushort port)
{
	if (host == NULL || port == 0)
		return -1;
	int sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	struct sockaddr_in addr;
	Net::setAddr(host, port, &addr);
	if (::connect(sock, (struct sockaddr*) &addr, sizeof(struct sockaddr)) == 0)
		return sock;
	Net::close(sock);
	return -1;
}

int Net::tcpConnectNoBlocking(const char* host, ushort port, uint sec , uint rcvbuf, uint sndbuf)
{
	int sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	Net::setNoBlocking(sock);
	Net::setRCVBUF(sock, rcvbuf);
	Net::setSNDBUF(sock, sndbuf);
	Net::setLinger(sock);
	Net::setNODELAY(sock);
	struct sockaddr_in addr;
	Net::setAddr(host, port, &addr);
	int r = ::connect(sock, (struct sockaddr*) &addr, sizeof(struct sockaddr));
	if (r == 0) 
		return sock;
#ifdef WINDOWS
	if (WSAGetLastError() != WSAEWOULDBLOCK)
	goto label;
#else
	if (errno != EINPROGRESS) 
		goto label;
#endif
	struct timeval tv;
	tv.tv_sec = sec;
	tv.tv_usec = 0;
	fd_set wset;
	FD_ZERO(&wset);
	FD_SET(sock, &wset);
	r = ::select(sock + 1, NULL, &wset, NULL, &tv);
	if (r <= 0)
		goto label;
	if (FD_ISSET(sock, &wset))
	{
		int err;
		socklen_t len = sizeof(int);
		if (::getsockopt(sock, SOL_SOCKET, SO_ERROR, (char* ) &err, &len) < 0 || err)
			goto label;
	} else
	{
		label: 
		Net::close(sock);
		return -1;
	}
	return sock;
}

int Net::tcpListen(const char* host, ushort port)
{
	int sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	int x = SO_REUSEADDR;
	if (::setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*) &x, sizeof(int)) != 0)
	{
		Net::close(sock);
		return -1;
	}
#ifdef SO_REUSEPORT
	x = SO_REUSEPORT;
	if (::setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, (char*) &x, sizeof(int)) != 0)
	{
		Net::close(sock);
		return -1;
	}
#endif
	struct sockaddr_in addr;
	Net::setAddr(host, port, &addr);
	if (::bind(sock, (struct sockaddr*) &addr, sizeof(struct sockaddr)) == 0 && ::listen(sock, 0xFFFF) == 0)
		return sock;
	Net::close(sock);
	return -1;
}

int Net::tcpAccept(int sock, struct sockaddr* peer)
{
	static socklen_t x = sizeof(struct sockaddr);
	return ::accept(sock, peer, &x);
}

int Net::tcpRecv(int sock, uchar* buff, int size)
{
	return ::recv(sock, (char*) buff, size, 0);
}

int Net::tcpRecvNoBlocking(int sock, uchar* buff, int size)
{
#ifndef WINDOWS
	return ::recv(sock, buff, size, MSG_DONTWAIT);
#else
	return ::recv(sock, (char*) buff, size, 0);
#endif
}

bool Net::tcpRecvN(int sock, uchar* buff, int size)
{
	if (size == 0)
		return false;
	int x = 0;
	while (1)
	{
		int len = ::recv(sock, (char*) buff + x, size - x, 0);
		if (len < 1)
			return false;
		x += len;
		if (x >= size)
			return true;
	}
	return false;
}

int Net::tcpSend(int sock, uchar* buff, int size)
{
	return ::send(sock, (char*) buff, size, 0);
}

int Net::udpBind(const char* host, ushort port, struct sockaddr_in* addr, int buf)
{
	int sock = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	int x = SO_REUSEADDR;
	if (::setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*) &x, sizeof(int)) != 0)
	{
		Net::close(sock);
		return -1;
	}
#ifdef SO_REUSEPORT
	x = SO_REUSEPORT;
	if (::setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, (char*) &x, sizeof(int)) != 0)
	{
		Net::close(sock);
		return -1;
	}
#endif
	x = buf * 1472;
	Net::setRCVBUF(sock, x);
	Net::setSNDBUF(sock, x);
	if (addr == NULL)
	{
		struct sockaddr_in xr;
		Net::setAddr(host, port, &xr);
		if (::bind(sock, (struct sockaddr*) &xr, sizeof(struct sockaddr)) != 0)
		{
			Net::close(sock);
			return -1;
		}
	} else
	{
		Net::setAddr(host, port, addr);
		if (::bind(sock, (struct sockaddr*) addr, sizeof(struct sockaddr)) != 0)
		{
			Net::close(sock);
			return -1;
		}
	}
	return sock;
}

int Net::udpBindNoBlocking(const char* host, ushort port, struct sockaddr_in* addr, int buf)
{
	int sock = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	int x = SO_REUSEADDR;
	if (::setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*) &x, sizeof(int)) != 0)
	{
		Net::close(sock);
		return -1;
	}
#ifdef SO_REUSEPORT
	x = SO_REUSEPORT;
	if (::setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, (char*) &x, sizeof(int)) != 0)
	{
		Net::close(sock);
		return -1;
	}
#endif
	if (!Net::setNoBlocking(sock))
	{
		Net::close(sock);
		return -1;
	}
	x = buf * 1472;
	Net::setRCVBUF(sock, x);
	Net::setSNDBUF(sock, x);
	if (addr == NULL)
	{
		struct sockaddr_in xr;
		Net::setAddr(host, port, &xr);
		if (::bind(sock, (struct sockaddr*) &xr, sizeof(struct sockaddr)) != 0)
		{
			Net::close(sock);
			return -1;
		}
	} else
	{
		Net::setAddr(host, port, addr);
		if (::bind(sock, (struct sockaddr*) addr, sizeof(struct sockaddr)) != 0)
		{
			Net::close(sock);
			return -1;
		}
	}
	return sock;
}

int Net::udpBindBroadCast(const char* host, ushort port, struct sockaddr_in* addr, int buf)
{
	int sock = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	int x = SO_REUSEADDR;
#if	!defined(MAC)
	if (::setsockopt(sock, SOL_SOCKET, SO_BROADCAST | SO_REUSEADDR, (char*) &x, sizeof(int)) != 0)
	{
		Net::close(sock);
		return -1;
	}
#endif
#ifdef MAC
	if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char*) &x, sizeof(int)) != 0)
	{
		Net::close(sock);
		return -1;
	}
#endif
	x = buf * 1472;
	Net::setRCVBUF(sock, x);
	Net::setSNDBUF(sock, x);
	if (addr == NULL)
	{
		struct sockaddr_in xr;
		Net::setAddr(host, port, &xr);
		if (::bind(sock, (struct sockaddr*) &xr, sizeof(struct sockaddr)) != 0)
		{
			Net::close(sock);
			return -1;
		}
	} else
	{
		Net::setAddr(host, port, addr);
		if (::bind(sock, (struct sockaddr*) addr, sizeof(struct sockaddr)) != 0)
		{
			Net::close(sock);
			return -1;
		}
	}
	return sock;
}

int Net::udpRecv(int sock, uchar* buff, int size, struct sockaddr_in* from)
{
	static socklen_t x = sizeof(struct sockaddr);
	return ::recvfrom(sock, (char*) buff, size, 0, (struct sockaddr*) from, &x);
}

int Net::udpSend(int sock, uchar* buff, int size, struct sockaddr_in* to)
{
	return ::sendto(sock, (char*) buff, size, 0, (struct sockaddr*) to, sizeof(struct sockaddr));
}

bool Net::setNoBlocking(int sock)
{
#ifdef WINDOWS
	u_long mode = 1;
	ioctlsocket(sock, FIONBIO, &mode);
#else
	int opt = ::fcntl(sock, F_GETFL);
	if (opt < 0)
	{
		LOG_ERROR("get FL failed, errno: %d", errno)
		return false;
	}
	opt |= O_NONBLOCK;
	if (::fcntl(sock, F_SETFL, opt) < 0)
	{
		LOG_ERROR("set FL failed, errno: %d", errno)
		return false;
	}
#endif
	return true;
}

bool Net::setLinger(int sock)
{
	struct linger ling = { 1, 0 };
	if (::setsockopt(sock, SOL_SOCKET, SO_LINGER, (char*) &ling, sizeof(struct linger)) != 0)
	{
#ifdef WINDOWS
		LOG_ERROR("set SO_LINGER opt failed, sock: %d, errno: %ld", sock, GetLastError())
#else
		LOG_ERROR("set SO_LINGER opt failed, sock: %d, errno: %d", sock, errno)
#endif
		return false;
	}
	return true;
}

bool Net::setSNDBUF(int sock, int size)
{
	if (::setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char*) &size, 4) != 0)
	{
#ifdef WINDOWS
		LOG_ERROR("set SO_SNDBUF opt failed, sock: %d, errno: %ld", sock, GetLastError())
#else
		LOG_ERROR("set SO_SNDBUF opt failed, sock: %d, errno: %d", sock, errno)
#endif
		return false;
	}
	return true;
}

bool Net::setRCVBUF(int sock, int size)
{
	if (::setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*) &size, 4) != 0)
	{
#ifdef WINDOWS
		LOG_ERROR("set SO_RCVBUF opt failed, sock: %d, errno: %ld", sock, GetLastError())
#else
		LOG_ERROR("set SO_RCVBUF opt failed, sock: %d, errno: %d", sock, errno)
#endif
		return false;
	}
	return true;
}

bool Net::setSNDTIMEO(int sock, int sec)
{
#ifdef WINDOWS
	int t = sec * 1000;
	if (::setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char *) &t, sizeof(int)) != 0)
	{
		LOG_ERROR("set SO_SNDTIMEO opt failed, sock: %d, errno: %ld", sock, GetLastError())
		return false;
	}
#else
	struct timeval tv;
	tv.tv_sec = sec;
	tv.tv_usec = 0;
	if (::setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char*) &tv, sizeof(struct timeval)) != 0)
	{
		LOG_ERROR("set SO_SNDTIMEO opt failed, sock: %d, errno: %d", sock, errno)
		return false;
	}
#endif
	return true;
}

bool Net::setRCVTIMEO(int sock, int ts)
{
#ifdef WINDOWS
	int t = ts * 1000;
	if (::setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *) &t, sizeof(int)) != 0)
	{
		LOG_ERROR("set SO_RCVTIMEO opt failed, sock: %d, errno: %ld", sock, GetLastError())
		return false;
	}
#else
	struct timeval tv;
	tv.tv_sec = ts;
	tv.tv_usec = 0;
	if (::setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*) &tv, sizeof(struct timeval)) != 0)
	{
		LOG_ERROR("set SO_RCVTIMEO opt failed, sock: %d, errno: %d", sock, errno)
		return false;
	}
#endif
	return true;
}

bool Net::setNODELAY(int sock)
{
#ifdef WINDOWS
	int x = 1;
	if (::setsockopt(sock, SOL_SOCKET, TCP_NODELAY, (char *) &x, sizeof(int)) != 0)
	{
		LOG_ERROR("set TCP_NODELAY opt failed, sock: %d, errno: %ld", sock, GetLastError())
		return false;
	}
#endif
#if defined(LINUX)
	int x = 1;
	if (::setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*) &x, sizeof(int)) != 0)
	{
		LOG_ERROR("set TCP_NODELAY opt failed, sock: %d, errno: %d", sock, errno)
		return false;
	}
#endif
	return true;
}

void Net::bigEndianInt(uint x, uchar* dat)
{
	dat[0] = (uchar) (x >> 24);
	dat[1] = (uchar) (x >> 16);
	dat[2] = (uchar) (x >> 8);
	dat[3] = (uchar) (x);
}

void Net::litEndianInt(uint x, uchar* dat)
{
	dat[0] = (uchar) (x);
	dat[1] = (uchar) (x >> 8);
	dat[2] = (uchar) (x >> 16);
	dat[3] = (uchar) (x >> 24);
}

ushort Net::getShort(uchar* dat)
{
	ushort val;
	::memcpy(&val, dat, 2);
	return ntohs(val);
}

uint Net::getInt(uchar* dat)
{
	uint val;
	::memcpy(&val, dat, 4);
	return ntohl(val);
}

ullong Net::getUllong(uchar* dat)
{
	ullong value = 0;
	for (int i = 0; i < 8; ++i)
		value += (ullong) ((ullong) (dat[i] & 0xFF)) << (8 * (8 - i - 1));
	return value;
}

ullong Net::bcd2llong(uchar* dat, int len)
{
	if (len > 9) 
		return 0L;
	uchar* by = (uchar*) ::calloc(1, len);
	::memcpy(by, dat, len);
	Net::reversal(by, len);
	ullong v = ::atol(Net::hex2strUperCase(by, len).c_str());
	::free(by);
	return v;
}

void Net::reversal(uchar* dat, int len)
{
	for (int i = 0; i < len / 2; ++i)
	{
		uchar tmp = dat[i];
		dat[i] = dat[len - 1 - i];
		dat[len - 1 - i] = tmp;
	}
}

#if !defined(MAC)
ullong Net::htonll(ullong h)
{
	ullong n = 0ULL;
	n |= ((h << 56) & 0xFF00000000000000ULL);
	n |= ((h << 40) & 0x00FF000000000000ULL);
	n |= ((h << 24) & 0x0000FF0000000000ULL);
	n |= ((h << 8) & 0x000000FF00000000ULL);
	n |= ((h >> 8) & 0x00000000FF000000ULL);
	n |= ((h >> 24) & 0x0000000000FF0000ULL);
	n |= ((h >> 40) & 0x000000000000FF00ULL);
	n |= ((h >> 56) & 0x00000000000000FFULL);
	return n;
}

ullong Net::ntohll(ullong n)
{
	return Net::htonll(n);
}
#endif

bool Net::isHexStr(const uchar* dat, int len)
{
	if (len < 1)
		return false;
	for (int i = 0; i < len; ++i)
	{
		bool ret = false;
		for (int k = 0; k < 0x10; ++k)
		{
			if (dat[i] == __0F__[k] || dat[i] == __0f__[k])
			{
				ret = true;
				break;
			}
		}
		if (!ret)
			return false;
	}
	return true;
}

void Net::hex2strUperCaseSpace(const uchar* dat, int len, char* str)
{
	int i = 0;
	for (; i < len; ++i)
	{
		str[i * 3 + 0] = __0F__[((dat[i] >> 4) & 0x0F)];
		str[i * 3 + 1] = __0F__[(dat[i] & 0x0F)];
		str[i * 3 + 2] = 0x20;
	}
	str[i * 3 - 1] = 0;
}

string Net::hex2strUperCaseSpace(const uchar* dat, int len)
{
	char* buf = (char*) ::calloc(1, len * 3 + 1);
	Net::hex2strUperCaseSpace(dat, len, buf);
	string str(buf);
	::free(buf);
	return str;
}

void Net::hex2strUperCase(const uchar* dat, int len, char* str)
{
	for (int i = 0; i < len; ++i)
	{
		str[i * 2 + 0] = __0F__[((dat[i] >> 4) & 0x0F)];
		str[i * 2 + 1] = __0F__[(dat[i] & 0x0F)];
	}
}

string Net::hex2strUperCase(const uchar* dat, int len)
{
	string str;
	str.resize(len * 2);
	Net::hex2strUperCase(dat, len, (char*) str.data());
	return str;
}

string Net::hex2strUperCase(const string& str)
{
	string x;
	x.resize(str.length() * 2);
	Net::hex2strUperCase((const uchar*) str.c_str(), str.length(), (char*) x.c_str());
	return str;
}

void Net::hex2strLowerCase(const uchar* dat, int len, char* str)
{
	for (int i = 0; i < len; ++i)
	{
		str[i * 2 + 0] = __0f__[((dat[i] >> 4) & 0x0F)];
		str[i * 2 + 1] = __0f__[(dat[i] & 0x0F)];
	}
}

string Net::hex2strLowerCase(const uchar* dat, int len)
{
	char* buf = (char*) ::calloc(1, len * 2 + 1);
	Net::hex2strLowerCase(dat, len, buf);
	string str = string(buf);
	::free(buf);
	return str;
}

uchar Net::hexStr2byte(const char* hex)
{
	return ((((hex[0] < 65 ? hex[0] - 0x30 : (hex[0] < 97 ? hex[0] - 55 : hex[0] - 87))) << 4) & 0xF0) | 
			(((hex[1] < 65 ? hex[1] - 0x30 : (hex[1] < 97 ? hex[1] - 55 : hex[1] - 87))) & 0x0F);
}

ushort Net::hexStr2short(const char* hex)
{
	ushort ret = 0x00;
	for (int i = 0; i < 4; ++i)
		ret |= (hex[i] < 65 ? hex[i] - 0x30 : (hex[i] < 97 ? hex[i] - 55 : hex[i] - 87)) << (3 - i) * 4;
	return ret;
}

uint Net::hexStr2int(const char* hex)
{
	uint ret = 0x00;
	for (int i = 0; i < 8; ++i)
		ret |= (hex[i] < 65 ? hex[i] - 0x30 : (hex[i] < 97 ? hex[i] - 55 : hex[i] - 87)) << (7 - i) * 4;
	return ret;
}

void Net::hexStr2bytes(const char* hex, int len, uchar* dat)
{
	for (int i = 0; i < len / 2; ++i)
		dat[i] = Net::hexStr2byte(hex + (i * 2));
}

string Net::hexStr2bytes(const char* hex, int len)
{
	string str;
	for (int i = 0; i < len / 2; ++i)
		str.push_back(Net::hexStr2byte(hex + (i * 2)));
	return str;
}

string Net::hexStr2bytesSpace(const char* hex, int len)
{
	return Net::hexStr2bytesSpace(string(hex, len));
}

string Net::hexStr2bytesSpace(const string& str)
{
	string x = str;
	string n = Misc::replaceAll(x, " ", "");
	return Net::hexStr2bytes(n.c_str(), n.size());
}

uint Net::hexOrint(const char* str)
{
	if (::strstr(str, "0x") != NULL) 
	{
		int len = ::strlen(str);
		if (len > 10 || len < 3) 
			return 0;
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
		return Net::hexStr2int((char*) x.c_str());
	}
	return ::atoi(str);
}

void Net::int2hexStr(uint i, char* str)
{
	str[0] = __0F__[((i >> 28) & 0x0F)];
	str[1] = __0F__[((i >> 24) & 0x0F)];
	str[2] = __0F__[((i >> 20) & 0x0F)];
	str[3] = __0F__[((i >> 16) & 0x0F)];
	str[4] = __0F__[((i >> 12) & 0x0F)];
	str[5] = __0F__[((i >> 8) & 0x0F)];
	str[6] = __0F__[((i >> 4) & 0x0F)];
	str[7] = __0F__[(i & 0x0F)];
}

void Net::printHex(const uchar* dat, int len)
{
	int rows = len / 16;
	int ac = len % 16;
	int i;
	for (i = 0; i < rows; ++i)
	{
		::printf("%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n", 
				dat[(16 * i) + 0],
				dat[(16 * i) + 1],
				dat[(16 * i) + 2],
				dat[(16 * i) + 3], 
				dat[(16 * i) + 4],
				dat[(16 * i) + 5],
				dat[(16 * i) + 6],
				dat[(16 * i) + 7], 
				dat[(16 * i) + 8],
				dat[(16 * i) + 9],
				dat[(16 * i) + 10],
				dat[(16 * i) + 11], 
				dat[(16 * i) + 12],
				dat[(16 * i) + 13],
				dat[(16 * i) + 14],
				dat[(16 * i) + 15], 
				Net::b2c(dat[(16 * i) + 0]),
				Net::b2c(dat[(16 * i) + 1]),
				Net::b2c(dat[(16 * i) + 2]),
				Net::b2c(dat[(16 * i) + 3]), 
				Net::b2c(dat[(16 * i) + 4]),
				Net::b2c(dat[(16 * i) + 5]),
				Net::b2c(dat[(16 * i) + 6]),
				Net::b2c(dat[(16 * i) + 7]), 
				Net::b2c(dat[(16 * i) + 8]),
				Net::b2c(dat[(16 * i) + 9]),
				Net::b2c(dat[(16 * i) + 10]),
				Net::b2c(dat[(16 * i) + 11]), 
				Net::b2c(dat[(16 * i) + 12]),
				Net::b2c(dat[(16 * i) + 13]),
				Net::b2c(dat[(16 * i) + 14]),
				Net::b2c(dat[(16 * i) + 15]));
	}
	for (i = 0; i < ac; i++)
		::printf("%02X ", dat[rows * 16 + i]);
	for (i = 0; ac > 0 && i < 16 - ac; i++)
		::printf("%s", "   ");
	for (i = 0; i < ac; i++)
		::printf("%c", Net::b2c(dat[rows * 16 + i]));
	::printf("\n");
}

void Net::printHex2str(const uchar* dat, int len, char* str)
{
	int rows = len / 16;
	int ac = len % 16;
	int i;
	for (i = 0; rows > 0 && i < rows; ++i)
	{
		::sprintf(str + 0x41 * i, "%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n", 
				dat[(16 * i) + 0],
				dat[(16 * i) + 1],
				dat[(16 * i) + 2],
				dat[(16 * i) + 3], 
				dat[(16 * i) + 4],
				dat[(16 * i) + 5],
				dat[(16 * i) + 6],
				dat[(16 * i) + 7],  
				dat[(16 * i) + 8],
				dat[(16 * i) + 9],
				dat[(16 * i) + 10],
				dat[(16 * i) + 11],  
				dat[(16 * i) + 12],
				dat[(16 * i) + 13],
				dat[(16 * i) + 14],
				dat[(16 * i) + 15],  
				Net::b2c(dat[(16 * i) + 0]),
				Net::b2c(dat[(16 * i) + 1]),
				Net::b2c(dat[(16 * i) + 2]),
				Net::b2c(dat[(16 * i) + 3]), 
				Net::b2c(dat[(16 * i) + 4]),
				Net::b2c(dat[(16 * i) + 5]),
				Net::b2c(dat[(16 * i) + 6]),
				Net::b2c(dat[(16 * i) + 7]), 
				Net::b2c(dat[(16 * i) + 8]),
				Net::b2c(dat[(16 * i) + 9]),
				Net::b2c(dat[(16 * i) + 10]),
				Net::b2c(dat[(16 * i) + 11]), 
				Net::b2c(dat[(16 * i) + 12]),
				Net::b2c(dat[(16 * i) + 13]),
				Net::b2c(dat[(16 * i) + 14]),
				Net::b2c(dat[(16 * i) + 15]));
	}
	int offset = 0x41 * rows;
	if (ac != 0)
	{
		for (i = 0; i < 0x30 + ac; ++i)
			str[offset + i] = ' ';
		for (i = 0; i < ac; ++i)
		{
			::sprintf(str + offset + (i * 3), "%02X ", dat[(rows * 16) + i]);
			::sprintf(str + offset + (0x30 + i), "%c", Net::b2c(dat[(rows * 16) + i]));
		}
		str[offset + (i * 3)] = 0x20;
		str[offset + (0x30 + i)] = '\n';
	}
}

string Net::printHex2str(const uchar* dat, int len)
{
	char* buf = (char*) ::calloc(1, (len / 16) * 65 + 65);
	Net::printHex2str(dat, len, buf);
	string str(buf);
	::free(buf);
	return str;
}

void Net::printInt2binStr(uint v, char* str)
{
	for (int i = 0; i < 32; ++i)
		str[31 - i] = (v >> i & 0x01) ? '1' : '0';
}

void Net::printLong2binStr(ullong v, char* str)
{
	for (int i = 0; i < 64; ++i)
		str[63 - i] = (v >> i & 0x01) ? '1' : '0';
}

string Net::itoa(int v)
{
	std::ostringstream os;
	os << v;
	return os.str();
}

string Net::ltoa(long long int v)
{
	string str;
#if defined(__MINGW32__) || defined(__MINGW64__)
	SPRINTF_STRING(&str, "%I64d", v)
#else
	SPRINTF_STRING(&str, "%lld", v)
#endif
	return str;
}

char Net::b2c(uchar chr)
{
	return (chr > 0x20 && chr < 0x7F) ? chr : '.';
}

Net::~Net()
{

}
