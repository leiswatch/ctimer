default:
	@gcc main.c -Wall -Wextra -o main -I./raylib/include -L./raylib/lib -l:libraylib.a -lm

build:
	@gcc main.c -Wall -Wextra -O3 -o main -I./raylib/include -L./raylib/lib -l:libraylib.a -lm
