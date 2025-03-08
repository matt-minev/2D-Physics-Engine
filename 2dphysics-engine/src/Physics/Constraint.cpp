#include "Constraint.h"

///////////////////////////////////////////////////////////////////////////////
// Mat6x6 with the all inverse mass and inverse I of bodies "a" and "b"
///////////////////////////////////////////////////////////////////////////////
//  [ 1/ma  0     0     0     0     0    ]
//  [ 0     1/ma  0     0     0     0    ]
//  [ 0     0     1/Ia  0     0     0    ]
//  [ 0     0     0     1/mb  0     0    ]
//  [ 0     0     0     0     1/mb  0    ]
//  [ 0     0     0     0     0     1/Ib ]
///////////////////////////////////////////////////////////////////////////////
MatMN Constraint::GetInvM() const
{
	MatMN invM(6, 6);

	invM.Zero();

	invM.rows[0][0] = a->invMass;
	invM.rows[1][1] = a->invMass;
	invM.rows[2][2] = a->invI;

	invM.rows[3][3] = b->invMass;
	invM.rows[4][4] = b->invMass;
	invM.rows[5][5] = b->invI;

	return invM;
}

///////////////////////////////////////////////////////////////////////////////
// VecN with the all linear and angular velocities of bodies "a" and "b"
///////////////////////////////////////////////////////////////////////////////
//  [ va.x ]
//  [ va.y ]
//  [ ωa   ]
//  [ vb.x ]
//  [ vb.y ]
//  [ ωb   ]
///////////////////////////////////////////////////////////////////////////////
VecN Constraint::GetVelocities() const
{
	VecN V(6);

	V.Zero();

	V[0] = a->velocity.x;
	V[1] = a->velocity.y;
	V[2] = a->angularVelocity;

	V[3] = b->velocity.x;
	V[4] = b->velocity.y;
	V[5] = b->angularVelocity;

	return V;
}

JointConstraint::JointConstraint() : Constraint(), jacobian(1, 6)
{
}

JointConstraint::JointConstraint(Body* a, Body* b, const Vec2& anchorPoint) : Constraint(), jacobian(1, 6)
{
	this->a = a;
	this->b = b;

	// Convert anchor point to local space
	this->aPoint = a->WorldSpaceToLocalSpace(anchorPoint);
	this->bPoint = b->WorldSpaceToLocalSpace(anchorPoint);
}

void JointConstraint::Solve()
{
	// Load the Jacobian matrix

	// V = GetVelocities();
	// invM = GetInvM();

	// Calculate lambda -> The impulse to apply to objects A & B

	// Apply the lambda impulse to both A & B
}
