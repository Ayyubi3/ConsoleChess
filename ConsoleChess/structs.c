#include "structs.h"

Point POINT_Add(Point point1, Point point2)
{

	return (Point) { point1.x + point2.x, point1.y + point2.y };
}

