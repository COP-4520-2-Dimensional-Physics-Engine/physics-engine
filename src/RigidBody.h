#pragma once
#include "maths.h"

class RigidBody {

	vec2 m_position;
	double m_radius;

public:

	RigidBody() {
	}

	virtual ~RigidBody() {}

	vec2 position() const { return m_position; }
	void setPosition(vec2 value) { m_position = value; }

	double radius() const { return m_radius; }
	void setRadius(double value) { m_radius = value; }

};
