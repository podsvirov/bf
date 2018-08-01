main.exe : main.c bf.h bf.c ; gcc main.c bf.c -lncurses -o main.exe
hello.exe : hello.c bf.h bf.c ; gcc hello.c bf.c -I. -lncurses -o hello.exe
format : main.c ; clang-format -i main.c bf.h bf.c
doc : Doxyfile ; doxygen Doxyfile
