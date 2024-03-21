#pragma once
#include <vector>
#include "RigidBody.h"

class PhysicsWorld {

	double lagAccumulator;

	std::vector<RigidBody *> bodies;

	void positionIntegration(double dt);

public:

	void step(double dt);
	void step(double dt, double fixedTimeStep);

	void add(RigidBody *body) {
		bodies.push_back(body);
	}

};
