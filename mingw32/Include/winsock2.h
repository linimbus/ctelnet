/*

  Definitions for winsock 2

  FIXME: This is mostly a stub for now. Taken from the Wine project.
  
  Portions Copyright (c) 1980, 1983, 1988, 1993
  The Regents of the University of California.  All rights reserved.

  Portions Copyright (c) 1993 by Digital Equipment Corporation.
 */

#ifndef _WINSOCK2_H
#define _WINSOCK2_H
#define _GNU_H_WINDOWS32_SOCKETS
#ifdef __cplusplus
extern "C" {
#endif

#include <winsock.h>

#define FD_MAX_EVENTS 10

#define FD_READ_BIT     0
#define FD_WRITE_BIT    1
#define FD_OOB_BIT      2
#define FD_ACCEPT_BIT   3
#define FD_CONNECT_BIT  4
#define FD_CLOSE_BIT    5

typedef struct _WSANETWORKEVENTS {
  long lNetworkEvents;
  int iErrorCode[FD_MAX_EVENTS];
} WSANETWORKEVENTS, *LPWSANETWORKEVENTS;

typedef HANDLE WSAEVENT;
#define WSACreateEvent() CreateEvent(NULL, TRUE, FALSE, NULL)
/* etc */

int WINAPI WSAEnumNetworkEvents(SOCKET s, WSAEVENT hEventObject, LPWSANETWORKEVENTS lpNetworkEvents);
int WINAPI WSAEventSelect(SOCKET s, WSAEVENT hEventObject, long lNetworkEvents);

ifdef __cplusplus
}
#endif
#endif

