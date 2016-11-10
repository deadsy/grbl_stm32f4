//-----------------------------------------------------------------------------
/*

Re-entrant syscall hooks for the newlib library.
See libc.pdf in the gnu tool files.
See http://www.eistec.se/docs/contiki/a01137_source.html

*/
//-----------------------------------------------------------------------------

#include <unistd.h>
#include <sys/reent.h>
#include <sys/times.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>

#include "usart.h"

#define DEBUG
#include "logging.h"

//-----------------------------------------------------------------------------
// read/write functions for the serial port

static _ssize_t usart_read_r(struct _reent *ptr, int fd, void *buf, size_t cnt) {
  static int prev_char = -1;
  char *cbuf = buf;
  unsigned int i;
  char c;

  DBG0("%s: %s() line %d cnt %d", __FILE__, __func__, __LINE__, cnt);

  i = 0;
  while (i < cnt) {

    if (prev_char != -1) {
      // we have a look-ahead character
      c = prev_char;
      prev_char = -1;
    } else {
      // block until we get a character
      while(usart_tstc() == 0);
      c = usart_getc();
    }

    // handle a backspace
    if (c == 8 || c == 0x7F) {
      if (i > 0) {
        i --;
        usart_putc(8);
        usart_putc(' ');
        usart_putc(8);
      }
      continue;
    }

    // filter out things we don't care about
    if (!isprint(c) && c != '\n' && c != '\r') {
      continue;
    }

    // local echo
    usart_putc(c);

    // handle \r\n and \n\r sequences
    if (c == '\r' || c == '\n') {
      // give the other character a chance to come in
      if (usart_tstc() == 0) {
        // TODO
      }
      // get the other character, if we have one
      if (usart_tstc() != 0) {
        prev_char = usart_getc();
        if (prev_char + c == '\r' + '\n') {
          // ignore the other \r or \n
          prev_char = -1;
        }
      }
      // local echo the other character
      usart_putc('\r' + '\n' - c);
      // return a buffer terminating with \n
      cbuf[i ++] = '\n';
      return i;
    }

    // add the character to the buffer
    cbuf[i ++] = c;
  }

  // return the full buffer
  return i;
}

static _ssize_t usart_write_r(struct _reent *ptr, int fd, const void *buf, size_t cnt) {
  unsigned i;
  const char *cbuf = buf;
  DBG0("%s: %s() line %d cnt %d", __FILE__, __func__, __LINE__, cnt);
  for (i = 0; i < cnt; i ++) {
    char c = *cbuf++;
    // autoadd a CR
    if (c == '\n') {
      usart_putc('\r');
    }
    usart_putc(c);
  }
  return cnt;
}

//-----------------------------------------------------------------------------
// Empty environment definition

char *__env[1] = { 0 };
char **environ = __env;

//-----------------------------------------------------------------------------
// file operations

int _open_r(struct _reent *ptr, const char *file, int flags, int mode) {
  DBG0("%s: %s() line %d", __FILE__, __func__, __LINE__);
  // TODO
  ptr->_errno = ENOENT;
  return -1;
}

int _close_r(struct _reent *ptr, int fd) {
  DBG0("%s: %s() line %d", __FILE__, __func__, __LINE__);
  // TODO
  ptr->_errno = EBADF;
  return -1;
}

_ssize_t _read_r(struct _reent *ptr, int fd, void *buf, size_t cnt) {
  DBG0("%s: %s() line %d fd %d cnt %d", __FILE__, __func__, __LINE__, fd, cnt);
  switch (fd) {
    case 0:
    case 1:
    case 2:
      return usart_read_r(ptr, fd, buf, cnt);
    default:
      break;
  }
  ptr->_errno = EBADF;
  return -1;
}

_ssize_t _write_r(struct _reent *ptr, int fd, const void *buf, size_t cnt) {
  DBG0("%s: %s() line %d fd %d cnt %d", __FILE__, __func__, __LINE__, fd, cnt);
  switch (fd) {
    case 0:
    case 1:
    case 2:
      return usart_write_r(ptr, fd, buf, cnt);
    default:
      break;
  }
  ptr->_errno = EBADF;
  return -1;
}

