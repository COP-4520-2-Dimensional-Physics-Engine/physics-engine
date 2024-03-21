#pragma once
#include <vector>
#include "RigidBody.h"
#include "ThreadPool.h"

class PhysicsWorld {
	const int threadCount = std::thread::hardware_concurrency();

	ThreadPool threadPool;

	double lagAccumulator;

	std::vector<RigidBody *> bodies;

	void positionIntegration(double dt);

	void collisionDetectionAndResponse();

public:
	PhysicsWorld();

	void step(double dt);
	void step(double dt, double fixedTimeStep);

	void add(RigidBody *body) {
		bodies.push_back(body);
	}

};
