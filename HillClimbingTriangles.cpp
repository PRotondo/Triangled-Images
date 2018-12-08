#define NUM_SHAPES 5000

#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#include "Tri.cpp"
#include <iostream>
#include <cassert>
#include <algorithm>
#include <sstream>
using namespace std;

#include <cairo.h>
#include <cairo-xlib.h>

#define RANDDOUBLE(max) ((random() / (double)RAND_MAX) * max)
#define ABS(val) ((val) < 0 ? -(val) : (val))
#define CLAMP(val, m, M) max( m, min(M,val))
#define sqr(x) ((x)*(x))
#define ERROR_MAXIMO 765
#define PrMutColour 0.1
#define PrCatacl 0.01

int WIDTH, HEIGHT;
int FONDO_G = 0, FONDO_B = 0, FONDO_R = 0;

// population
vector<Triangle> individuo(NUM_SHAPES);
// fitness
double mejorFitness;
// IMAGEN A APROXIMAR
unsigned char * goal_data = NULL;
// ans
long long ans = 0;

vector <vector<int> > r,g,b,C;

void init_Pop(void) 
{
	for (int j = 0; j < NUM_SHAPES; j++) 
	{
		int x = rand()%WIDTH, y = rand()%HEIGHT;
		for (int k = 0; k < 3; k++) 
		{
			individuo[j].points[k].x = x;
			individuo[j].points[k].y = y;
		}
		int thispixel = y * WIDTH * 4 + x * 4;
		individuo[j].r = goal_data[thispixel + 2 ]; // red 
		individuo[j].g = goal_data[thispixel + 1]; // green
		individuo[j].b = goal_data[thispixel]; // blue
	}
}


double evalFitness(vector<Triangle> ind) 
{
	for (int k = 0; k < NUM_SHAPES; k++)
	{
		int xmin = min(ind[k].points[0].x,min(ind[k].points[1].x,ind[k].points[2].x)), xmax =  max(ind[k].points[0].x,max(ind[k].points[1].x,ind[k].points[2].x));
		int ymin = min(ind[k].points[0].y,min(ind[k].points[1].y,ind[k].points[2].y)), ymax =  max(ind[k].points[0].y,max(ind[k].points[1].y,ind[k].points[2].y));
		for (int x = xmin; x <= xmax; x++)
		{
			int l, h = ymax + 1, yinit;
			for (l = ymin; l <= ymax; l++)
				if (inside(ind[k],x,l))	break;
			if (!inside(ind[k],x,l))	continue;
			yinit = l;
			while ( h - l > 1)
			{
				int m = (h + l) >> 1;
				if (inside(ind[k],x,m))	l = m;
				else h = m;
			}
			for (int y = yinit; y <= l; y++)
				{
					r[x][y] += (int)ind[k].r;
					g[x][y] += (int)ind[k].g;
					b[x][y] += (int)ind[k].b;
					C[x][y]++;
				}

		}
	}
	for (int x = 0; x < WIDTH; x++)
		for (int y = 0; y < HEIGHT; y++)
		{
			int R, G, B;
			if (C[x][y] > 0)
			{
				R = r[x][y] / C[x][y];
				G = g[x][y] / C[x][y];
				B = b[x][y] / C[x][y];
			}
			else R = FONDO_R, G = FONDO_G, B = FONDO_B;
			int thispixel = y * WIDTH * 4 + x * 4;
			ans += ABS( (int) goal_data[thispixel + 2 ] - R) + ABS( (int) goal_data[thispixel + 1 ] - G) + ABS( (int) goal_data[thispixel  ] - B);
		}
	return 1 - ans / (((double) ERROR_MAXIMO) * WIDTH * HEIGHT);
}

