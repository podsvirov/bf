#include <bf.h>

#include <emscripten.h>

static char name[32];
static int running = 1;

void frame()
{
    if (running) {
        bfcs();
        bfo(0, "Enter your name and press {Enter}: ");
        if (bfi(1, "%s", name) == 1) {
          bfo(2, "Hello, %s!", name);
          running = 0;
        }
        bffs();
    } else {
        bfbi();
        emscripten_cancel_main_loop();
    }
}

int main() {
  bfhi();
  bfas(1);
  emscripten_set_main_loop(frame, 0, 1);
  return 0;
}
