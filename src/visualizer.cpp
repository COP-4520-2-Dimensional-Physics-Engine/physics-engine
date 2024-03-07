#include "visualizer.h"
#include "PhysicsWorld.h"
#include <raylib.h>
#include <random>

static const int defaultWidth = 640;
static const int defaultHeight = 480;
static const int tickRate = 120;

namespace visualizer {

static PhysicsWorld *world;
static std::vector<RigidBody *> renderBodies;
static std::vector<Color> renderColors;

static void render() {
	ClearBackground(RAYWHITE);

	double scale = 10;
	vec2 offset;

	for (size_t i = 0; i < renderBodies.size(); ++i) {
		RigidBody *body = renderBodies[i];
		vec2 position = body->position() * scale + offset
			+ vec2(GetScreenWidth() / 2.0, GetScreenHeight() / 2.0);
		double radius = body->radius() * scale;
		DrawCircle(position.x, position.y, radius, renderColors[i]);
	}
}

static void generateRandomStuff() {
	std::random_device rdev;
	std::mt19937 rng(rdev());

	auto positionDistr = std::uniform_real_distribution(-100.0, 100.0);
	auto sizeDistr = std::uniform_real_distribution(1.0, 10.0);
	auto colorDistr = std::uniform_int_distribution<int>(0, 255);

	for (int i = 0; i < 100; ++i) {
		RigidBody *body = new RigidBody();
		body->setPosition(vec2(positionDistr(rng), positionDistr(rng)));
		body->setRadius(sizeDistr(rng));

		// for testing purposes
		body->setAcceleration(vec2(0, 0.001));

		world->add(body);
		renderBodies.push_back(body);
		renderColors.push_back(Color {
			.r = (unsigned char)colorDistr(rng),
			.g = (unsigned char)colorDistr(rng),
			.b = (unsigned char)colorDistr(rng),
			.a = 255
		});
	}
}

void run() {
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(defaultWidth, defaultHeight, "Physics Visualizer");
	SetTargetFPS(60);

	world = new PhysicsWorld();

	generateRandomStuff();

	while (!WindowShouldClose()) {
		// step physics simulation
		world->step(GetFrameTime(), 1.0 / tickRate);

		// draw the state of the simulation
		BeginDrawing();
		render();
		EndDrawing();
	}

	CloseWindow();
}

}
