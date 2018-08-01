#include <bf.h>

int main() {
  bfhi(); // hello bf
  char name[32];
  int running = 1;
  do {
    bfcs(); // clear screen
    bfo(0, "Enter your name and press {Enter}: "); // buffered output
    if (bfi(1, "%s", name) == 1) { // like scanf
      bfo(2, "Hello, %s!", name); // like printf
      running = 0;
    }
    bffs(); // fill screen
  } while (running);
  bfbi(); // bye bf
  return 0;
}
