#include "Body.h"
#include "../Graphics.h"

#include <iostream>
#include <cmath>

Body::Body(const Shape& shape, float x, float y, float mass)
{
	this->shape = shape.Clone();
	this->position = Vec2(x, y);
	this->velocity = Vec2(0, 0);
	this->acceleration = Vec2(0, 0);
	this->rotation = 0.0;
	this->angularVelocity = 0.0;
	this->angularAcceleration = 0.0;
	this->sumForces = Vec2(0, 0);
	this->sumTorque = 0.0;
	this->restitution = 1.0;
	this->friction = 0.7;

	this->mass = mass;
	if (mass != 0.0)
	{
		this->invMass = 1.0 / mass;
	}
	else
	{
		this->invMass = 0.0;
	}

	I = shape.GetMomentOfInertia() * mass;
	if (I != 0.0)
	{
		this->invI = 1.0 / I;
	}
	else
	{
		this->invI = 0.0;
	}

	std::cout << "Body constructor called!" << std::endl;
}

Body::~Body()
{
	// Deallocate shape
	delete shape;

	// Deallocate texture
	SDL_DestroyTexture(texture);

	std::cout << "Body destructor called!" << std::endl;
}

void Body::SetTexture(const char* textureFileName)
{
	// Load the file from "textureFileName" as an SDL_Texture
	SDL_Surface* surface = IMG_Load(textureFileName);
	if (surface)
	{
		texture = SDL_CreateTextureFromSurface(Graphics::renderer, surface);
		SDL_FreeSurface(surface);
	}
	else
	{
		std::cerr << "Error loading texture: " << textureFileName << std::endl;
	}
}

bool Body::IsStatic() const
{
	const float epsilon = 0.005f;
	return fabs(invMass - 0.0) < epsilon;
}

void Body::AddForce(const Vec2& force)
{
	sumForces += force;
}

void Body::AddTorque(float torque)
{
	sumTorque += torque;
}


void Body::ClearForces()
{
	sumForces = Vec2(0.0, 0.0);
}

void Body::ClearTorque()
{
	sumTorque = 0.0;
}

void Body::ApplyImpulse(const Vec2& j)
{
	if (IsStatic())
	{
		return;
	}

	velocity += j * invMass;
}

void Body::ApplyImpulse(const Vec2& j, const Vec2& r)
{
	if (IsStatic())
	{
		return;
	}

	velocity += j * invMass;
	angularVelocity += r.Cross(j) * invI;
}


void Body::IntegrateLinear(float dt)
{
	if (IsStatic())
	{
		return;
	}

	// Find the linear acceleration based on the forces that are being applied and the mass
	acceleration = sumForces * invMass;

	// Implicit Euler Integration
	// Integrate the linear acceleration to find the new linear velocity
	velocity += acceleration * dt;

	// Integrate the velocity to find the new position
	position += velocity * dt;

	// Clear all the forces acting on the object before the next physics step
	ClearForces();
}

void Body::IntegrateAngular(float dt)
{
	if (IsStatic())
	{
		return;
	}

	// Find the angular acceleration based on the torque that is being applied and the moment of inertia
	angularAcceleration = sumTorque * invI;

	// Implicit Euler Integration
	// Integrate the angular acceleration to find the new angular velocity
	angularVelocity += angularAcceleration * dt;

	// Integrate the angular velocity to find the new angular rotation angle
	rotation += angularVelocity * dt;

	// Clear all the torque acting on the object before the next physics step
	ClearTorque();
}

void Body::Update(float dt)
{
	IntegrateLinear(dt);
	IntegrateAngular(dt);

	shape->UpdateVertices(rotation, position);
}
