#include <thread>
#include <vector>
#include "PhysicsWorld.h"

void integratePosition(std::vector<RigidBody *> &objects, int start, int end, double dt) {
	for (int i = start; i < end; i++) {
		objects[i]->setPosition(objects[i]->position() + objects[i]->velocity() * dt);
		objects[i]->setVelocity(objects[i]->velocity() + objects[i]->acceleration() * dt);
	}
}

void PhysicsWorld::positionIntegration(double dt) {
	std::vector<std::thread> threads(numThreads);

	int chunkSize = ssize(bodies) / numThreads;
	int start = 0;
	for (int i = 0; i < numThreads; i++) {
		int end = i < numThreads - 1 ? start + chunkSize : ssize(bodies);
		threads[i] = std::thread(integratePosition, std::ref(bodies), start, end, dt);
		start = end;
	}

	for (auto & thread : threads) {
		thread.join();
	}
}

void PhysicsWorld::collisionDetectionAndResponse() {
	for (auto it = begin(bodies); it != end(bodies); it++) {
		std::vector<std::thread> threads(numThreads);

		int chunkSize = (std::distance(it, end(bodies)) - 1) / numThreads;
		auto start = next(it);
		for (int i = 0; i < numThreads; i++) {
			auto end = i < numThreads - 1 ? start + chunkSize : std::end(bodies);
			threads[i] = std::thread([it, start, end] {
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

		for (auto & thread : threads) {
			thread.join();
		}
	}
}

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
