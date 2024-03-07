#include "PhysicsWorld.h"

void PhysicsWorld::step(double dt) {
	for (RigidBody *body : bodies) {
		body->setPosition(body->position() + body->velocity());
		body->setVelocity(body->velocity() + body->acceleration());
	}
}

void PhysicsWorld::step(double dt, double fixedTimeStep) {
	lagAccumulator += dt;
	while (lagAccumulator >= fixedTimeStep) {
		step(fixedTimeStep);
		lagAccumulator -= fixedTimeStep;
	}
}
