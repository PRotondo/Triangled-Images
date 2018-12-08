all: Triangles

Triangles: 
	g++ `pkg-config --libs --cflags cairo cairo-xlib` HillClimbingTriangles.cpp -o HCT -O4 -lcairo

clean:
	rm *.o HCT
