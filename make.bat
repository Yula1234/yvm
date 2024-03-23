@echo off

echo Compiling...
fasm src/main.asm src/yvm.o && gcc -fno-pie -no-pie src/main.c src/yvm.o -o main.exe -m32