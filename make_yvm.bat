@echo off

echo Compiling yvm.nasm...
nasm --gprefix _ -fwin32 ./yvm/yvm.nasm -o ./yvm/yvm.o