int _fstat_r(struct _reent *ptr, int fd, struct stat *pstat) {
  DBG0("%s: %s() line %d fd %d", __FILE__, __func__, __LINE__, fd);
  switch (fd) {
    case 0:
    case 1:
    case 2: {
      // say it's a character file
      pstat->st_mode = S_IFCHR;
      return 0;
    }
    default:
      break;
  }
  ptr->_errno = EBADF;
  return -1;
}

off_t _lseek_r(struct _reent *ptr, int fd, off_t pos, int whence) {
  DBG0("%s: %s() line %d", __FILE__, __func__, __LINE__);
  switch (fd) {
    case 0:
    case 1:
    case 2: {
      return 0;
    }
    default:
      break;
  }
  ptr->_errno = EBADF;
  return -1;
}

int _isatty_r(struct _reent *ptr, int fd) {
  DBG0("%s: %s() line %d fd %d", __FILE__, __func__, __LINE__, fd);
  switch (fd) {
    case 0:
    case 1:
    case 2: {
      return 1;
    }
    default:
      break;
  }
  ptr->_errno = EBADF;
  return -1;
}

int _isatty(int fd) {
  return _isatty_r(_REENT, fd);
}

int _link_r(struct _reent *ptr, const char *old, const char *new) {
  DBG0("%s: %s() line %d", __FILE__, __func__, __LINE__);
  ptr->_errno = EMLINK;
  return -1;
}

int _stat_r(struct _reent *ptr, const char *file, struct stat *pstat) {
  // say it's a character file
  DBG0("%s: %s() line %d", __FILE__, __func__, __LINE__);
  pstat->st_mode = S_IFCHR;
  return 0;
}

int _unlink_r(struct _reent *ptr, const char *file) {
  DBG0("%s: %s() line %d", __FILE__, __func__, __LINE__);
  ptr->_errno = ENOENT;
  return -1;
}

//-----------------------------------------------------------------------------
// memory allocation

register char *stack_ptr asm("sp");

void *_sbrk_r(struct _reent *ptr, ptrdiff_t incr) {
  extern char end asm("end");
  static char *heap_end;
  char *prev_heap_end;

  if (heap_end == 0) {
    heap_end = &end;
  }

  DBG0("%s: %s() line %d heap_end 0x%p incr %d", __FILE__, __func__, __LINE__, heap_end, incr);

  prev_heap_end = heap_end;
  if (heap_end + incr > stack_ptr) {
    ptr->_errno = ENOMEM;
    return (void *) -1;
  }

  heap_end += incr;

  return (void *) prev_heap_end;
}

//-----------------------------------------------------------------------------
// process control

void _exit (int status) {
  DBG0("%s: %s() line %d", __FILE__, __func__, __LINE__);
  while (1);
}

int _execve_r(struct _reent *ptr, const char *name, char *const argv [], char *const env []) {
  DBG0("%s: %s() line %d", __FILE__, __func__, __LINE__);
  ptr->_errno = ENOMEM;
  return -1;
}

int _fork_r(struct _reent *ptr) {
  DBG0("%s: %s() line %d", __FILE__, __func__, __LINE__);
  ptr->_errno = ENOTSUP;
  return -1;
}

int _wait_r(struct _reent *ptr, int *status) {
  DBG0("%s: %s() line %d", __FILE__, __func__, __LINE__);
  ptr->_errno = ECHILD;
  return -1;
}

int _kill_r(struct _reent *ptr, int pid, int sig) {
  DBG0("%s: %s() line %d", __FILE__, __func__, __LINE__);
  ptr->_errno = EINVAL;
  return -1;
}

int _getpid_r(struct _reent *ptr) {
  DBG0("%s: %s() line %d", __FILE__, __func__, __LINE__);
  return 1;
}

//-----------------------------------------------------------------------------
// time

clock_t _times_r(struct _reent *ptr, struct tms *ptms) {
  DBG0("%s: %s() line %d", __FILE__, __func__, __LINE__);
  ptr->_errno = EACCES;
  return -1;
}

//-----------------------------------------------------------------------------
