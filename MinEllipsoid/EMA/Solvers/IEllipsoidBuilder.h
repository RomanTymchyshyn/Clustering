#pragma once
#include <vector>
#include "../Drawing.h"

class IEllipsoidBuilder
{
public:
	virtual Ellipsoid2D* Exec(const vector<Point2D> & points, Window* window = NULL) = 0;
};