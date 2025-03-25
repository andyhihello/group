rm src/main.exe 
cd src
gcc -o main.exe main.c player.c player_attrack.c -lraylib -lgdi32 -lwinmm -lm
./main