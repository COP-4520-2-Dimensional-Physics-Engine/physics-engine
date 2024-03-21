#include <thread>
#include <vector>

#include "PhysicsWorld.h"

void changeObjects(std::vector<RigidBody *> &objects, int start, int end, double dt) {
	for (int i = start; i < end; ++i) {
		objects[i]->setPosition(objects[i]->position() + objects[i]->velocity());
		objects[i]->setVelocity(objects[i]->velocity() + objects[i]->acceleration());
	}
}

void PhysicsWorld::positionIntegration(double dt) {
	const int numThreads = std::thread::hardware_concurrency();
	std::vector<std::thread> threads(numThreads);

	int chunkSize = ssize(bodies) / numThreads;
	int start = 0;
	for (int i = 0; i < numThreads; i++) {
		int end = start + chunkSize;
		if (i == numThreads - 1) {
			end = ssize(bodies);  // Assign the remaining objects to the last thread
		}
		threads[i] = std::thread(changeObjects, std::ref(bodies), start, end, dt);
		start = end;
	}

	for (auto & thread : threads) {
		thread.join();
	}
}

void PhysicsWorld::step(double dt) {
	positionIntegration(dt);

	for (auto it = begin(bodies); it != end(bodies); it++) {
		for (auto jt = next(it); jt != end(bodies); jt++) {
			if ((*it)->collides(*jt)) {
				vec2 normal = ((*it)->position() - (*jt)->position()).unit();
				(*it)->reflectVelocity(normal);
				(*jt)->reflectVelocity(normal);

				double overlap = (*it)->radius() + (*jt)->radius() - ((*it)->position() - (*jt)->position()).length();
				(*it)->setPosition((*it)->position() + normal * overlap / 2);
				(*jt)->setPosition((*jt)->position() - normal * overlap / 2);
			}
		}
	}
}

void PhysicsWorld::step(double dt, double fixedTimeStep) {
	lagAccumulator += dt;
	while (lagAccumulator >= fixedTimeStep) {
		step(fixedTimeStep);
		lagAccumulator -= fixedTimeStep;
	}
}
