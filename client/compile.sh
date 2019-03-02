#!/bin/sh
g++ main.cpp -o main.out -g -O0 -std=c++11 -L./glfw-3.2.1/lib/unix -L./glew-2.1.0/lib -lglew -lglfw3 -lGL -lm -ldl -lXinerama -lXrandr -lXi -lXcursor -lX11 -lXxf86vm -lpthread
