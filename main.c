#include "bf.h"

#include <ctype.h>
#include <stdio.h>
#include <time.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

// Frame
static size_t f = 0;
static clock_t t = 0;
static clock_t tpf = CLOCKS_PER_SEC / 60;
#define TKS_NUM 60
static clock_t tks[TKS_NUM];

void tui() {
  bfo(0, "Hello from buffer!\n");
  // bfoa(1, "Milliseconds per frame %.3f\n", (float)tpf / (float)CLOCKS_PER_SEC
  // * 1000.0f);
  bfoa(1, "Application average %.3f ms/frame (%.1f FPS)\n",
       (float)tpf / (float)CLOCKS_PER_SEC * 1000.f,
       (float)CLOCKS_PER_SEC / (float)tpf);
  bfo(2, "Press {Esc} at any time to end the program\n");

  static int ch = 0;
  static int nf = 0;

  if (bfch() != bfer()) {
    ch = bfch();
  }
  if (isgraph(ch)) {
    bfoa(3, "Latest code = %i char = %c\n", ch, ch);
  } else {
    bfoa(3, "Latest code = %i\n", ch);
  }
  bfoa(4, "Current frame %i\n", nf++);
  bfo(5, "Enter your name and press {Enter}: ");
  static char name[256];
  if (bfia(6, "%s", name) == 1) {
    sprintf(bfb(0), "Hello, %s!\n", name);
    sprintf(bfb(5), "Enter your name again and press {Enter}: ");
    bfo(7, "Press {Enter} to restart program with new greeting");
  } else {
    return;
  }
}

void frame() {
  bfcs();
  tui();
  bffs();
  tks[f % TKS_NUM] = clock() - t;
  t = clock();
  tpf = TKS_NUM;
  for (size_t i = 0; i < TKS_NUM; i++)
    tpf += tks[i];
  tpf = tpf / TKS_NUM;
  f++;
#ifdef __EMSCRIPTEN__
  if (bfch() == 27) // {Esc}
  {
    bfbi();
    emscripten_cancel_main_loop();
  }
#endif
}

int main() {
  t = clock();
  for (size_t i = 0; i < TKS_NUM; i++)
    tks[i] = tpf;
  bfhi();

#ifdef __EMSCRIPTEN__
  bfas(1);
  emscripten_set_main_loop(frame, 0, 1);
#else
  do {
    frame();
  } while (bfch() != 27); // {Esc}
  bfbi();
#endif

  return 0;
}
