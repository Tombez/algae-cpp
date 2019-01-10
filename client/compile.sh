#!/bin/sh
g++ main.cpp -o main -g -O0 -std=c++11 -I./glfw-3.2.1/include -I./glew-2.1.0/include -L./glfw-3.2.1/lib -L./glew-2.1.0/lib -lglew -lglfw3 -lGL -lm -ldl -lXinerama -lXrandr -lXi -lXcursor -lX11 -lXxf86vm -lpthread
