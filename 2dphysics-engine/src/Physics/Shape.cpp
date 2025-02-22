#include "Shape.h"

#include <iostream>

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


PolygonShape::PolygonShape(const std::vector<Vec2> vertices)
{
	//TODO: ...
}

PolygonShape::~PolygonShape()
{
	//TODO: ...
}

ShapeType PolygonShape::GetType() const
{
	return POLYGON;
}

Shape* PolygonShape::Clone() const
{
	return new PolygonShape(vertices);
}


BoxShape::BoxShape(float width, float height)
{
	//TODO: ...
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

