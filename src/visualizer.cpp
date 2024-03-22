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

double scale = 10;
vec2 offset;

static void render() {
	ClearBackground(RAYWHITE);

	for (size_t i = 0; i < renderBodies.size(); ++i) {
		RigidBody *body = renderBodies[i];
		vec2 position = body->position() * scale * vec2(1, -1) + offset
			+ vec2(GetScreenWidth() / 2.0, GetScreenHeight() / 2.0);
		double radius = body->radius() * scale;

		// if there's no way we can see the body, don't even render it
		if (position.x < -radius || position.y < -radius
				|| position.x > GetScreenWidth() + radius || position.y > GetScreenHeight() + radius)
			continue;

		DrawCircle(position.x, position.y, radius, renderColors[i]);
	}
}

std::random_device rdev;
std::mt19937 rng(rdev());

static int placeFrame = -30;

static void processInput() {
	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
		// place a body every 5 frames (1/12th of a second)
		if (placeFrame == -30 || placeFrame >= 0 && placeFrame % 5 == 0) {
			vec2 mouseScreen = vec2(GetMouseX(), GetMouseY());
			vec2 halfScreen = vec2(GetScreenWidth() / 2.0, GetScreenHeight() / 2.0);
			mouseScreen = mouseScreen - halfScreen;
			vec2 mouseWorld = (mouseScreen - offset) * vec2(1, -1) / scale;

			RigidBody *body = new RigidBody();
			body->setPosition(mouseWorld);
			body->setRadius(10);

			// for testing purposes
			body->setAcceleration(vec2(0, -196.2));

			auto colorDistr = std::uniform_int_distribution<int>(0, 255);

			world->add(body);
			renderBodies.push_back(body);
			renderColors.push_back(Color {
				.r = (unsigned char)colorDistr(rng),
				.g = (unsigned char)colorDistr(rng),
				.b = (unsigned char)colorDistr(rng),
				.a = 255
			});
		}

		placeFrame += 1;
	}
	else {
		placeFrame = -30;
	}

	Vector2 delta = GetMouseDelta();

	if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE) || IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
		offset = offset + vec2(delta.x, delta.y);
	}

	Vector2 wheel = GetMouseWheelMoveV();

	double newScale = scale * pow(1.2, wheel.y);
	if (newScale != scale) {

		// when we zoom out, we want to preserve the world space location of the mouse cursor
		vec2 mouseScreen = vec2(GetMouseX(), GetMouseY());
		vec2 halfScreen = vec2(GetScreenWidth() / 2.0, GetScreenHeight() / 2.0);
		mouseScreen = mouseScreen - halfScreen;
		offset = mouseScreen - (mouseScreen - offset) * newScale / scale;

		scale = newScale;
	}
}

static void generateRandomStuff() {
	auto positionDistr = std::uniform_real_distribution(-100.0, 100.0);
	auto sizeDistr = std::uniform_real_distribution(1.0, 10.0);
	auto colorDistr = std::uniform_int_distribution<int>(0, 255);

	for (int i = 0; i < 1000; ++i) {
		RigidBody *body = new RigidBody();
		body->setPosition(vec2(positionDistr(rng), positionDistr(rng)));
		body->setRadius(sizeDistr(rng));

		// for testing purposes
		body->setAcceleration(vec2(0, 5));

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

		// process input
		processInput();

		// draw the state of the simulation
		BeginDrawing();
		render();
		EndDrawing();
	}

	CloseWindow();
}

}
