#pragma once
#include "Game.h"

struct Point { int x; int y; } ;

typedef struct Point Point;

Point POINT(int x, int y);
Point POINT_Add(Point point1, Point point2);
int POINT_isZero(Point point);
int POINT_equals(Point point1, Point point2);
int POINT_getIndex(Point p);
int POINT_isPointInArray(Game* sys, Point p, int length, Point* arr);