void iterate(Triangle t, void f(Triangle, int,int) )
{
		int xmin = min(t.points[0].x,min(t.points[1].x,t.points[2].x)), xmax =  max(t.points[0].x,max(t.points[1].x,t.points[2].x));
		int ymin = min(t.points[0].y,min(t.points[1].y,t.points[2].y)), ymax =  max(t.points[0].y,max(t.points[1].y,t.points[2].y));
		for (int x = xmin; x <= xmax; x++)
		{
			int l, h = ymax + 1, yinit;
			for (l = ymin; l <= ymax; l++)
				if (inside(t,x,l))	break;
			if (!inside(t,x,l))	continue;
			yinit = l;
			while ( h - l > 1)
			{
				int m = (h + l) >> 1;
				if (inside(t,x,m))	l = m;
				else h = m;
			}
			for (int y = yinit; y <= l; y++) f(t, x,y);

		}
}

void sum(Triangle t, int x, int y)
{
	int thispixel = y * WIDTH * 4 + x * 4, R, G, B;
	if (C[x][y] > 0)
	{
		R = r[x][y] / C[x][y];
		G = g[x][y] / C[x][y];
		B = b[x][y] / C[x][y];
	}
	else R = FONDO_R, G = FONDO_G, B = FONDO_B;
	ans -= ABS( (int) goal_data[thispixel + 2 ] - R) + ABS( (int) goal_data[thispixel + 1 ] - G) + ABS( (int) goal_data[thispixel  ] - B);
	C[x][y]++;
	r[x][y] += t.r;
	g[x][y] += t.g;
	b[x][y] += t.b;	
	if (C[x][y] > 0)
	{
		R = r[x][y] / C[x][y];
		G = g[x][y] / C[x][y];
		B = b[x][y] / C[x][y];
	}
	else R = FONDO_R, G = FONDO_G, B = FONDO_B;
	ans += ABS( (int) goal_data[thispixel + 2 ] - R) + ABS( (int) goal_data[thispixel + 1 ] - G) + ABS( (int) goal_data[thispixel  ] - B);
}

void sub(Triangle t, int x, int y)
{
	int thispixel = y * WIDTH * 4 + x * 4;
	int R, G, B;
	if (C[x][y] > 0)
	{
		R = r[x][y] / C[x][y];
		G = g[x][y] / C[x][y];
		B = b[x][y] / C[x][y];
	}
	else R = FONDO_R, G = FONDO_G, B = FONDO_B;
	ans -= ABS( (int) goal_data[thispixel + 2 ] - R) + ABS( (int) goal_data[thispixel + 1 ] - G) + ABS( (int) goal_data[thispixel  ] - B);
	C[x][y]--;
	r[x][y] -= t.r;
	g[x][y] -= t.g;
	b[x][y] -= t.b;
	if (C[x][y] > 0)
	{
		R = r[x][y] / C[x][y];
		G = g[x][y] / C[x][y];
		B = b[x][y] / C[x][y];
	}
	else R = FONDO_R, G = FONDO_G, B = FONDO_B;
	ans += ABS( (int) goal_data[thispixel + 2 ] - R) + ABS( (int) goal_data[thispixel + 1 ] - G) + ABS( (int) goal_data[thispixel  ] - B);
}

double changeTri(Triangle Old, Triangle New)
{
	iterate(Old,sub);
	iterate(New,sum);
	return 1 - ans / (((double) ERROR_MAXIMO) * WIDTH * HEIGHT);
}

