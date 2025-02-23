#ifndef COLLISIONDETECTION_H
#define COLLISIONDETECTION_H

#include "./Body.h"

struct CollisionDetection
{
	static bool IsColliding(Body* a, Body* b);
	static bool IsCollidingCircleCircle(Body* a, Body* b);
	// TODO: static bool IsCollidingPolygonPolygon(Body* a, Body* b);
	// TODO: static bool IsCollidingPolygonCircle(Body* a, Body* b);
};

#endif