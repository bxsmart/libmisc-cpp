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

#ifndef MACRO_H_
#define MACRO_H_

#ifdef WINDOWS
#include <winsock2.h>
#include <windows.h>
#include <winbase.h>
#include <Iphlpapi.h>
#include <ws2tcpip.h>
#include <process.h>
#include <direct.h>
#include <io.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <signal.h>
#include <assert.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include <locale.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#if defined(__MINGW32__) || defined(__MINGW64__)
#include <unistd.h>
#include <sys/time.h>
#include <semaphore.h>
#endif
#if defined(LINUX) || defined(MAC)
#include <unistd.h>
#include <netdb.h>
#include <semaphore.h>
#include <pthread.h>
#include <poll.h>
#include <dirent.h>
#include <regex.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#ifdef LINUX
#include <netinet/tcp.h>
#include <net/if_arp.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/syscall.h>
#include <sys/sysinfo.h>
#include <sys/prctl.h>
#ifndef ANDROID
#include <execinfo.h>
#endif
#endif
#include <sys/mman.h>
#include <net/if.h>
#include <arpa/inet.h>
#endif
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <queue>
#include <set>
#include <map>
#include <functional>
#include <unordered_set>
#include <unordered_map>
#include <forward_list>
#include <algorithm>
#include <iomanip>
#include <atomic>
#include <thread>
#include <condition_variable>
#include <future>
#include <memory>
#include <mutex>

using namespace std;

#define __LIBMISC_H__

#if !defined (__LIBMISC_H__) && !defined (LIBMISC)
#error only libmisc.h can be included directly.
#endif
typedef unsigned char uchar; 
typedef unsigned short ushort; 
typedef unsigned int uint; 
typedef unsigned long long int ullong; 
#define PRESENT													0x01							
#define NOT_PRESENT												0x00							
#define INVALID													  -1							
#define H2N_DIR													0x00							
#define N2H_DIR													0x01							

#endif 