void mutate(int dest)
{
	if ( RANDDOUBLE(1) < PrMutColour)
	{
		int c = rand()%3;
		if (c == 0)
		{
			if (RANDDOUBLE(1) < PrCatacl)
			{
				individuo[dest].r = rand()&255;
				return;
			}
			individuo[dest].r = CLAMP( (int)individuo[dest].r + (rand()&1 ? (-1):1)*( rand()%101) ,0,255);
		}
		else if ( c == 1)
		{
			if (RANDDOUBLE(1) < PrCatacl)
			{
				individuo[dest].g = rand()&255;
				return;
			}
			individuo[dest].g = CLAMP( (int)individuo[dest].g + (rand()&1 ? (-1):1)*( rand()%51) ,0,255);
		}
		else
		{
			if (RANDDOUBLE(1) < PrCatacl)
			{
				individuo[dest].b = rand()&255;
				return;
			}
			individuo[dest].b = CLAMP( (int)individuo[dest].b + (rand()&1 ? (-1):1)*( rand()%51) ,0,255);
		}
		return;
	}
	int v = rand()%3;
	// mutate x
	if ( rand()&1 )
	{
		if (RANDDOUBLE(1) < PrCatacl)
		{
			individuo[dest].points[v].x = rand()%WIDTH;
			return;
		}
		individuo[dest].points[v].x = CLAMP( (int) individuo[dest].points[v].x +  (rand()&1 ? (-1):1)*( rand()%(WIDTH>>3)), 0 , WIDTH-1);
		return;
	}
	if (RANDDOUBLE(1) < PrCatacl)
	{
		individuo[dest].points[v].y = rand()%HEIGHT;
		return;
	}
	individuo[dest].points[v].y = CLAMP( (int) individuo[dest].points[v].y +  (rand()&1 ? (-1):1)*( rand()%(HEIGHT>>3)), 0 , HEIGHT-1);
}

void save(vector<Triangle> ind, int n)
{
	cairo_surface_t *surface;
	cairo_t *cr;	
	surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, WIDTH, HEIGHT);
	cr = cairo_create (surface);
	for (int x = 0; x < WIDTH; x++)
		for (int y = 0; y < HEIGHT; y++)
		{
			cairo_rectangle (cr, x, y, 1, 1);
			int R, G, B;
			if (C[x][y] > 0)
			{
				R = r[x][y] / C[x][y];
				G = g[x][y] / C[x][y];
				B = b[x][y] / C[x][y];
			}
			else R = FONDO_R, G = FONDO_G, B = FONDO_B;
			cairo_set_source_rgba (cr, R / (double)255, G / (double)255, B / (double)255, 1);
			cairo_fill (cr);
		}
	stringstream s;	s << n;
	cairo_surface_write_to_png(surface,("out " + s.str() + ".png").c_str());
}

int main(int argc, char ** argv) 
{
	cairo_surface_t * pngsurf;
//	IF NO ARGUMENT IS  GIVEN, THEN LOAD MONA, ELSE LOAD GIVEN IMAGE
	if (argc == 1)
		pngsurf = cairo_image_surface_create_from_png("mona.png");
	else
		pngsurf = cairo_image_surface_create_from_png(argv[1]);
//	WIDTH AND HEIGHT
	WIDTH = cairo_image_surface_get_width(pngsurf);
	HEIGHT = cairo_image_surface_get_height(pngsurf);
//	Get the data from the image
	goal_data = cairo_image_surface_get_data(pngsurf);
//	Setear fondo
	for (int x = 0; x < WIDTH; x++)
		for (int y = 0; y < HEIGHT; y++)
		{
			int thispixel = 4*y*WIDTH + 4*x;
			FONDO_R += goal_data[thispixel + 2];
			FONDO_G += goal_data[thispixel + 1];
			FONDO_B += goal_data[thispixel    ];
		}
		FONDO_R /= WIDTH*HEIGHT, FONDO_G /= WIDTH*HEIGHT, FONDO_B /= WIDTH*HEIGHT;
	for (int a = 0; a < 10; a++)
	{
	//	SEED
		srandom(getpid() + time(NULL));
	//	RANDOM INITIAL POPULATION
		init_Pop();
		struct timeval start;
		gettimeofday(&start, NULL);
		g = b = C = r = vector<vector<int> >(WIDTH,vector<int>(HEIGHT,0));
		mejorFitness = evalFitness(individuo);
		for (int G = 0; ; G++)
		{
//			if ( G < 10000 && (G%500) == 0) 
//				save(individuo,G);
			if ( (G % 1000000) == 0 )
				cout << mejorFitness << endl, save(individuo,G);
			int dest = rand()%NUM_SHAPES;
			Triangle Old = individuo[dest];
			mutate(dest);
			Triangle New = individuo[dest];
			double NewFit = changeTri(Old,New);
			if (NewFit > mejorFitness)
				mejorFitness = NewFit;
			else
				individuo[dest] = Old, changeTri(New,Old);
		}
	}
}
