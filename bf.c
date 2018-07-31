#include "bf.h"

#include <curses.h>
#include <locale.h>
#include <string.h>
#include <time.h>

struct _bfbuf {
  bool done;
  int size;
  int argc;
  char str[1024];
};

static WINDOW *_bfw = NULL;
static bool _bfas = FALSE;
static int _bfch = ERR;
static struct _bfbuf _bfbufs[1024];

#ifdef PDCURSES
static bool _bfu8 = TRUE;
#else
static bool _bfu8 = FALSE;
#endif

int bfhi() {
  if (_bfw)
    return 0;

#ifdef NCURSES_VERSION
  setlocale(LC_ALL, "");
#endif

  _bfw = initscr();
  cbreak();
  noecho();
  scrollok(_bfw, TRUE);

  bfas(_bfas);

  memset(_bfbufs, 0, sizeof(_bfbufs));

  return 0;
}

void bfcs() { clear(); }

void bffs() {
  refresh();
  _bfch = getch();
}

void bfov(struct _bfbuf *buf, const char *fmt, va_list args) {
  buf->size = vsprintf(buf->str, fmt, args);
  buf->done = TRUE;
  addstr(buf->str);
}

int bfo(int nbuf, const char *fmt, ...) {
  struct _bfbuf *buf = &_bfbufs[nbuf];
  if (buf->done) {
    addstr(buf->str);
  } else {
    va_list args;
    va_start(args, fmt);
    bfov(buf, fmt, args);
    va_end(args);
  }
  return buf->size;
}

int bfoa(int nbuf, const char *fmt, ...) {
  struct _bfbuf *buf = &_bfbufs[nbuf];
  va_list args;
  va_start(args, fmt);
  bfov(buf, fmt, args);
  va_end(args);
  return buf->size;
}

void bfau8c(struct _bfbuf *buf, int code) {
    int chw = 1;
    if (code < 0x80) {
        chw = 1;
        buf->str[buf->size] = code;
    } else if (code < 0x800) {
        chw = 2;
        buf->str[buf->size] = ((code & 0x07c0) >> 6) | 0xc0;
        buf->str[buf->size + 1] = (code & 0x003f) | 0x80;
    } else {
        chw = 3;
        buf->str[buf->size] = ((code & 0xf000) >> 12) | 0xe0;
        buf->str[buf->size + 1] = ((code & 0x0fc0) >> 6) | 0x80;
        buf->str[buf->size + 2] = (code & 0x003f) | 0x80;
    }
    buf->size += chw;
    buf->str[buf->size] = 0;
}

int bfiv(struct _bfbuf *buf, const char *fmt, va_list args) {
  if (_bfch != ERR && _bfch != 10) {
    if (_bfch == 8 || _bfch == 127) // {Backspace}
    {
      if (buf->size > 0) {
          int chw = 1;
          if (_bfu8) {
            while (chw <= buf->size
                   && ((buf->str[buf->size - chw] & 0xc0) == 0x80)) {
                chw++;
            }
          }
          buf->size -= chw;
          buf->str[buf->size] = 0;
      }
    } else {
        if (_bfu8) {
          bfau8c(buf, _bfch);
        } else {
          buf->str[buf->size] = _bfch;
          buf->size += 1;
          buf->str[buf->size] = 0;
        }
    }
  } else if (_bfch != ERR) {
    buf->argc = 0;
    for (int i = 0; fmt[i] != 0; i++) {
      if (fmt[i] == '%')
        buf->argc++;
    }
    int res = vsscanf(buf->str, fmt, args);
    if (res == buf->argc) {
      buf->done = TRUE;
      addstr(buf->str);
      addch('\n');
      return buf->argc;
    }
  }
  addstr(buf->str);
  return -1;
}

int bfi(int nbuf, const char *fmt, ...) {
  struct _bfbuf *buf = &_bfbufs[nbuf];
  if (buf->done) {
    addstr(buf->str);
    addch('\n');
    return buf->argc;
  }
  va_list args;
  va_start(args, fmt);
  int res = bfiv(buf, fmt, args);
  va_end(args);
  return res;
}

int bfia(int nbuf, const char *fmt, ...) {
  struct _bfbuf *buf = &_bfbufs[nbuf];
  if (buf->done) {
    bfre(nbuf);
  }
  va_list args;
  va_start(args, fmt);
  int res = bfiv(buf, fmt, args);
  va_end(args);
  return res;
}

char *bfb(int nbuf) { return _bfbufs[nbuf].str; }

int bfre(int nbuf) {
  struct _bfbuf *buf = &_bfbufs[nbuf];
  memset(buf, 0, sizeof(struct _bfbuf));
  return 0;
}

int bfch() { return _bfch; }

int bfer() { return ERR; }

int bfbi() {
  if (_bfw) {
    endwin();
  }
  return 0;
}

int bfas(int as) {
  int r = _bfas;
  if (_bfw) {
    if (as) {
      nodelay(_bfw, TRUE);
      _bfas = TRUE;
    } else {
      nodelay(_bfw, FALSE);
      _bfas = FALSE;
    }
  }
  return r;
}

int bfu8(int u8)
{
  int r = _bfu8;
  if (u8) {
    _bfu8 = TRUE;
  } else {
    _bfu8 = FALSE;
  }
  return r;
}
