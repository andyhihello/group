rm src/main.exe 
cd src
gcc -o main.exe main.c  -lraylib -lgdi32 -lwinmm 
./main