#pragma once
#include "../../Drawing.h"
#include "ConvHull.h"

using namespace alglib;

Ellipsoid2D* RublevAlg(const vector<Point2D> & points, Window* window = NULL);