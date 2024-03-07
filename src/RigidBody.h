#pragma once
#include "maths.h"

class RigidBody {

	vec2 m_position;
	double m_radius;
	vec2 m_velocity;
	vec2 m_acceleration;

public:

	RigidBody() {
	}

	virtual ~RigidBody() {}

	vec2 position() const { return m_position; }
	void setPosition(vec2 value) { m_position = value; }

	double radius() const { return m_radius; }
	void setRadius(double value) { m_radius = value; }

	vec2 velocity() const { return m_velocity; }
	void setVelocity(vec2 value) { m_velocity = value; }

	vec2 acceleration() const { return m_acceleration; }
	void setAcceleration(vec2 value) { m_acceleration = value; }

};
