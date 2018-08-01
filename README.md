# bf - buffered formatted input/output {#mainpage}

This is a very small and lazy C library writen by Konstantin Podsvirov
to create interactive examples for his [book].

All the work with formatted input and output is performed by standard
[scanf] and [prinntf] functions, and behind the screen works [curses].

## Source code

Source available at [github].

## Code sample

```c
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
```

## Living preview

Launch the above code in your browser
[just now](https://podsvirov.github.io/bf/hello.html)!

## Documentation

Online documentation available
[there](https://podsvirov.github.io/bf/).

[github]: https://github.com/podsvirov/bf
[book]: http://book.podsvirov.pro
[scanf]: https://en.cppreference.com/w/c/io/fscanf
[prinntf]: https://en.cppreference.com/w/c/io/fprintf
[curses]: https://en.wikipedia.org/wiki/Curses_(programming_library
