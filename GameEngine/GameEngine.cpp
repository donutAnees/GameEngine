// GameEngine.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "engine.h"
#include <sstream>
#include <chrono>
#include <cmath>

#define SCREEN_WIDTH 960.0f
#define SCREEN_HEIGHT 520.0f

#define DBOUT( s )            \
{                             \
   std::wostringstream os_;    \
   os_ << s;                   \
   OutputDebugString( os_.str().c_str() );  \
}

std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();
const float radius = 20.0f;

float getElapsedTime() {
	auto now = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> elapsed = now - startTime;  // Time elapsed in seconds
	return elapsed.count();
}

class MainGame : public engine {
private:
	Mesh mesh;
	Cube cube = Cube(0, 0, 0, 1);
public:
	MainGame() : engine(SCREEN_WIDTH, SCREEN_HEIGHT, 1, 1) {
		DBOUT("Loading File");
		if (!mesh.loadFromObjectFile("teapot.obj")) {
			DBOUT("Failed to load object file" << std::endl);
		}
	}
	void updateFrame() override {
		m_console.fill(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, PIXEL_SOLID, BG_BLACK);

		if (GetAsyncKeyState((unsigned short)'A') & 0x8000) {
			m_camera.updateCameraLeft();
		}

		if (GetAsyncKeyState((unsigned short)'D') & 0x8000) {
			m_camera.updateCameraRight();
		}

		if (GetAsyncKeyState((unsigned short)'W') & 0x8000) {
			m_camera.updateCameraForward();
		}

		if (GetAsyncKeyState((unsigned short)'S') & 0x8000) {
			m_camera.updateCameraBackward();
		}
		
		// Print camera position to terminal
		DBOUT("Camera Position: " << m_camera.m_pos.x << " " << m_camera.m_pos.y << " " << m_camera.m_pos.z << std::endl);
		
		mat4x4 matProj;
		matProj.initProjectionMatrix(0.1f, 1000.0f, 90.0f, SCREEN_HEIGHT / SCREEN_WIDTH);

		// Get elapsed time since start
		float time = getElapsedTime();

		deltaTime = time - lastFrame;
		lastFrame = time;

		mat4x4 matView;
		matView.initViewMatrix(m_camera.m_pos, m_camera.m_forward, m_camera.m_up, m_camera.m_right);

		for (auto& tri : mesh.triangles) {
			tri.computeNormal();

			// We need to hide the triangles which are away from the camera, this can be done by
			// getting the dot product of the normal of the triangle and the vector from the camera to the triangle
			// If the dot product is negative, then the triangle is facing towards the camera, else it is facing away

			Triangle triProjected, triView;

			for (int i = 0; i < 3; i++) {
				matView.matrixMultiplyVector(tri.p[i], triView.p[i]);
			}

			for (int i = 0; i < 3; i++) {
				matProj.matrixMultiplyVector(triView.p[i], triProjected.p[i]);
			}
	
			// Scale into the view
			for (int i = 0; i < 3; i++) {
				// Normalize from [-1,1] to [0,2]
				triProjected.p[i].x += 1.0f;
				triProjected.p[i].y += 1.0f;
				// Convert from [0,2] to [0,screenWidth] and [0,screenHeight]
				triProjected.p[i].x *= 0.5f * SCREEN_WIDTH;
				triProjected.p[i].y *= 0.5f * SCREEN_HEIGHT;
			}
			m_console.drawTriangle(triProjected.p[0].x, triProjected.p[0].y, triProjected.p[1].x, triProjected.p[1].y, triProjected.p[2].x, triProjected.p[2].y, PIXEL_SOLID, FG_WHITE);
		}
	}
};

int main()
{
	MainGame game;
	game.start();
	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
