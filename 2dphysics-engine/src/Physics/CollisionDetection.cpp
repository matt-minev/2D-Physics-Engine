#include "CollisionDetection.h"

#include <limits>

bool CollisionDetection::IsColliding(Body* a, Body* b, Contact& contact)
{
    bool aIsCircle = a->shape->GetType() == CIRCLE;
    bool bIsCircle = b->shape->GetType() == CIRCLE;

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

    if (aIsPolygon && bIsCircle) 
    {
        return IsCollidingPolygonCircle(a, b, contact);
    }

    if (aIsCircle && bIsPolygon) 
    {
        return IsCollidingPolygonCircle(b, a, contact);
    }

    return false;
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

    Vec2 aAxis, bAxis;
    Vec2 aPoint, bPoint;

    float abSeparation = aPolygonShape->FindMinSeparation(bPolygonShape, aAxis, aPoint);
    if (abSeparation >= 0)
	{
        return false;
	}

    float baSeparation = bPolygonShape->FindMinSeparation(aPolygonShape, bAxis, bPoint);
    if (baSeparation >= 0)
    {
        return false;
    }

    // Populate the Contact information
    contact.a = a;
    contact.b = b;

    if (abSeparation > baSeparation)
    {
        contact.depth = -abSeparation;
        contact.normal = aAxis.Normal();
        contact.start = aPoint;
        contact.end = aPoint + contact.normal * contact.depth;
    }
    else
    {
        contact.depth = -baSeparation;
        contact.normal = -bAxis.Normal();
        contact.start = bPoint - contact.normal * contact.depth;
        contact.end = bPoint;
    }

    return true;
}

bool CollisionDetection::IsCollidingPolygonCircle(Body* polygon, Body* circle, Contact& contact)
{
    const PolygonShape* polygonShape = (PolygonShape*)polygon->shape;
    const CircleShape* circleShape = (CircleShape*)circle->shape;
    const std::vector<Vec2>& polygonVertices = polygonShape->worldVertices;

    bool isOutside = false;
    Vec2 minCurrVertex;
    Vec2 minNextVertex;
    float distanceCircleEdge = std::numeric_limits<float>::lowest();

    // Loop all the edges of the polygon/box finding the nearest edge to the circle center
    for (int i = 0; i < polygonVertices.size(); i++) 
    {
        int currVertex = i;
        int nextVertex = (i + 1) % polygonVertices.size();
        Vec2 edge = polygonShape->EdgeAt(currVertex);
        Vec2 normal = edge.Normal();

        // Compare the circle center with the rectangle vertex
        Vec2 vertexToCircleCenter = circle->position - polygonVertices[currVertex];
        float projection = vertexToCircleCenter.Dot(normal);

        // If found a dot product projection that is in the positive/outside side of the normal
        if (projection > 0) 
        {
            // Circle center is outside the polygon
            distanceCircleEdge = projection;
            minCurrVertex = polygonVertices[currVertex];
            minNextVertex = polygonVertices[nextVertex];
            isOutside = true;
            break;
        }
        else 
        {
            // Circle center is inside the rectangle, find the min edge (the one with the least negative projection)
            if (projection > distanceCircleEdge) 
            {
                distanceCircleEdge = projection;
                minCurrVertex = polygonVertices[currVertex];
                minNextVertex = polygonVertices[nextVertex];
            }
        }
    }

    if (isOutside) {
        ///////////////////////////////////////
        // Check if we are inside region A:
        ///////////////////////////////////////
        Vec2 v1 = circle->position - minCurrVertex; // vector from the nearest vertex to the circle center
        Vec2 v2 = minNextVertex - minCurrVertex; // the nearest edge (from curr vertex to next vertex)
        if (v1.Dot(v2) < 0) 
        {
            // Distance from vertex to circle center is greater than radius... no collision
            if (v1.Magnitude() > circleShape->radius) 
            {
                return false;
            }
            else 
            {
                // Detected collision in region A:
                contact.a = polygon;
                contact.b = circle;
                contact.depth = circleShape->radius - v1.Magnitude();
                contact.normal = v1.Normalize();
                contact.start = circle->position + (contact.normal * -circleShape->radius);
                contact.end = contact.start + (contact.normal * contact.depth);
            }
        }
        else {
            ///////////////////////////////////////
            // Check if we are inside region B:
            ///////////////////////////////////////
            v1 = circle->position - minNextVertex; // vector from the next nearest vertex to the circle center
            v2 = minCurrVertex - minNextVertex;   // the nearest edge
            if (v1.Dot(v2) < 0) 
            {
                // Distance from vertex to circle center is greater than radius... no collision
                if (v1.Magnitude() > circleShape->radius) 
                {
                    return false;
                }
                else 
                {
                    // Detected collision in region B:
                    contact.a = polygon;
                    contact.b = circle;
                    contact.depth = circleShape->radius - v1.Magnitude();
                    contact.normal = v1.Normalize();
                    contact.start = circle->position + (contact.normal * -circleShape->radius);
                    contact.end = contact.start + (contact.normal * contact.depth);
                }
            }
            else 
            {
                ///////////////////////////////////////
                // Inside region C:
                ///////////////////////////////////////
                if (distanceCircleEdge > circleShape->radius) 
                {
                    // No collision... Distance between the closest distance and the circle center is greater than the radius.
                    return false;
                }
                else 
                {
                    // Detected collision in region C:
                    contact.a = polygon;
                    contact.b = circle;
                    contact.depth = circleShape->radius - distanceCircleEdge;
                    contact.normal = (minNextVertex - minCurrVertex).Normal();
                    contact.start = circle->position - (contact.normal * circleShape->radius);
                    contact.end = contact.start + (contact.normal * contact.depth);
                }
            }
        }
    }
    else 
    {
        // The center of circle is inside the polygon... it is definitely colliding!
        contact.a = polygon;
        contact.b = circle;
        contact.depth = circleShape->radius - distanceCircleEdge;
        contact.normal = (minNextVertex - minCurrVertex).Normal();
        contact.start = circle->position - (contact.normal * circleShape->radius);
        contact.end = contact.start + (contact.normal * contact.depth);
    }

    return true;
}
