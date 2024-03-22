#include "PhysicsWorld.h"
#include <iostream>

void integratePosition(std::vector<RigidBody *> &bodies, int start, int end, double dt) {
	for (int i = start; i < end; i++) {
		bodies[i]->setPosition(bodies[i]->position() + bodies[i]->velocity() * dt);
		bodies[i]->setVelocity(bodies[i]->velocity() + bodies[i]->acceleration() * dt);
	}
}

void PhysicsWorld::positionIntegration(double dt) {
	int chunkSize = ssize(bodies) / threadCount;
	int start = 0;
	for (int i = 0; i < threadCount; i++) {
		int end = i < threadCount - 1 ? start + chunkSize : ssize(bodies);
		threadPool.enqueue(integratePosition, std::ref(bodies), start, end, dt);
		start = end;
	}

	threadPool.wait();
}

void detectCollisions(std::vector<std::array<RigidBody *, 2>> &collisions, std::mutex &mut, size_t start, size_t end, std::vector<RigidBody *> &bodies) {
	for (size_t index = start; index < end; index++) {
		size_t i = index / bodies.size();
		size_t j = index % bodies.size();
		if (i >= j)
			continue;
		if (bodies[i]->collides(bodies[j])) {
			std::unique_lock<std::mutex> lock(mut);
			collisions.push_back({bodies[i], bodies[j]});
		}
	}
}

std::vector<std::array<RigidBody *, 2>> PhysicsWorld::collisionDetection() {
	std::vector<std::array<RigidBody *, 2>> collisions;
	std::mutex mut;

	size_t numPairs = bodies.size() * bodies.size();

	size_t chunkSize = numPairs / threadCount;
	size_t start = 0;
	for (int i = 0; i < threadCount; i++) {
		size_t end = i < threadCount - 1 ? start + chunkSize : numPairs;
		threadPool.enqueue(detectCollisions, std::ref(collisions), std::ref(mut), start, end, std::ref(bodies));
		start = end;
	}

	threadPool.wait();

	return collisions;
}

void PhysicsWorld::collisionResponse(std::vector<std::array<RigidBody *, 2>> &collisions) {
	for (auto [a, b] : collisions) {
		vec2 normal = (a->position() - b->position()).unit();
		a->reflectVelocity(normal);
		b->reflectVelocity(normal);

		double overlap = a->radius() + b->radius() - (a->position() - b->position()).length();
		a->setPosition(a->position() + normal * overlap / 2);
		b->setPosition(b->position() - normal * overlap / 2);
	}
}

PhysicsWorld::PhysicsWorld() : threadPool(threadCount), lagAccumulator(0) {}

void PhysicsWorld::step(double dt) {
	positionIntegration(dt);

  auto collisions = collisionDetection();

	collisionResponse(collisions);
}

void PhysicsWorld::step(double dt, double fixedTimeStep) {
	lagAccumulator += dt;
	while (lagAccumulator >= fixedTimeStep) {
		step(fixedTimeStep);
		lagAccumulator -= fixedTimeStep;
	}
}
