#include "Shape.h"

#include <iostream>
#include <limits>

CircleShape::CircleShape(const float radius)
{
	this->radius = radius;
	std::cout << "CircleShape constructor called!" << std::endl;
}

CircleShape::~CircleShape()
{
	std::cout << "CircleShape destructor called!" << std::endl;
}

ShapeType CircleShape::GetType() const
{
	return CIRCLE;
}

Shape* CircleShape::Clone() const
{
	return new CircleShape(radius);
}

float CircleShape::GetMomentOfInertia() const
{
	// For solid circles, the moment of inertia is 1/2 * r^2
	// Still needs to be multiplied by the rigidbody's mass
	return 0.5 * (radius * radius);
}


PolygonShape::PolygonShape(const std::vector<Vec2> vertices)
{
	// Initialize the vertices of the polygon shape
	for (auto vertex: vertices)
	{
		localVertices.push_back(vertex);
		worldVertices.push_back(vertex);
	}

	std::cout << "PolygonShape constructor called!" << std::endl;
}

PolygonShape::~PolygonShape()
{
	std::cout << "PolygonShape destructor called!" << std::endl;
}

ShapeType PolygonShape::GetType() const
{
	return POLYGON;
}

Shape* PolygonShape::Clone() const
{
	return new PolygonShape(localVertices);
}

float PolygonShape::GetMomentOfInertia() const
{
	// TODO:
	// Compute the moment of inertia of the polygon correctly!
	return 5000.0f;
}

Vec2 PolygonShape::EdgeAt(int index) const
{
	int currVertex = index;
	int nextVertex = (index + 1) % worldVertices.size();

	return worldVertices[nextVertex] - worldVertices[currVertex];
}

float PolygonShape::FindMinSeparation(const PolygonShape* other, Vec2& axis, Vec2& point) const
{
	float separation = std::numeric_limits<float>::lowest();

	// Loop all the vertices of "this" polygon
	for (int i = 0; i < this->worldVertices.size(); i++)
	{
		Vec2 va = this->worldVertices[i];
		Vec2 normal = this->EdgeAt(i).Normal();

		float minSep = std::numeric_limits<float>::max();
		Vec2 minVertex;

		// Loop all the vertices of the "other" polygon
		for (int j = 0; j < other->worldVertices.size(); j++)
		{
			Vec2 vb = other->worldVertices[j];
			float proj = (vb - va).Dot(normal);
			if (proj < minSep)
			{
				minSep = proj;
				minVertex = vb;
			}
		}

		if (minSep > separation)
		{
			separation = minSep;
			axis = this->EdgeAt(i);
			point = minVertex;
		}
	}

	return separation;
}


// Function to rotate and translate polygon vertices from "local space" to "world space"
void PolygonShape::UpdateVertices(float angle, const Vec2& position)
{
	// Loop all the vertices, transforming from local to world space
	for (int i = 0; i < localVertices.size(); i++)
	{
		// Rotate
		worldVertices[i] = localVertices[i].Rotate(angle);

		// Translate
		worldVertices[i] += position;
	}
}


BoxShape::BoxShape(float width, float height)
{
	this->width = width;
	this->height = height;

	// Load the local vertices of the box polygon
	localVertices.push_back(Vec2(-width / 2.0, -height / 2.0)); // top left
	localVertices.push_back(Vec2(+width / 2.0, -height / 2.0)); // top right
	localVertices.push_back(Vec2(+width / 2.0, +height / 2.0)); // bottom right
	localVertices.push_back(Vec2(-width / 2.0, +height / 2.0)); // bottom left

	worldVertices.push_back(Vec2(-width / 2.0, -height / 2.0)); // top left
	worldVertices.push_back(Vec2(+width / 2.0, -height / 2.0)); // top right
	worldVertices.push_back(Vec2(+width / 2.0, +height / 2.0)); // bottom right
	worldVertices.push_back(Vec2(-width / 2.0, +height / 2.0)); // bottom left
}

BoxShape::~BoxShape()
{
	//TODO: ...
}

ShapeType BoxShape::GetType() const
{
	return BOX;
}

Shape* BoxShape::Clone() const
{
	return new BoxShape(width, height);
}

float BoxShape::GetMomentOfInertia() const
{
	// For a rectangle, the moment of inertia is 1/12 * (w^2 + h^2)
	// Still needs to be multiplied by the rigidbody's mass
	return (0.083333) * (width * width + height * height);
}

