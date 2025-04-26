rm src/main.exe 
cd src
gcc -o main.exe main.c player.c menu.c stage1.c  -lraylib -lgdi32 -lwinmm 
./main