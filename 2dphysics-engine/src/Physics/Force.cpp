#include "./Force.h"

#include <algorithm>

Vec2 Force::GenerateDragForce(const Particle& particle, float k)
{
    Vec2 dragForce = Vec2(0, 0);
    if (particle.velocity.MagnitudeSquared() > 0)
    {
	    // Calculate drag direction (inverse of velocity unit vector)
        Vec2 dragDirection = particle.velocity.UnitVector() * -1.0;

        // Calculate the drag magnitude, k * |v|^2
        float dragMagnitude = k * particle.velocity.MagnitudeSquared();

        // Generate the final drag force with direction and magnitude
        dragForce = dragDirection * dragMagnitude;
    }

    return dragForce;
}

Vec2 Force::GenerateFrictionForce(const Particle& particle, float k)
{
    Vec2 frictionForce = Vec2(0, 0);

    // Calculate the friction direction (inverse of velocity unit vector)
    Vec2 frictionDirection = particle.velocity.UnitVector() * -1.0;

    // Calculate the friction magnitude (simply a constant)
    float frictionMagnitude = k;

    // Calculate the final friction force
    frictionForce = frictionDirection * frictionMagnitude;

    return frictionForce;
}

Vec2 Force::GenerateSpringForce(const Particle& particle, const Vec2& anchor, float restLength, float k)
{
    // Calculate the distance between the anchor and the object
    Vec2 d = particle.position - anchor;

    // Find the spring displacement considering the rest length
    float displacement = d.Magnitude() - restLength;

    // Calculate the direction and the magnitude of the spring force
    Vec2 springDirection = d.UnitVector();
    float springMagnitude = -k * displacement;

    // Calculate the final resulting spring force
    Vec2 springForce = springDirection * springMagnitude;

    return springForce;
}

Vec2 Force::GenerateSpringForce(const Particle& a, const Particle& b, float restLength, float k)
{
    // Calculate the distance between the anchor and the object
    Vec2 d = a.position - b.position;

    // Find the spring displacement considering the rest length
    float displacement = d.Magnitude() - restLength;

    // Calculate the direction and the magnitude of the spring force
    Vec2 springDirection = d.UnitVector();
    float springMagnitude = -k * displacement;

    // Calculate the final resulting spring force
    Vec2 springForce = springDirection * springMagnitude;

    return springForce;
}

Vec2 Force::GenerateGravitationalForce(const Particle& a, const Particle& b, float G, float minDistance, float maxDistance)
{
    // Calculate the distance between the two objects
    Vec2 d = (b.position - a.position);

    float distanceSquared = d.MagnitudeSquared();

    // Clamp the values of the distance (to allow for some interesting visual effects)
    distanceSquared = std::clamp(distanceSquared, minDistance, maxDistance);

    // Calculate the direction of the attraction force
    Vec2 attractionDirection = d.UnitVector();

    // Calculate the strength of the attraction force
    float attractionMagnitude = G * (a.mass * b.mass) / distanceSquared;

    // Calculate the final resulting attraction force vector
    Vec2 attractionForce = attractionDirection * attractionMagnitude;

    return attractionForce;
}
