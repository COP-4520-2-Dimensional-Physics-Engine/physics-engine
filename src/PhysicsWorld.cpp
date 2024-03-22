#include "PhysicsWorld.h"

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

	threadPool.flush();
}

std::vector<std::array<RigidBody *, 2>> PhysicsWorld::collisionDetection() {
	std::vector<std::array<RigidBody *, 2>> collisions;

	std::mutex collisionsMutex;

	for (int i = 0; i < ssize(bodies); i++) {

		for (int j = i + 1; j < ssize(bodies); j++) {

			threadPool.enqueue([&] {

				if ((bodies[i]->collides(bodies[j]))) {

					std::lock_guard<std::mutex> lock(collisionsMutex);

					collisions.push_back({bodies[i], bodies[j]});

				}

			});

		}

	}

	threadPool.flush();

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
