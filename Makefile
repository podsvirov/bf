main.exe : main.c bf.h bf.c ; gcc main.c bf.c -lncurses -o main.exe
format : main.c ; clang-format -i main.c bf.h bf.c
