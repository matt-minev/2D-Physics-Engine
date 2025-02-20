#ifndef FORCE_H
#define FORCE_H

#include "./Vec2.h"
#include "./Particle.h"

struct Force
{
	static Vec2 GenerateDragForce(const Particle& particle, float k);

	static Vec2 GenerateFrictionForce(const Particle& particle, float k);
};

#endif