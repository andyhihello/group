rm src/main.exe 
cd src
gcc -o main.exe main.c player.c  -lraylib -lgdi32 -lwinmm 
./main