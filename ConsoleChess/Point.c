#include "Point.h"

//Point
Point POINT(int x, int y)
{
	return (Point) { x, y };
}

Point POINT_Add(Point point1, Point point2)
{
	return (Point) { point1.x + point2.x, point1.y + point2.y };
}

int POINT_isZero(Point point)
{
	return (point.x == 0 && point.y == 0);
}

int POINT_equals(Point point1, Point point2)
{
	return (point1.x == point2.x) && (point1.y == point2.y);
}

int POINT_getIndex(Point p)
{
	return (p.x - 1) + (p.y - 1) * 8;
}

int POINT_isPointInArray(Game* sys, Point p, int length, Point* arr)
{
	for (int i = 0; i < length; i++)
	{
		if (POINT_equals(p, arr[i]))
		{
			return 1;
		}
	}
	return 0;
}