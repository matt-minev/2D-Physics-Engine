#ifndef FORCE_H
#define FORCE_H

#include "./Vec2.h"
#include "./Particle.h"

struct Force
{
	static Vec2 GenerateDragForce(const Particle& particle, float k);

	static Vec2 GenerateFrictionForce(const Particle& particle, float k);

	static Vec2 GenerateSpringForce(const Particle& particle, const Vec2& anchor, float restLength, float k);
	static Vec2 GenerateSpringForce(const Particle& a, const Particle& b, float restLength, float k);

	static Vec2 GenerateGravitationalForce(const Particle& a, const Particle& b, float G, float minDistance, float maxDistance);
};

#endif