all: Lab1

Lab1: Lab1.cpp GameObject.cpp GameObject.h  MyVector.cpp MyVector.h
	g++ -o Lab1 Lab1.cpp MyVector.cpp GameObject.cpp GameObject.h MyVector.h -lGL -lSDL -lGLU -lglut -lm -I/usr/include/SDL
	
clean:
	rm Lab1
run:
	./Lab1 bunny500.m
