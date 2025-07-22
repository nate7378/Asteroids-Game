# Asteroids Game (OpenGL/GLUT)

A simple Asteroids-style game built with C++, OpenGL, and GLUT.

1.Linux(Ubuntu/Debian)

Install dependencies:

sudo apt-get install g++ freeglut3-dev libglu1-mesa-dev
git clone https://github.com/nate7378/Asteroids-Game.git
cd Asteroids-Game
g++ asteroids_game.cpp -o asteroids -lGL -lGLU -lglut
./asteroids

2.Windows (MinGW)

Install MinGW (with g++) and GLUT:

Download MinGW and add g++ to PATH.
Download freeglut for MinGW and place it in MinGW’s include/lib folders.
Compile & run in Command Prompt:
   
g++ asteroids_game.cpp -o asteroids -I"C:\MinGW\include" -L"C:\MinGW\lib" -lopengl32 -lglu32 -lfreeglut
asteroids.exe
