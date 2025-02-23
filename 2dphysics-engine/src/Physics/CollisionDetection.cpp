#include "CollisionDetection.h"

#include <limits>

bool CollisionDetection::IsColliding(Body* a, Body* b, Contact& contact)
{
    bool aIsCircle = a->shape->GetType() == CIRCLE;
    bool bIsCircle = a->shape->GetType() == CIRCLE;

    bool aIsPolygon = a->shape->GetType() == POLYGON || a->shape->GetType() == BOX;
    bool bIsPolygon = b->shape->GetType() == POLYGON || b->shape->GetType() == BOX;

    if (aIsCircle && bIsCircle)
    {
        return IsCollidingCircleCircle(a, b, contact);
    }

    if (aIsPolygon && bIsPolygon)
    {
        return IsCollidingPolygonPolygon(a, b, contact);
    }
}

bool CollisionDetection::IsCollidingCircleCircle(Body* a, Body* b, Contact& contact)
{
    CircleShape* aCircleShape = (CircleShape*) a->shape;
    CircleShape* bCircleShape = (CircleShape*) b->shape;

    const Vec2 ab = b->position - a->position;
    const float radiusSum = aCircleShape->radius + bCircleShape->radius;

    bool isColliding = ab.MagnitudeSquared() <= (radiusSum * radiusSum);

	if (!isColliding)
	{
        return false;
	}

    // Compute contact collision information
    contact.a = a;
    contact.b = b;

    contact.normal = ab;
    contact.normal.Normalize();

    contact.start = b->position - contact.normal * bCircleShape->radius;
    contact.end = a->position + contact.normal * aCircleShape->radius;

    contact.depth = (contact.end - contact.start).Magnitude();

    return true;
}

bool CollisionDetection::IsCollidingPolygonPolygon(Body* a, Body* b, Contact& contact)
{
    const PolygonShape* aPolygonShape = (PolygonShape*) a->shape;
    const PolygonShape* bPolygonShape = (PolygonShape*) b->shape;


	if (aPolygonShape->FindMinSeparation(bPolygonShape) >= 0)
	{
        return false;
	}

    if (bPolygonShape->FindMinSeparation(aPolygonShape) >= 0)
    {
        return false;
    }

    return true;
}

// TODO: Implement other methods to check collision between other different shapes