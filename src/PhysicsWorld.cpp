#include "PhysicsWorld.h"

void PhysicsWorld::step(double dt) {
	for (RigidBody *body : bodies) {
		body->setPosition(body->position() + body->velocity());
		body->setVelocity(body->velocity() + body->acceleration());
	}

	for (auto it = begin(bodies); it != end(bodies); it++) {
		for (auto jt = next(it); jt != end(bodies); jt++) {
			if ((*it)->collides(*jt)) {
				// TODO: implement collision response
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
