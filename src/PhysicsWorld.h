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

	std::vector<std::array<RigidBody *, 2>> collisionDetection();

	void collisionResponse(std::vector<std::array<RigidBody *, 2>> &collisions);

public:
	PhysicsWorld();

	void step(double dt);
	void step(double dt, double fixedTimeStep);

	void add(RigidBody *body) {
		bodies.push_back(body);
	}

};
