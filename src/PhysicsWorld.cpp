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

void PhysicsWorld::collisionDetectionAndResponse() {
	for (auto it = begin(bodies); it != end(bodies); it++) {
		auto start = next(it);
		int chunkSize = std::distance(start, end(bodies)) / threadCount;

		for (int i = 0; i < threadCount; i++) {
			auto end = i < threadCount - 1 ? start + chunkSize : std::end(bodies);

			threadPool.enqueue([it, start, end] {
				for (auto jt = start; jt != end; jt++) {
					if ((*it)->collides(*jt)) {

						vec2 normal = ((*it)->position() - (*jt)->position()).unit();
						(*it)->reflectVelocity(normal);
						(*jt)->reflectVelocity(normal);

						double overlap = (*it)->radius() + (*jt)->radius() - ((*it)->position() - (*jt)->position()).length();
						(*it)->setPosition((*it)->position() + normal * overlap / 2);
						(*jt)->setPosition((*jt)->position() - normal * overlap / 2);

					}
				}
			});

			start = end;
		}

		threadPool.flush();
	}
}

PhysicsWorld::PhysicsWorld() : threadPool(threadCount), lagAccumulator(0) {}

void PhysicsWorld::step(double dt) {
	positionIntegration(dt);

	collisionDetectionAndResponse();
}

void PhysicsWorld::step(double dt, double fixedTimeStep) {
	lagAccumulator += dt;
	while (lagAccumulator >= fixedTimeStep) {
		step(fixedTimeStep);
		lagAccumulator -= fixedTimeStep;
	}
}
