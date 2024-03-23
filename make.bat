@echo off

echo Compiling yvm.nasm...
nasm --gprefix _ -fwin32 ./yvm/yvm.nasm -o ./yvm/yvm.o
if %ERRORLEVEL% == 0 (
	echo Compiling main.c...
	gcc ./yvm/main.c ./yvm/yvm.o -o ./bin/yvm.exe -m32
)