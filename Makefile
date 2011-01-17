all: Lab1

Lab1: Lab1.cpp
	g++ -o Lab1 Lab1.cpp MyVector.cpp GameObject.cpp GameObject.h MyVector.h -lGL -lSDL -lGLU -lglut -lm -I/usr/include/SDL
	
clean:
	rm Lab1
