if [ -f "src/main.exe" ]; then
    rm src/main.exe
fi
cd src
gcc -o main.exe main.c player.c menu.c stage.c enemy.c boss.c texture.c  -lraylib -lgdi32 -lwinmm 
./main