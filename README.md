# ctimer

This is a simple timer written in C and Raylib.

### Installation
Make sure you have `gcc` and `make` (optional) installed on your system.

Download `raylib` release from https://github.com/raysan5/raylib and unpack into the project directory.

To compile the program just run `make build` command or run 

`gcc main.c -Wall -Wextra -O3 -o main -I./raylib/include -L./raylib/lib -l:libraylib.a -lm`

in your terminal

### Run
To run the program just run the `main` executable. If you want to run in in a countdown mode, you can just pass the time in `<x>h<y>m<z>s` format.
