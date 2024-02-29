#include "visualizer.h"
#include "PhysicsWorld.h"
#include <raylib.h>

static const int defaultWidth = 640;
static const int defaultHeight = 480;
static const int tickRate = 120;

namespace visualizer {

static void render() {
	ClearBackground(RAYWHITE);

	DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
}

void run() {
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(defaultWidth, defaultHeight, "Physics Visualizer");
	SetTargetFPS(60);

	PhysicsWorld *world = new PhysicsWorld();

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
