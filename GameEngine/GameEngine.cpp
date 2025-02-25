// GameEngine.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "engine.h"
#include "geometry.h"

class MainGame : public engine {
private:
	Cube cube = Cube(0,0,0,1);
public:
	MainGame() : engine(960, 520, 1, 1) {
	}
	void updateFrame() override {
		m_console.fill(0, 0, 960, 520, PIXEL_SOLID, BG_BLACK);

		mat4x4 matProj;
		matProj.initProjectionMatrix(0.1f, 1000.0f, 90.0f, 520.0f / 960.0f);

		for (auto& tri : cube.triangles) {
			Triangle triProjected, triTrans;

			// Rotate in 3D space
			triTrans = tri;

			float translationX = -2.5f;
			float translationY = 0.5f;
			float translationZ = 2.0f;
			
			for (int i = 0; i < 3; i++) {
				triTrans.p[i].x += translationX;
				triTrans.p[i].y += translationY;
				triTrans.p[i].z += translationZ;
			}

			for (int i = 0; i < 3; i++) {
				matProj.matrixMultiplyVector(triTrans.p[i], triProjected.p[i]);
			}
			// Scale into the view
			for (int i = 0; i < 3; i++) {
				// Normalize from [-1,1] to [0,2]
				triProjected.p[i].x += 1.0f;
				triProjected.p[i].y += 1.0f;
				// Convert from [0,2] to [0,screenWidth] and [0,screenHeight]
				triProjected.p[i].x *= 0.5f * 960.0f;
				triProjected.p[i].y *= 0.5f * 520.0f;
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
