#pragma once

class PhysicsWorld {

	double lagAccumulator;

public:

	void step(double dt);
	void step(double dt, double fixedTimeStep);

};
