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

float FindMinSeparation(const PolygonShape& a, const PolygonShape& b)
{
    float separation = std::numeric_limits<float>::lowest();

    // Loop all the vertices of  "a"
    for (int i = 0; i < a.worldVertices.size(); i++)
    {
        Vec2 va = a.worldVertices[i];
        Vec2 normal = a.EdgeAt(i).Normal();

        float minSep = std::numeric_limits<float>::max();

        // Loop all the vertices of  "b"
        for (int j = 0; j < b.worldVertices.size(); j++)
        {
            Vec2 vb = b.worldVertices[j];
        	minSep = std::min(minSep, (vb - va).Dot(normal));
        }
        separation = std::max(separation, minSep);
    }

    return separation;
}

bool CollisionDetection::IsCollidingPolygonPolygon(Body* a, Body* b, Contact& contact)
{
    const PolygonShape* aPolygonShape = (PolygonShape*) a->shape;
    const PolygonShape* bPolygonShape = (PolygonShape*) b->shape;


	if (FindMinSeparation(*aPolygonShape, *bPolygonShape) >= 0)
	{
        return false;
	}

    if (FindMinSeparation(*bPolygonShape, *aPolygonShape) >= 0)
    {
        return false;
    }

    return true;
}

// TODO: Implement other methods to check collision between other different shapes