// Copyright (c) Athena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

#ifndef	_SOCKET_H_
#define _SOCKET_H_

#include <stdio.h>

#ifdef __WIN32
#define __USE_W32_SOCKETS
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif
#include <time.h>
#include "malloc.h"
#include "cbasetypes.h"

extern time_t last_tick;
extern time_t stall_time;

// define declaration

#define RFIFOSPACE(fd) (session[fd]->max_rdata-session[fd]->rdata_size)
#ifdef TURBO
#define RFIFOHEAD(fd) char *rbPtr = session[fd]->rdata+session[fd]->rdata_pos
#define RFIFOP(fd,pos) (&rbPtr[pos])
#else
//Make it a comment so it does not disrupts the rest of code.
#define RFIFOHEAD(fd) //
#define RFIFOP(fd,pos) (session[fd]->rdata+session[fd]->rdata_pos+(pos))
#endif
// use function instead of macro.
#define RFIFOB(fd,pos) (*(unsigned char*)RFIFOP(fd,pos))
#define RFIFOW(fd,pos) (*(unsigned short*)RFIFOP(fd,pos))
#define RFIFOL(fd,pos) (*(unsigned long*)RFIFOP(fd,pos))
#define RFIFOREST(fd)  (session[fd]->rdata_size-session[fd]->rdata_pos)
#define RFIFOFLUSH(fd) (memmove(session[fd]->rdata,RFIFOP(fd,0),RFIFOREST(fd)),session[fd]->rdata_size=RFIFOREST(fd),session[fd]->rdata_pos=0)
//#define RFIFOSKIP(fd,len) ((session[fd]->rdata_size-session[fd]->rdata_pos-(len)<0) ? (fprintf(stderr,"too many skip\n"),exit(1)) : (session[fd]->rdata_pos+=(len)))

#define RBUFP(p,pos) (((unsigned char*)(p))+(pos))
#define RBUFB(p,pos) (*(unsigned char*)RBUFP((p),(pos)))
#define RBUFW(p,pos) (*(unsigned short*)RBUFP((p),(pos)))
#define RBUFL(p,pos) (*(unsigned long*)RBUFP((p),(pos)))

#define WFIFOSPACE(fd) (session[fd]->max_wdata-session[fd]->wdata_size)
#ifdef TURBO
#define WFIFOHEAD(fd, x) char *wbPtr = session[fd]->wdata+session[fd]->wdata_size;
#define WFIFOP(fd,pos) (&wbPtr[pos])
#else
#define WFIFOHEAD(fd, x) ;
#define WFIFOP(fd,pos) (session[fd]->wdata+session[fd]->wdata_size+(pos))
#endif
#define WFIFOB(fd,pos) (*(unsigned char*)WFIFOP(fd,pos))
#define WFIFOW(fd,pos) (*(unsigned short*)WFIFOP(fd,pos))
#define WFIFOL(fd,pos) (*(unsigned long*)WFIFOP(fd,pos))
// use function instead of macro.
//#define WFIFOSET(fd,len) (session[fd]->wdata_size = (session[fd]->wdata_size + (len) + 2048 < session[fd]->max_wdata) ? session[fd]->wdata_size + len : session[fd]->wdata_size)
#define WBUFP(p,pos) (((unsigned char*)(p)) + (pos))
#define WBUFB(p,pos) (*(unsigned char*)((p) + (pos)))
#define WBUFW(p,pos) (*(unsigned short*)((p) + (pos)))
#define WBUFL(p,pos) (*(unsigned long*)((p) + (pos)))

//FD_SETSIZE must be modified on the project files/Makefile, since a change here won't affect
// dependant windows libraries.
/*
#ifdef __WIN32
//The default FD_SETSIZE is kinda small for windows systems.
	#ifdef FD_SETSIZE
	#undef FD_SETSIZE
	#endif
#define FD_SETSIZE 4096
#endif
*/
#ifdef __INTERIX
#define FD_SETSIZE 4096
#endif // __INTERIX

/* Removed Cygwin FD_SETSIZE declarations, now are directly passed on to the compiler through Makefile [Valaris] */

// Session type
enum SessionType {
	SESSION_UNKNOWN	= -1,
	SESSION_RAW		= 0,
	SESSION_HTTP	= 1,
//-----------------
	SESSION_MAX		= 2
};

// Struct declaration

struct socket_data{
	unsigned char eof;
	unsigned char *rdata, *wdata;
	unsigned int max_rdata, max_wdata;
	unsigned int rdata_size, wdata_size;
	int rdata_pos;
	time_t rdata_tick;
	struct sockaddr_in client_addr;
	int (*func_recv)(int);
	int (*func_send)(int);
	int (*func_parse)(int);
	int (*func_console)(char*);
	void* session_data;
	void* session_data2;
	enum SessionType type;
};

// Parse functions table
struct func_parse_table {
	int (*func)(int);
	int (*check)(struct socket_data *);
};
extern struct func_parse_table func_parse_table[SESSION_MAX];


// Data prototype declaration

extern struct socket_data *session[FD_SETSIZE];

extern int fd_max;

//////////////////////////////////
// some checking on sockets
extern bool session_isValid(int fd);
extern bool session_isActive(int fd);
//////////////////////////////////

// Function prototype declaration

int make_listen_port(int);
int make_listen_bind(long,int);
int make_connection(long,int);
int delete_session(int);
int realloc_fifo(int fd,unsigned int rfifo_size,unsigned int wfifo_size);
int realloc_writefifo(int fd, size_t addition);
int WFIFOSET(int fd,int len);
int RFIFOSKIP(int fd,int len);

int do_sendrecv(int next);
int do_parsepacket(void);
void do_close(int fd);
void socket_init(void);
void socket_final(void);

extern void flush_fifo(int fd);
extern void flush_fifos(void);
extern void set_nonblocking(int fd, int yes);

int start_console(void);

void set_defaultparse(int (*defaultparse)(int));
void set_defaultconsoleparse(int (*defaultparse)(char*));

//Resolves the hostname and stores the string representation of the string in ip.
//Meant to simplify calls to gethostbyname without the need of all the
//required network includes.
in_addr_t resolve_hostbyname(char* hostname, char *ip);

extern unsigned int addr_[16];   // ip addresses of local host (host byte order)
extern unsigned int naddr_;   // # of ip addresses


#endif	// _SOCKET_H_
