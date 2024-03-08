#include "PhysicsWorld.h"

void PhysicsWorld::step(double dt) {
	for (RigidBody *body : bodies) {
		body->setPosition(body->position() + body->velocity());
		body->setVelocity(body->velocity() + body->acceleration());
	}

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
