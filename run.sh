rm src/main.exe 
cd src
gcc -o main.exe main.c player.c menu.c stage.c enemy.c  -lraylib -lgdi32 -lwinmm 
./main