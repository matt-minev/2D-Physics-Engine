#include "World.h"
#include "Constants.h"
#include "CollisionDetection.h"

#include <iostream>

World::World(float gravity)
{
	// Expected to enter a negative number (-9.8) for gravity because it is a downward force
	G = -gravity;
	std::cout << "World constructor called!" << std::endl;
}

World::~World()
{
	for (auto body : bodies)
	{
		delete body;
	}

	for (auto constraint : constraints)
	{
		delete constraint;
	}

	std::cout << "World destructor called!" << std::endl;
}

void World::AddBody(Body* body)
{
	bodies.push_back(body);
}

std::vector<Body*>& World::GetBodies()
{
	return bodies;
}

void World::AddConstraint(Constraint* constraint)
{
	constraints.push_back(constraint);
}

std::vector<Constraint*>& World::GetConstraints()
{
	return constraints;
}

void World::AddForce(const Vec2& force)
{
	forces.push_back(force);
}

void World::AddTorque(const float torque)
{
	torques.push_back(torque);
}

void World::Update(float dt)
{
	// Create a vector of constraints that will be solved frame per frame
	std::vector<PenetrationConstraint> penetrations;

	// Loop all the bodies of the world applying forces
	for (auto& body : bodies)
	{
		// Apply the "weight" force to all the bodies
		Vec2 weight = Vec2(0.0, body->mass * G * PIXELS_PER_METER);
		body->AddForce(weight);

		// Apply forces to all bodies
		for (auto force : forces)
		{
			body->AddForce(force);
		}

		// Apply torque to all bodies
		for (auto torque : torques)
		{
			body->AddTorque(torque);
		}
	}

	// Integrate all the forces
	for (auto& body: bodies)
	{
		body->IntegrateForces(dt);
	}

	// Check all the rigidbodies with the other rigidbodies for collision
	for (int i = 0; i <= bodies.size() - 1; i++)
	{
		for (int j = i + 1; j < bodies.size(); j++)
		{
			Body* a = bodies[i];
			Body* b = bodies[j];

			std::vector<Contact> contacts;

			if (CollisionDetection::IsColliding(a, b, contacts))
			{
				for (auto contact: contacts)
				{
					// Create a new penetration constraint
					PenetrationConstraint penetration(contact.a, contact.b, contact.start, contact.end, contact.normal);
					penetrations.push_back(penetration);
				}
			}
		}
	}

	// Solve all constraints
	// PreSolve Joint Constraints
	for (auto& constraint : constraints)
	{
		constraint->PreSolve(dt);
	}

	// PreSolve Penetration Constraints
	for (auto& constraint : penetrations)
	{
		constraint.PreSolve(dt);
	}

	// Solve all the constraints
	for (int i = 0; i < 9; i++)
	{
		for (auto& constraint : constraints) // joint constraints
		{
			constraint->Solve();
		}

		for (auto& constraint : penetrations) // penetration constraints
		{
			constraint.Solve();
		}
	}

	// Postsolve all the joint constraints
	for (auto& constraint : constraints)
	{
		constraint->PostSolve();
	}

	// Postsolve all the penetration constraints
	for (auto& constraint : penetrations)
	{
		constraint.PostSolve();
	}

	// Integrate all the velocities
	for (auto& body : bodies)
	{
		body->IntegrateVelocities(dt);
	}
}
