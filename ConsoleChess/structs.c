#include "structs.h"

Point POINT_Add(Point point1, Point point2)
{

	return (Point) { point1.x + point2.x, point1.y + point2.y };
}

int POINT_IsZero(Point point)
{
	if (point.x == '\0' ||
		point.x <= 0 &&
		point.y == '\0' ||
		point.y <= 0)
	{
		return 1;
	}
	return 0;
}
