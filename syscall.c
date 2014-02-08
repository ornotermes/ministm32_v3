/*
 * Copyright (C) 2013 ARCOS-Lab Universidad de Costa Rica
 * Written by Federico Ruiz Ugalde <memeruiz@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <errno.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "syscall.h"

#undef errno
extern int errno;
#define HEAPSIZE 4096

long _write(int fd, const void *buf, size_t cnt);
long _write(int fd, const void *buf, size_t cnt) {
  (void)fd;
  (void)buf;
  return cnt;
}

long _read(int fd, char *buf, size_t cnt);
long _read(int fd, char *buf, size_t cnt) {
  (void)fd;
  (void)buf;
  (void)cnt;
  return 0;
}

int _open(const char *file, int flags, int mode);
int _open(const char *file, int flags, int mode) {
  (void)file;
  (void)flags;
  (void)mode;
  return -1;
}

long _close(int fd);
long _close(int fd) {
  (void)fd;
  return -1;
}

unsigned char _heap[HEAPSIZE];

caddr_t _sbrk(int incr);
caddr_t _sbrk(int incr) {
  static unsigned char *heap_end;
  unsigned char *prev_heap_end;
  ///* debugging
  /*
  static int first=0;
  if (first==2) {
    char incr_c[15];
  } else {
    first++;
  }
  */
  //
/* initialize */
  if( heap_end == 0 ) heap_end = _heap;
  prev_heap_end = heap_end;
  if( heap_end + incr -_heap > HEAPSIZE ) {
/* heap overflow—announce on stderr */
    write( 2, "Heap overflow!\n", 15 );
    abort();
  }
  heap_end += incr;
  return (caddr_t) prev_heap_end;
}

int _stat(const char *file, struct stat *pstat);
int _stat(NOT_USED const char *file, struct stat *pstat) {
  (void)file;
  pstat->st_mode = S_IFCHR;
  return 0;
}

int _fstat(int fd, struct stat *pstat);
int _fstat(NOT_USED int fd, struct stat *pstat) {
  (void)fd;
  pstat->st_mode = S_IFCHR;
  return 0;
}

off_t _lseek(int fd, off_t pos, int whence);
off_t _lseek(NOT_USED int fd, NOT_USED off_t pos, NOT_USED int whence) {
  (void)fd;
  (void)pos;
  (void)whence;
  return 0;
}

int poll(FILE *f) {
  (void)f;
  return -1;
}

int _kill(int pid);
int _kill(int pid)
{
  (void)pid;
  return -1;
}

int _getpid(void);
int _getpid(void)
{
  return -1;
}

void _exit(int sig)
{
  (void)sig;
  while(1);
}
