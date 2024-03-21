#pragma once
#include <vector>
#include <thread>
#include "RigidBody.h"

class PhysicsWorld {
	const int numThreads = std::thread::hardware_concurrency();

	double lagAccumulator;

	std::vector<RigidBody *> bodies;

	void positionIntegration(double dt);

	void collisionDetectionAndResponse();

public:

	void step(double dt);
	void step(double dt, double fixedTimeStep);

	void add(RigidBody *body) {
		bodies.push_back(body);
	}

};
