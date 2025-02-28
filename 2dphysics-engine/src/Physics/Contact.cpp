#include "Contact.h"

void Contact::ResolvePenetration()
{
    if (a->IsStatic() && b->IsStatic())
    {
        return;
    }

    float da = depth / (a->invMass + b->invMass) * a->invMass;
    float db = depth / (a->invMass + b->invMass) * b->invMass;

    a->position -= normal * da;
    b->position += normal * db;
}

// Resolves the collision using the impulse method
void Contact::ResolveCollision() {
    // Apply positional correction using the projection method
    ResolvePenetration();

    // Define elasticity (coefficient of restitution e)
    float e = std::min(a->restitution, b->restitution);
	float f = std::min(a->friction, b->friction);

    // Calculate the relative velocity between the two objects
	Vec2 ra = end - a->position;
	Vec2 rb = start - b->position;
    Vec2 va = a->velocity + Vec2(-a->angularVelocity * ra.y, a->angularVelocity * ra.x);
	Vec2 vb = b->velocity + Vec2(-b->angularVelocity * rb.y, b->angularVelocity * rb.x);
    const Vec2 vrel = va - vb;

    // Calculate the collision impulse along the normal
    float vrelDotNormal = vrel.Dot(normal);
    const Vec2 impulseDirectionN = normal;
    const float impulseMagnitudeN = -(1 + e) * vrelDotNormal / ((a->invMass + b->invMass) + ra.Cross(normal) * ra.Cross(normal) * a->invI + rb.Cross(normal) * rb.Cross(normal) * b->invI); 
    Vec2 jN = impulseDirectionN * impulseMagnitudeN;

	// Calculate the friction impulse along the tangent
    Vec2 tangent = normal.Normal();
	float vrelDotTangent = vrel.Dot(tangent);
	const Vec2 impulseDirectionT = tangent;
	const float impulseMagnitudeT = f * -(1 + e) * vrelDotTangent / ((a->invMass + b->invMass) + ra.Cross(tangent) * ra.Cross(tangent) * a->invI + rb.Cross(tangent) * rb.Cross(tangent) * b->invI);
	Vec2 jT = impulseDirectionT * impulseMagnitudeT;

	// Calculate the final impulse j by combining the normal and tangent (friction) impulses
	Vec2 j = jN + jT;

    // Apply the impulse vector to both objects in opposite directions
    a->ApplyImpulse(j, ra);
    b->ApplyImpulse(-j, rb);
}


