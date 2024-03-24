@echo off

echo Compiling yvm...
gcc ./yvm/main.c -o yvm.exe -m32

if %ERRORLEVEL% == 0 (
	echo Compiling yasm...
	g++ -fmax-errors=2 -Wdouble-promotion -Wdiv-by-zero -Wold-style-cast -Wextra -pedantic -Wall -Werror -Wswitch -std=c++2a ./yasm/main.cpp -o yasm.exe
)