#include "CollisionDetection.h"

bool CollisionDetection::IsColliding(Body* a, Body* b)
{
    bool aIsCircle = a->shape->GetType() == CIRCLE;
    bool bIsCircle = a->shape->GetType() == CIRCLE;

    if (aIsCircle && bIsCircle)
    {
        return IsCollidingCircleCircle(a, b);
    }
}

bool CollisionDetection::IsCollidingCircleCircle(Body* a, Body* b)
{
    CircleShape* aCircleShape = (CircleShape*) a->shape;
    CircleShape* bCircleShape = (CircleShape*) b->shape;

    const Vec2 ab = b->position - a->position;
    const float radiusSum = aCircleShape->radius + bCircleShape->radius;

    bool isColliding = ab.MagnitudeSquared() <= (radiusSum * radiusSum);

    return isColliding;
}

// TODO: Implement other methods to check collision between other different shapes