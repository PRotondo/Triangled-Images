#include <iostream>
using namespace std;

typedef struct 
{
	int x, y;
} point_t;

typedef struct 
{
	unsigned char r, g, b;
	point_t points[3];
} Triangle;

bool inside(Triangle T, int x, int y)
{
	int sg = (T.points[1].x-T.points[0].x)*(y - T.points[0].y) - (x - T.points[0].x)*(T.points[1].y - T.points[0].y);
	int sg2 = (T.points[2].x-T.points[1].x)*(y - T.points[1].y) - (x - T.points[1].x)*(T.points[2].y - T.points[1].y);
	if ( sg > 0 && sg2 < 0 || sg < 0 && sg2 > 0)	return false;
	int sg3 = (T.points[0].x-T.points[2].x)*(y - T.points[2].y) - (x - T.points[2].x)*(T.points[0].y - T.points[2].y);
	if (sg > 0 && sg3 < 0 || sg < 0 && sg3 > 0)	return false;
	return !(sg2 > 0 && sg3 < 0 || sg2 < 0 && sg3 > 0);
}

double area(Triangle T)
{
	double a = ( (T.points[2].x - T.points[0].x)*(T.points[1].y - T.points[0].y) - (T.points[2].y - T.points[0].y)*(T.points[1].x-T.points[0].x) ) / (double)2;
	return a >= 0 ? a:(-a);
}
